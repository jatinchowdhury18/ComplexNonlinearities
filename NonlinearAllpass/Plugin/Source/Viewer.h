/*
  ==============================================================================

    Viewer.h
    Created: 11 Dec 2019 8:25:34am
    Author:  jatin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Viewer : public Component,
               private Timer
{
public:
    Viewer (AudioProcessorValueTreeState& vts);
    ~Viewer();

    void paint (Graphics&) override;
    void resized() override;

    void setNeedsUpdateCurve() { needsCurveUpdate.exchange (true); }

private:
    void updateCurve();
    void timerCallback() override;
    std::atomic_bool needsCurveUpdate;

    Path curvePath;

    std::unique_ptr<AudioProcessor> proc;
    AudioProcessorValueTreeState& vts;

    AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Viewer)
};
