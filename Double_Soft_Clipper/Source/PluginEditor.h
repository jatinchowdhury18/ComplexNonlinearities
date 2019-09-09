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
class DoubleSoftClipperAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DoubleSoftClipperAudioProcessorEditor (DoubleSoftClipperAudioProcessor&);
    ~DoubleSoftClipperAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    DoubleSoftClipperAudioProcessor& processor;
    AudioProcessorValueTreeState& vts;

    NonlinearityViewer nlViewer;

    Slider upperLimSlider;
    std::unique_ptr<SliderAttachment> upperLimAttach;

    Slider lowerLimSlider;
    std::unique_ptr<SliderAttachment> lowerLimAttach;

    Slider slopeSlider;
    std::unique_ptr<SliderAttachment> slopeAttach;

    Slider widthSlider;
    std::unique_ptr<SliderAttachment> widthAttach;

    Slider upperSkewSlider;
    std::unique_ptr<SliderAttachment> upperSkewAttach;

    Slider lowerSkewSlider;
    std::unique_ptr<SliderAttachment> lowerSkewAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoubleSoftClipperAudioProcessorEditor)
};
