#include "NLViewer.h"

namespace
{
    constexpr int size = 44100 / 10;
    constexpr float fs = 44100.0f;
    constexpr float freq = 100.0f;
    constexpr float gain = 1.0f;
}

NLViewer::NLViewer (AudioProcessorValueTreeState& vts) :
    vts (vts)
{
    startTimerHz (25);

    dryBuffer.setSize (1, size);
    for (int n = 0; n < size; ++n)
        dryBuffer.setSample (0, n, gain * sinf (MathConstants<float>::twoPi * n * freq / fs));

    wetBuffer.setSize (1, size);
}

void NLViewer::timerCallback()
{
    updateCurve();
}

void NLViewer::updateCurve()
{
    wetBuffer.makeCopyOf (dryBuffer);

    gru.setParams (*vts.getRawParameterValue ("wf_"), *vts.getRawParameterValue ("wh_"),
                   *vts.getRawParameterValue ("uf_"), *vts.getRawParameterValue ("uh_"),
                   *vts.getRawParameterValue ("bf_"));

    gru.reset();
    gru.processBlock (wetBuffer.getWritePointer (0), size);

    const auto yFactor = 0.45f;

    curvePath.clear();
    bool started = false;
    for (int n = size/2; n <  size; ++n)
    {
        auto x = dryBuffer.getSample (0, n);
        auto y = wetBuffer.getSample (0, n);

        auto xDraw = (x +  (gain/1.0f)) * (float) getWidth() / (gain*2);
        auto yDraw = (float) getHeight() * (0.5f - yFactor * y);

        if (! started)
        {
            curvePath.startNewSubPath (xDraw, yDraw);
            started = true;
        }
        else
        {
            curvePath.lineTo (xDraw, yDraw);
        }
    }

    repaint();
}

void NLViewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::blue);
    g.strokePath (curvePath, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void NLViewer::resized()
{
    updateCurve();
}
