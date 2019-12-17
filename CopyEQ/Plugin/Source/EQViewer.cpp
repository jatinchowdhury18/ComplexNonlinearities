#include "EQViewer.h"
#include "Spline.h"

namespace
{
    constexpr int order = 8;
    constexpr int size = 1 << order;
    constexpr float fs = 44100.0f;
    constexpr float fLow = 20.0f;
    constexpr float fHigh = 22000.0f;
}

EQViewer::EQViewer (CopyEqAudioProcessor& proc) :
    proc (proc),
    forwardFFT (order)
{
    startTimerHz (10);

    fftBuffer.reset (new float[2*size]);
    H.reset (new float[size]);
}

EQViewer::~EQViewer()
{
}

void EQViewer::timerCallback()
{
    updateCurve();
}

float EQViewer::getMagForX (float x)
{
    float width = (float) getWidth();
    float freq = fLow * powf ((fHigh / fLow), ((x - 2.5f) / (width - 5.0f)));
    int index = int ((size/2) * freq / (fs/2.0f));

    return H[index];
}

void EQViewer::doFFT()
{
    auto* filtL = proc.getFilter (0).get();
    auto* filtR = proc.getFilter (1).get();

    if (filtL == nullptr || filtR == nullptr)
        return;

    if (filtL->h.get() == nullptr || filtR->h.get() == nullptr)
        return;

    for (int n = 0; n < filtL->order; ++n)
    {
        fftBuffer[n] = (filtL->h[n] + filtR->h[n]) / 2.0f;
    }
    for (int n = filtL->order; n < 2*size; ++n)
        fftBuffer[n] = 0.0f;

    forwardFFT.performFrequencyOnlyForwardTransform (fftBuffer.get());

    const int avgLength = 30;
    for (int n = 0; n < size - avgLength; ++n)
    {
        float sum = 0.0f;
        for (int k = 0; k < avgLength; ++k)
            sum += fftBuffer[n+k];
        H[n] = sum / (float) avgLength;
    }

    float max = 0.0f;
    for (int n = 0; n < size; ++n)
    {
        if (abs(H[n]) > max)
            max = abs (H[n]);
    }

    for (int n = 0; n < size; ++n)
        H[n] /= max;
}

void EQViewer::updateCurve()
{
    doFFT();

    Array<Point<double>> points;

    const auto sr = proc.getSampleRate() == 0.0 ? proc.getSampleRate() : 44100.0;
    const auto binWidth = (float) sr / (float) size;
    const float scaleFactor = ((getHeight() / 2) - (getHeight()) / (8)) / 3.0f;
    for (int k = 0; k < size / 2; ++k)
    {
        auto freq = k * binWidth;
        float xPos = 0.0f;

        if (freq > fLow)
        {
            auto exp = logf (freq / fLow) / logf (fHigh / fLow);
            xPos = exp * ((float) getWidth() - 5.0f) + 2.5f;
        }
        else if (freq > fHigh)
        {
            break;
        }

        auto traceMag = Decibels::gainToDecibels (H[k]);
        auto traceY = (getHeight() / 4) - (traceMag * scaleFactor);

        points.add (Point<double> ((double) xPos, (double) traceY));
    }

    Spline spline (points);

    curvePath.clear();
    curvePath.startNewSubPath (points.getFirst().toFloat());
    for (int x = (int) points.getFirst().getX(); x < (int) points.getLast().getX(); ++x)
    {
        float y = (float) spline.interpolate ((double) x);
        curvePath.lineTo ((float) x, (float) y);
    }
    curvePath.lineTo (points.getLast().toFloat());

    repaint();
}

void EQViewer::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::white);
    g.strokePath (curvePath, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void EQViewer::resized()
{
}
