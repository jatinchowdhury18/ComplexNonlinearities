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
    constexpr int size = 16000 / 32;
    constexpr float fs = 16000.0f;
    constexpr float freq = 200.0f;
    constexpr float gain = 1.0f;
}

NonlinearityViewer::NonlinearityViewer (AudioProcessorValueTreeState& vts) :
    processor (vts)
{
    dryBuffer.setSize (1, size);
    for (int n = 0; n < size; ++n)
        dryBuffer.setSample (0, n, gain * sinf (MathConstants<float>::twoPi * n * freq / fs));

    wetBuffer.setSize (1, size);

    processBuffer();
}

NonlinearityViewer::~NonlinearityViewer()
{
}

void NonlinearityViewer::processBuffer()
{
    MidiBuffer mBuffer;
    wetBuffer.makeCopyOf (dryBuffer);

    processor.prepareToPlay ((double) fs, size);
    processor.processBlock (wetBuffer, mBuffer);
    processor.releaseResources();
}

void NonlinearityViewer::updateCurve()
{
    processBuffer();

    curvePath.clear();
    bool started = false;
    for (int n = 3*size/4; n < size; ++n)
    {
        auto x = dryBuffer.getSample (0, n);
        auto y = wetBuffer.getSample (0, n);

        auto xDraw = (x + 1.0f) * (float) getWidth() / 2.0f;
        auto yDraw = (float) getHeight() * (0.5f - 0.4f * y);

        if (std::isnan (xDraw) || std::isnan (yDraw))
            return;

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
