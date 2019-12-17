#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (CopyEqAudioProcessor& proc) :
    AudioProcessorEditor (proc),
    proc (proc),
    viewer (proc)
{
    setSize (290, 400);
    startTimerHz (10);

    addAndMakeVisible (viewer);

    auto setupSlider = [=] (SliderWithAttach& slider, String name,
                            AudioProcessorValueTreeState& vts, String paramID,
                            std::function<void()> onChange = {})
    {
        addAndMakeVisible (slider.s);

        if (paramID.isNotEmpty())
            slider.att.reset (new SliderAttachment (vts, paramID, slider.s));

        slider.s.setName (name);
        slider.s.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        slider.s.setNumDecimalPlacesToDisplay (2);
        slider.s.setTextBoxStyle (Slider::TextBoxBelow, false, 75, 16);
        slider.s.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
        slider.s.onValueChange = onChange;
    };

    auto setupButton = [=] (ButtonWithAttach& button, String text, Colour onColour,
                            AudioProcessorValueTreeState& vts, String paramID,
                            bool toggleState = true, std::function<void()> onChange = {})
    {
        addAndMakeVisible (button.b);

        if (paramID.isNotEmpty())
            button.att.reset (new ButtonAttachment (vts, paramID, button.b));

        button.b.setButtonText (text);
        button.b.setColour (TextButton::buttonColourId, Colours::transparentBlack);
        button.b.setColour (TextButton::buttonOnColourId, onColour);
        button.b.setColour (TextButton::textColourOffId, Colours::white);
        button.b.setColour (TextButton::textColourOnId, onColour.contrasting());

        button.b.setClickingTogglesState (toggleState);
        button.b.onStateChange = onChange;
    };

    setupButton (learnButton,  "LEARN",      Colours::red,    proc.getVTS(), "", false);
    setupButton (contButton,   "CONTINUOUS", Colours::orange, proc.getVTS(), Tags::continID, true, [=] { stereoSlider.s.setEnabled (! contButton.b.getToggleState()); });
    setupButton (flipButton,   "FLIP",       Colours::blue,   proc.getVTS(), Tags::flipID);
    setupButton (bypassButton, "BYPASS",     Colours::yellow, proc.getVTS(), Tags::bypassID);
    learnButton.b.onClick = [&proc] { proc.triggerLearn(); };

    setupSlider (rateSlider,   "Learning Rate",    proc.getVTS(), Tags::nablaID);
    setupSlider (lengthSlider, "Learning Time",    proc.getVTS(), "", [=, &proc] { proc.setLearnLength ((float) lengthSlider.s.getValue()); });
    setupSlider (warpSlider,   "Warp Factor",      proc.getVTS(), Tags::rhoID);
    setupSlider (filtSlider,   "Sidechain Filter", proc.getVTS(), Tags::lpfID);
    setupSlider (dryWetSlider, "Dry/Wet",          proc.getVTS(), Tags::dwID);
    setupSlider (stereoSlider, "Stereo",           proc.getVTS(), Tags::stID);

    lengthSlider.s.setRange (Range<double> (0.1, 5), 0.1);
    lengthSlider.s.setValue ((double) proc.getLearnLength(), dontSendNotification);
    lengthSlider.s.setTextValueSuffix (" seconds");

    filtSlider.s.textFromValueFunction = [] (double value) { return String (value, 2) + " Hz"; };
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::timerCallback()
{
    learnButton.b.setToggleState (proc.getLearnState(), dontSendNotification);
}

void PluginEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::dodgerblue);
    g.setFont (Font (18.0f).boldened());
    g.drawFittedText ("Copy EQ", Rectangle<int> (0, 0, getWidth(), 20), Justification::centred,  1);

    g.setColour (Colours::white);
    g.setFont (Font (14.0f));
    auto makeName = [&g] (Component& comp)
    {
        auto width = g.getCurrentFont().getStringWidth (comp.getName());
        Rectangle<int> rect (comp.getBounds().getCentreX() - width / 2, comp.getY() - 7, width, 15);
        g.drawFittedText (comp.getName(), rect, Justification::centred, 1);
    };

    makeName (rateSlider.s);
    makeName (lengthSlider.s);
    makeName (warpSlider.s);
    makeName (filtSlider.s);
    makeName (dryWetSlider.s);
    makeName (stereoSlider.s);
}

void PluginEditor::resized()
{
    const int buttonWidth = 80;
    const int buttonHeight = 25;
    const int pad = 5;

    learnButton.b.setBounds  (60,  30, buttonWidth, buttonHeight);
    contButton.b.setBounds   (150, 30, buttonWidth, buttonHeight);
    flipButton.b.setBounds   (60,  learnButton.b.getBottom() + pad, buttonWidth, buttonHeight);
    bypassButton.b.setBounds (150, contButton.b.getBottom()  + pad, buttonWidth, buttonHeight);

    const int x1 = 110;
    const int x2 = 200;
    const int y1 = 105;
    const int sliderDim = 80;
    const int pad2 = 15;

    lengthSlider.s.setBounds (10, y1, sliderDim, sliderDim);    
    filtSlider.s.setBounds   (x1, y1, sliderDim, sliderDim);
    warpSlider.s.setBounds   (x2, y1, sliderDim, sliderDim);
    rateSlider.s.setBounds   (10, lengthSlider.s.getBottom() + pad2, sliderDim, sliderDim);
    stereoSlider.s.setBounds (x1, filtSlider.s.getBottom()   + pad2, sliderDim, sliderDim);
    dryWetSlider.s.setBounds (x2, warpSlider.s.getBottom()   + pad2, sliderDim, sliderDim);

    viewer.setBounds (0, getHeight() - 105, getWidth(), 105);
}
