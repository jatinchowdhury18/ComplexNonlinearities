/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SubharmonicsAudioProcessorEditor::SubharmonicsAudioProcessorEditor (SubharmonicsAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    knobs (p, p.getVTS(), [&p] { p.updateParams(); })
{
    setSize (450, 100);
    addAndMakeVisible (knobs);
}

SubharmonicsAudioProcessorEditor::~SubharmonicsAudioProcessorEditor()
{
}

//==============================================================================
void SubharmonicsAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void SubharmonicsAudioProcessorEditor::resized()
{
    knobs.setBounds (0, getHeight() - 100, getWidth(), 100);
}
