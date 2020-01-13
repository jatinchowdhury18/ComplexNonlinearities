/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "KnobsComponent.h"

//==============================================================================
/**
*/
class SubharmonicsAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    SubharmonicsAudioProcessorEditor (SubharmonicsAudioProcessor&);
    ~SubharmonicsAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    SubharmonicsAudioProcessor& processor;

    KnobsComponent knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubharmonicsAudioProcessorEditor)
};
