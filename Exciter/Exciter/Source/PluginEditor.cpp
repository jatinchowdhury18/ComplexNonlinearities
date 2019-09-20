/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ExciterAudioProcessorEditor::ExciterAudioProcessorEditor (ExciterAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    vts (p.getVTS()),
    nlViewer (p.getVTS())
{
    setSize (400, 500);
    addAndMakeVisible (nlViewer);

    auto setupBox = [this] (ComboBox& box, AudioProcessorValueTreeState& vts, String paramID,
        std::unique_ptr<ComboBoxAttachment>& attachment, StringArray choices,
        std::function<void()> onChange = {})
    {
        addAndMakeVisible (box);
        box.addItemList (choices, 1);
        box.setSelectedItemIndex (0);
        box.onChange = onChange;

        attachment.reset (new ComboBoxAttachment (vts, paramID, box));
    };

    setupBox (rectBox, vts, "rect", rectBoxAttach, rectChoices, [this] { nlViewer.updateCurve(); });

    auto setupSlider = [this] (Slider& slider, AudioProcessorValueTreeState& vts, String paramID,
        std::unique_ptr<SliderAttachment>& attachment, String name = {},
        std::function<void()> onValueChange = {}, std::function<String (double)> textFromValue = {},
        std::function<double (String)> valueFromText = {})
    {
        addAndMakeVisible (slider);
        attachment.reset (new SliderAttachment (vts, paramID, slider));

        slider.setSliderStyle (Slider::RotaryVerticalDrag);
        slider.setName (name);
        slider.textFromValueFunction = textFromValue;
        slider.valueFromTextFunction = valueFromText;
        slider.setNumDecimalPlacesToDisplay (2);
        slider.setTextBoxStyle (Slider::TextBoxBelow, false, 60, 15);
        slider.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
        slider.onValueChange = onValueChange;
    };

    setupSlider (freqSlide,  vts, "freq",      freqAttach,  "Freq",  [this] { nlViewer.updateCurve(); });
    setupSlider (driveSlide, vts, "drivegain", driveAttach, "Drive", [this] { nlViewer.updateCurve(); });
}

ExciterAudioProcessorEditor::~ExciterAudioProcessorEditor()
{
}

//==============================================================================
void ExciterAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void ExciterAudioProcessorEditor::resized()
{
    nlViewer.setBounds (0, 0, getWidth(), getWidth());

    rectBox.setBounds      (5,                       440, 80, 20);
    freqSlide.setBounds    (rectBox.getRight(),      400, 67, 90);
    driveSlide.setBounds   (freqSlide.getRight(),    400, 67, 90);
}
