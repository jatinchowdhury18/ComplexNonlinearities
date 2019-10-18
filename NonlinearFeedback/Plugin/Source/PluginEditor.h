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

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
/**
*/
class NonlinearFeedbackAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    NonlinearFeedbackAudioProcessorEditor (NonlinearFeedbackAudioProcessor&);
    ~NonlinearFeedbackAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    NonlinearFeedbackAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;

    Viewer viewer;

    Slider freqSlide;
    std::unique_ptr<SliderAttachment> freqAttach;

    Slider qSlide;
    std::unique_ptr<SliderAttachment> qAttach;

    Slider driveSlide;
    std::unique_ptr<SliderAttachment> driveAttach;

    ComboBox satBox;
    std::unique_ptr<ComboBoxAttachment> satBoxAttach;
    const StringArray satChoices = StringArray ({"None", "Hard", "Soft", "Tanh", "ASinh" });

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonlinearFeedbackAudioProcessorEditor)
};
