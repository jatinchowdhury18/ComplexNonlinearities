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

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;

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

    Slider widthSlide;
    std::unique_ptr<SliderAttachment> widthAttach;

    Slider driveSlide;
    std::unique_ptr<SliderAttachment> driveAttach;

    Slider satSlide;
    std::unique_ptr<SliderAttachment> satAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HysteresisAudioProcessorEditor)
};
