/*
  ==============================================================================

    NonlinearityViewer.cpp
    Created: 8 Sep 2019 4:01:55pm
    Author:  jatin

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NonlinearityViewer.h"

namespace
{
    constexpr int size = 44100 / 4;
    constexpr float fs = 44100.0f;
    constexpr float freq = 100.0f;
    constexpr float gain = 10.0f;
}

NonlinearityViewer::NonlinearityViewer (AudioProcessorValueTreeState& vts) :
    vts (vts)
{
    dryBuffer.setSize (1, size);
    for (int n = 0; n < size; ++n)
        dryBuffer.setSample (0, n, gain * sinf (MathConstants<float>::twoPi * n * freq / fs));

    wetBuffer.setSize (1, size);
}

NonlinearityViewer::~NonlinearityViewer()
{
}

void NonlinearityViewer::updateCurve()
{
    processor.setFreq (*vts.getRawParameterValue ("freq"));
    processor.setDepth (*vts.getRawParameterValue ("depth"));
    processor.setFF (*vts.getRawParameterValue ("feedforward"));
    processor.setFB (*vts.getRawParameterValue ("feedback"));
    processor.setSatType (static_cast<SatType> ((int) *vts.getRawParameterValue ("sat")));
    processor.setWaveType (static_cast<WaveType> ((int) *vts.getRawParameterValue ("wave")));

    const auto yFactor = 0.1f;

    wetBuffer.makeCopyOf (dryBuffer);
    processor.reset (fs);
    processor.processBlock (wetBuffer.getWritePointer (0), size);

    curvePath.clear();
    bool started = false;
    for (int n = size/2; n < size; ++n)
    {
        auto x = dryBuffer.getSample (0, n);
        auto y = wetBuffer.getSample (0, n);

        auto xDraw = (x + (gain/1.0f)) * (float) getWidth() / (gain*2.0f);
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

void NonlinearityViewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::forestgreen);
    g.strokePath (curvePath, PathStrokeType (2.0f));
}

void NonlinearityViewer::resized()
{
    updateCurve();
}
