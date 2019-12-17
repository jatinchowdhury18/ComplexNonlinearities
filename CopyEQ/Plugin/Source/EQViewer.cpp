#include "EQViewer.h"

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

    curvePath.clear();
    bool started = false;
    const float scaleFactor = ((getHeight() / 2) - (getHeight()) / (8)) / 3.0f;
    for (float xPos = 0.0f; xPos < (float) getWidth(); xPos += 1.0f)
    {
        auto traceMag = Decibels::gainToDecibels (getMagForX (xPos));
        auto traceY = (getHeight() / 4) - (traceMag * scaleFactor);

        if (! started)
        {
            curvePath.startNewSubPath (xPos, traceY);
            started = true;
        }
        else
        {
            curvePath.lineTo (xPos, traceY);
        }
    }

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
