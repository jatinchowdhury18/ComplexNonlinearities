/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DoubleSoftClipperAudioProcessorEditor::DoubleSoftClipperAudioProcessorEditor (DoubleSoftClipperAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    vts (p.getVTS()),
    nlViewer (p.getDSC())
{
    addAndMakeVisible (nlViewer);
    setSize (400, 500);

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

    setupSlider (upperLimSlider, vts, "upperlim", upperLimAttach, "Upper Lim", [this] { nlViewer.repaint(); });
    setupSlider (lowerLimSlider, vts, "lowerlim", lowerLimAttach, "Lower Lim", [this] { nlViewer.repaint(); });
    setupSlider (slopeSlider, vts, "slope", slopeAttach, "Slope", [this] { nlViewer.repaint(); },
                 [] (double x) { return String (DoubleSoftClipper::getSlopeFromParam ((float) x), 2); });
    setupSlider (widthSlider, vts, "width", widthAttach, "Width", [this] { nlViewer.repaint(); });
    setupSlider (upperSkewSlider, vts, "upperskew", upperSkewAttach, "Up Skew", [this] { nlViewer.repaint(); },
                 [] (double x) { return String (DoubleSoftClipper::getSkewFromParam ((float) x), 2); });
    setupSlider (lowerSkewSlider, vts, "lowerskew", lowerSkewAttach, "Low Skew", [this] { nlViewer.repaint(); },
                 [] (double x) { return String (DoubleSoftClipper::getSkewFromParam ((float) x), 2); });
}

DoubleSoftClipperAudioProcessorEditor::~DoubleSoftClipperAudioProcessorEditor()
{
}

//==============================================================================
void DoubleSoftClipperAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::white);

    auto makeLabel = [&g] (Slider& slider)
    {
        Rectangle<int> labelRect (slider.getX(), 405, slider.getWidth(), 20);
        g.drawFittedText (slider.getName(), labelRect, Justification::centredTop, 1);
    };

    makeLabel (upperLimSlider);
    makeLabel (lowerLimSlider);
    makeLabel (slopeSlider);
    makeLabel (widthSlider);
    makeLabel (upperSkewSlider);
    makeLabel (lowerSkewSlider);
}

void DoubleSoftClipperAudioProcessorEditor::resized()
{
    nlViewer.setBounds (0, 0, getWidth(), getWidth());

    upperLimSlider.setBounds (0,                           410, 67, 90);
    lowerLimSlider.setBounds (upperLimSlider.getRight(),   410, 67, 90);
    slopeSlider.setBounds (lowerLimSlider.getRight(),      410, 67, 90);
    widthSlider.setBounds (slopeSlider.getRight(),         410, 67, 90);
    upperSkewSlider.setBounds (widthSlider.getRight(),     410, 67, 90);
    lowerSkewSlider.setBounds (upperSkewSlider.getRight(), 410, 67, 90);
}
