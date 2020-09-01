/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HysteresisAudioProcessorEditor::HysteresisAudioProcessorEditor (HysteresisAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    vts (p.getVTS()),
    nlViewer (p.getVTS()),
    knobs (p, p.getVTS(), [=] { nlViewer.updateCurve(); })
{
    setSize (400, 500);
    addAndMakeVisible (nlViewer);
    addAndMakeVisible (knobs);
}

HysteresisAudioProcessorEditor::~HysteresisAudioProcessorEditor()
{
}

//==============================================================================
void HysteresisAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void HysteresisAudioProcessorEditor::resized()
{
    nlViewer.setBounds (0, 0, getWidth(), getWidth());
    knobs.setBounds (0, getHeight() - 100, getWidth(), 100);
}
