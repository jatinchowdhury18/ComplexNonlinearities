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
    constexpr float gain = 1.0f;
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
    auto drive = *vts.getRawParameterValue ("drivegain");
    processor.setDrive (drive / 100.0f);
    processor.setControlGain (ExciterProcessor::getControlGainFromDrive (drive / 100.0f));
    processor.setDetectorFreq (*vts.getRawParameterValue ("freq"));
    
    auto rectType = static_cast<RectifierType> ((int) *vts.getRawParameterValue ("rect"));
    processor.setRectifierType (rectType);
    processor.setSaturator (static_cast<SaturatorType> ((int) *vts.getRawParameterValue ("sat")));

    auto yFactor = 0.9f;
    if (rectType == FWR)
        yFactor *= 0.2f;
    else if (rectType == HWR)
        yFactor *= 0.15f;
    else if (rectType == Diode)
        yFactor *= 0.04f;

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
