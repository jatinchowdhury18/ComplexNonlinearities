/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GatedRecurrentDistortionAudioProcessorEditor::GatedRecurrentDistortionAudioProcessorEditor (GatedRecurrentDistortionAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    viewer (p.getVTS()),
    knobs (p, p.getVTS(), [&p] { p.updateParams(); })
{
    setSize (400, 400);

    addAndMakeVisible (viewer);
    addAndMakeVisible (knobs);
}

GatedRecurrentDistortionAudioProcessorEditor::~GatedRecurrentDistortionAudioProcessorEditor()
{
}

//==============================================================================
void GatedRecurrentDistortionAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void GatedRecurrentDistortionAudioProcessorEditor::resized()
{
    viewer.setBounds (0, 0, getWidth(), getHeight() - 100);
    knobs.setBounds (0, getHeight() - 100, getWidth(), 100);
}
