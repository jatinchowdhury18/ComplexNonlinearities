/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NonlinearFeedbackAudioProcessorEditor::NonlinearFeedbackAudioProcessorEditor (NonlinearFeedbackAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    vts (p.getVTS()),
    viewer (vts)
{
    setSize (400, 500);
    addAndMakeVisible (viewer);

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

    setupBox (satBox,   vts, "sat",   satBoxAttach,   satChoices,   [this] { viewer.setNeedsCurveUpdate (true); });

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

    setupSlider (freqSlide,  vts, "freq",      freqAttach,  "Freq",  [this] { viewer.setNeedsCurveUpdate (true); });
    setupSlider (qSlide,     vts, "q",         qAttach,     "Q",     [this] { viewer.setNeedsCurveUpdate (true); });
    setupSlider (driveSlide, vts, "drivegain", driveAttach, "Drive", [this] { viewer.setNeedsCurveUpdate (true); });

    freqSlide.setTextValueSuffix (" Hz");
    freqSlide.setNumDecimalPlacesToDisplay (0);

    qSlide.setSkewFactorFromMidPoint (0.707f);
    driveSlide.setTextValueSuffix (" dB");
}

NonlinearFeedbackAudioProcessorEditor::~NonlinearFeedbackAudioProcessorEditor()
{
}

//==============================================================================
void NonlinearFeedbackAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::white);
    auto makeName = [this, &g] (Component& comp, String name)
    {
        const int height = 20;
        Rectangle<int> nameBox (comp.getX(), 402, comp.getWidth(), height);
        g.drawFittedText (name, nameBox, Justification::centred, 1);
    };

    makeName (freqSlide,  "Frequency");
    makeName (qSlide,     "Q");
    makeName (driveSlide, "Drive");
    makeName (satBox,     "Saturator");
}

void NonlinearFeedbackAudioProcessorEditor::resized()
{
    viewer.setBounds (0, 0, getWidth(), getWidth());

    freqSlide.setBounds    (5,                          415, 90, 80);
    qSlide.setBounds       (freqSlide.getRight()  - 20, 415, 90, 80);
    driveSlide.setBounds   (qSlide.getRight()     - 20, 415, 90, 80);
    satBox.setBounds       (driveSlide.getRight() - 00, 440, 90, 20);
}
