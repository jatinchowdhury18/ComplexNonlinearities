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
    nlViewer (p.getVTS())
{
    setSize (400, 500);
    addAndMakeVisible (nlViewer);

    auto setupSlider = [this] (Slider& slider, AudioProcessorValueTreeState& vts, String paramID,
        std::unique_ptr<SliderAttachment>& attachment, String name = {},
        std::function<void()> onValueChange = {}, std::function<String (double)> textFromValue = {},
        std::function<double (String)> valueFromText = {})
    {
        addAndMakeVisible (slider);
        attachment.reset (new SliderAttachment (vts, paramID, slider));

        slider.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        slider.setName (name);
        slider.textFromValueFunction = textFromValue;
        slider.valueFromTextFunction = valueFromText;
        slider.setNumDecimalPlacesToDisplay (2);
        slider.setTextBoxStyle (Slider::TextBoxBelow, false, 60, 15);
        slider.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
        slider.onValueChange = onValueChange;
    };

    setupSlider (widthSlide, vts, "width",     widthAttach, "Width",      [this] { nlViewer.updateCurve(); });
    setupSlider (driveSlide, vts, "drivegain", driveAttach, "Drive",      [this] { nlViewer.updateCurve(); });
    setupSlider (satSlide,   vts, "sat",       satAttach,   "Saturation", [this] { nlViewer.updateCurve(); });
}

HysteresisAudioProcessorEditor::~HysteresisAudioProcessorEditor()
{
}

//==============================================================================
void HysteresisAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::white);
    auto makeName = [this, &g] (Component& comp, String name)
    {
        const int height = 20;
        Rectangle<int> nameBox (comp.getX(), 402, comp.getWidth(), height);
        g.drawFittedText (name, nameBox, Justification::centred, 1);
    };

    makeName (widthSlide, "Width");
    makeName (driveSlide, "Drive");
    makeName (satSlide,   "Saturation");
}

void HysteresisAudioProcessorEditor::resized()
{
    nlViewer.setBounds (0, 0, getWidth(), getWidth());

    widthSlide.setBounds   (5,                       415, 80, 80);
    driveSlide.setBounds   (widthSlide.getRight(),   415, 80, 80);
    satSlide.setBounds     (driveSlide.getRight(),   415, 80, 80);
}
