/*
  ==============================================================================

    Viewer.cpp
    Created: 11 Dec 2019 8:25:34am
    Author:  jatin

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Viewer.h"
#include "PluginProcessor.h"

namespace
{
    constexpr int size = 44100 / 40;
    constexpr float fs = 44100.0f;
    constexpr float freq = 400.0f;
    constexpr float gain = 1.0f;
}

//==============================================================================
Viewer::Viewer (AudioProcessorValueTreeState& vts) :
    vts (vts)
{
    proc.reset (new NonlienarAllpassAudioProcessor);

    dryBuffer.setSize (1, size);
    for (int n = 0; n < size; ++n)
        dryBuffer.setSample (0, n, gain * sinf (MathConstants<float>::twoPi * n * freq / fs));

    wetBuffer.setSize (1, size);

    startTimerHz (27);
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

void Viewer::updateCurve()
{
    auto* procCast = dynamic_cast<NonlienarAllpassAudioProcessor*> (proc.get());

    // set params
    procCast->gainParam = vts.getRawParameterValue ("gain_");
    procCast->orderParam = vts.getRawParameterValue ("order");
    procCast->satParam = vts.getRawParameterValue ("sat");
    procCast->freqParam = vts.getRawParameterValue ("freq_Hz");

    // process
    wetBuffer.makeCopyOf (dryBuffer);
    procCast->prepareToPlay ((double) fs, size);

    MidiBuffer midi;
    procCast->processBlock (wetBuffer, midi);
    procCast->releaseResources();

    // update path
    curvePath.clear();
    bool started = false;
    for (int n = size/2; n < size; ++n)
    {
        auto x = (float) (n - size / 2) / (float) (size / 2); // dryBuffer.getSample (0, n);
        auto y = wetBuffer.getSample (0, n);

        auto xDraw = x *  (float) getWidth(); // (x + (gain/1.0f)) * (float) getWidth() / (gain*2.0f);
        auto yDraw = (float) getHeight() * (0.5f - 0.22f * y);

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

void Viewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::forestgreen);
    g.strokePath (curvePath, PathStrokeType (2.0f));
}

void Viewer::resized()
{
    updateCurve();
}
