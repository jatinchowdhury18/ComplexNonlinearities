/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "NonlinearityViewer.h"
#include "../../../shared_code/KnobsComponent.h"

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using BoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
/**
*/
class HysteresisAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    HysteresisAudioProcessorEditor (HysteresisAudioProcessor&);
    ~HysteresisAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    HysteresisAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;

    NonlinearityViewer nlViewer;
    KnobsComponent knobs;

    // Slider widthSlide;
    // std::unique_ptr<SliderAttachment> widthAttach;
    // 
    // Slider driveSlide;
    // std::unique_ptr<SliderAttachment> driveAttach;
    // 
    // Slider satSlide;
    // std::unique_ptr<SliderAttachment> satAttach;
    // 
    // ComboBox osBox;
    // std::unique_ptr<BoxAttachment> osAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HysteresisAudioProcessorEditor)
};
