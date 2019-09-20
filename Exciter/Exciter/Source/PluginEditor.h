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
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
/**
*/
class ExciterAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    ExciterAudioProcessorEditor (ExciterAudioProcessor&);
    ~ExciterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    ExciterAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;

    NonlinearityViewer nlViewer;

    ComboBox rectBox;
    std::unique_ptr<ComboBoxAttachment> rectBoxAttach;
    const StringArray rectChoices = StringArray ({"FWR", "HWR", "Diode" });

    Slider freqSlide;
    std::unique_ptr<SliderAttachment> freqAttach;

    Slider driveSlide;
    std::unique_ptr<SliderAttachment> driveAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExciterAudioProcessorEditor)
};
