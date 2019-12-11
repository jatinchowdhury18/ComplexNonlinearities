/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Viewer.h"
#include "KnobsComponent.h"

//==============================================================================
/**
*/
class NonlienarAllpassAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    NonlienarAllpassAudioProcessorEditor (NonlienarAllpassAudioProcessor&);
    ~NonlienarAllpassAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    NonlienarAllpassAudioProcessor& processor;

    Viewer viewer;
    KnobsComponent knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonlienarAllpassAudioProcessorEditor)
};
