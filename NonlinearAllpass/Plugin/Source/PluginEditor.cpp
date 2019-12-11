/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NonlienarAllpassAudioProcessorEditor::NonlienarAllpassAudioProcessorEditor (NonlienarAllpassAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    viewer (p.getVTS()),
    knobs (p, p.getVTS(), [this] { viewer.setNeedsUpdateCurve(); })
{
    setSize (400, 500);

    addAndMakeVisible (knobs);
    addAndMakeVisible (viewer);
}

NonlienarAllpassAudioProcessorEditor::~NonlienarAllpassAudioProcessorEditor()
{
}

//==============================================================================
void NonlienarAllpassAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void NonlienarAllpassAudioProcessorEditor::resized()
{
    viewer.setBounds (0, 0, getWidth(), getWidth());
    knobs.setBounds (0, getHeight() - 100, getWidth(), 100);
}
