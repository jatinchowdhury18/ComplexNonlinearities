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
#include "NLViewer.h"

//==============================================================================
/**
*/
class GatedRecurrentDistortionAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    GatedRecurrentDistortionAudioProcessorEditor (GatedRecurrentDistortionAudioProcessor&);
    ~GatedRecurrentDistortionAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    GatedRecurrentDistortionAudioProcessor& processor;

    NLViewer viewer;
    KnobsComponent knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GatedRecurrentDistortionAudioProcessorEditor)
};
