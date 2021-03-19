#include "Viewer.h"

namespace
{
    constexpr int order = 15;
    constexpr int size = 1 << order;
    constexpr float fs = 44100.0f;
    constexpr float fLow = 20.0f;
    constexpr float fHigh = 20000.0f;
}

Viewer::Viewer (AudioProcessorValueTreeState& vts) :
    vts (vts),
    forwardFFT (order)
{
    needsCurveUpdate = false;
    startTimerHz (27);

    eqShapeParameter    = vts.getRawParameterValue ("shape");
    eqFreqParameter     = vts.getRawParameterValue ("freq");
    eqQParameter        = vts.getRawParameterValue ("q");
    eqGainParameter     = vts.getRawParameterValue ("gain");
    driveParameter      = vts.getRawParameterValue ("drivegain");
    satParameter        = vts.getRawParameterValue ("sat");

    dryFFTBuffer.reset (new float[2*size]);
    wetFFTBuffer.reset (new float[2*size]);
    H.reset (new float[size]);

    dryBuffer.setSize (1, size);

    const float beta = (float) size * logf (fHigh / fLow);
    const float phi = MathConstants<float>::pi / 180.0f;

    for (int n = 0; n < size; ++n)
    {
        float phase = MathConstants<float>::twoPi * beta * fLow * (powf (fHigh / fLow, (float) n / (float) size) - 1.0f);
        dryBuffer.setSample (0, n, cosf ((phase + phi) / fs));
        dryFFTBuffer[n] = dryBuffer.getSample (0, n);
    }

    forwardFFT.performFrequencyOnlyForwardTransform (dryFFTBuffer.get());

    wetBuffer.setSize (1, size);

    processor.toggleOnOff (true);
    processBuffer();
}

Viewer::~Viewer()
{
}

void Viewer::timerCallback()
{
    if (! needsCurveUpdate)
        return;

    updateCurve();
    needsCurveUpdate.exchange (false);
}

void Viewer::processBuffer()
{
    processor.setEqShape (static_cast<EqShape> ((int) eqShapeParameter->load()));
    processor.setFrequency (eqFreqParameter->load());
    processor.setQ (eqQParameter->load());
    processor.setGain (Decibels::decibelsToGain (eqGainParameter->load()));
    processor.setSaturator (static_cast<SatType> ((int) satParameter->load()));

    wetBuffer.clear();
    wetBuffer.makeCopyOf (dryBuffer);
    wetBuffer.applyGain (Decibels::decibelsToGain (driveParameter->load()));

    processor.reset ((double) fs);
    processor.processBlock (wetBuffer.getWritePointer (0), size);

    wetBuffer.applyGain (Decibels::decibelsToGain (-1.0f * driveParameter->load()));

    for (int n = 0; n < size; ++n)
    {
        wetFFTBuffer[n] = wetBuffer.getSample (0, n);
    }

    forwardFFT.performFrequencyOnlyForwardTransform (wetFFTBuffer.get());

    float HTemp [size];
    for (int n = 0; n < size; ++n)
    {
        HTemp[n] = std::abs (wetFFTBuffer[n] / dryFFTBuffer[n]);
    }

    const int avgLength = 50;
    for (int n = 0; n < size - avgLength; ++n)
    {
        float sum = 0.0f;
        for (int k = 0; k < avgLength; ++k)
            sum += HTemp[n+k];
        H[n] = sum / (float) avgLength;
    }
}

float Viewer::getMagnitudeForX (float x)
{
    float width = (float) getWidth();
    float freq = fLow * pow ((fHigh / fLow), ((x - 2.5f) / (width - 5.0f)));
    int index = int ((size/2) * freq / (fs/2.0f));

    return H[index];
}

void Viewer::updateCurve()
{
    processBuffer();

    curvePath.clear();
    bool started = false;
    const float scaleFactor = ((getHeight() / 2) - (getHeight()) / (8)) / 15.0f;
    for (float xPos = 0.0f; xPos < (float) getWidth(); xPos += 1.0f)
    {
        auto traceMag = Decibels::gainToDecibels (getMagnitudeForX (xPos));
        auto traceY = (getHeight() / 2) - (traceMag * scaleFactor);

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

void Viewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::forestgreen);
    g.strokePath (curvePath, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void Viewer::resized()
{
    updateCurve();
}
