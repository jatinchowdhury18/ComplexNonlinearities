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
class WavefolderAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    WavefolderAudioProcessorEditor (WavefolderAudioProcessor&);
    ~WavefolderAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    WavefolderAudioProcessor& processor;

    NonlinearityViewer nlViewer;

    Slider freqSlide;
    std::unique_ptr<SliderAttachment> freqAttach;

    Slider depthSlide;
    std::unique_ptr<SliderAttachment> depthAttach;

    Slider ffSlide;
    std::unique_ptr<SliderAttachment> ffAttach;

    Slider fbSlide;
    std::unique_ptr<SliderAttachment> fbAttach;

    ComboBox satBox;
    std::unique_ptr<ComboBoxAttachment> satBoxAttach;
    const StringArray satChoices = StringArray ({"None", "Hard", "Soft", "Tanh", "ASinh" });

    ComboBox waveBox;
    std::unique_ptr<ComboBoxAttachment> waveBoxAttach;
    const StringArray waveChoices = StringArray ({"Zero", "Tri", "Sine" });

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavefolderAudioProcessorEditor)
};
