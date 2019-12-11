#include "KnobsComponent.h"

KnobsComponent::KnobsComponent (AudioProcessor& p, AudioProcessorValueTreeState& vts, std::function<void()> paramLambda)
{
    auto addSlider = [=, &vts] (AudioParameterFloat* param)
    {
        SliderWithAttachment* newSlide = new SliderWithAttachment;

        addAndMakeVisible (newSlide->slider);
        newSlide->attachment.reset (new SliderAttachment (vts, param->paramID, newSlide->slider));

        auto suffix = " " + param->paramID.fromLastOccurrenceOf ("_", false, false);
        newSlide->slider.setTextValueSuffix (suffix);

        newSlide->slider.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        newSlide->slider.setName (param->name);
        newSlide->slider.textFromValueFunction = nullptr; // @TODO: Don't override lambda from VTS
        newSlide->slider.setNumDecimalPlacesToDisplay (2);
        newSlide->slider.setTextBoxStyle (Slider::TextBoxBelow, false, 75, 16);
        newSlide->slider.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
        newSlide->slider.onValueChange = paramLambda;

        sliders.add (newSlide);
    };

    auto addBox = [=, &vts] (AudioParameterChoice* param)
    {
        BoxWithAttachment* newBox = new BoxWithAttachment;

        addAndMakeVisible (newBox->box);
        newBox->box.setName (param->name);
        newBox->box.addItemList (param->choices, 1);
        newBox->box.setSelectedItemIndex (0);
        newBox->box.onChange = paramLambda;

        newBox->attachment.reset (new ComboBoxAttachment (vts, param->paramID, newBox->box));

        boxes.add (newBox);
    };

    auto addButton = [=, &vts] (AudioParameterBool* param)
    {
        ButtonWithAttachment* newButton = new ButtonWithAttachment;

        addAndMakeVisible (newButton->button);
        newButton->button.setButtonText (param->name);
        newButton->button.setClickingTogglesState (true);
        newButton->button.setColour (TextButton::buttonOnColourId, Colours::red);
        newButton->button.onStateChange = paramLambda;

        newButton->attachment.reset (new ButtonAttachment (vts, param->paramID, newButton->button));

        buttons.add (newButton);
    };

    auto params = p.getParameters();

    for (auto* param : params)
    {
        if (auto* paramFloat = dynamic_cast<AudioParameterFloat*> (param))
        {
            addSlider (paramFloat);
            continue;
        }

        if (auto* paramChoice = dynamic_cast<AudioParameterChoice*> (param))
        {
            addBox (paramChoice);
            continue;
        }

        if (auto* paramBool = dynamic_cast<AudioParameterBool*> (param))
        {
            addButton (paramBool);
            continue;
        }
    }

    setSize (getWidth(), 100);
}

void KnobsComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::white);
    auto makeName = [this, &g] (Component& comp, String name)
    {
        const int height = 20;
        Rectangle<int> nameBox (comp.getX(), 2, comp.getWidth(), height);
        g.drawFittedText (name, nameBox, Justification::centred, 1);
    };

    for (auto* s : sliders)
        makeName (s->slider, s->slider.getName());

    for (auto* b : boxes)
        makeName (b->box, b->box.getName());
}

void KnobsComponent::resized()
{
    int x = 5;
    bool first = true;
    for (auto* s : sliders)
    {
        int offset = first ? 0 : 20;
        s->slider.setBounds (x - offset, 15, 85, 80);
        x = s->slider.getRight();
        first = false;
    }

    for (auto* b : boxes)
    {
        int offset = first ? 0 : 5;
        b->box.setBounds (x - offset, 40, 70, 20);
        x = b->box.getRight();
        first = false;
    }

    for (auto* b : buttons)
    {
        int offset = first ? 0 : 5;
        b->button.setBounds (x - offset, 40, 70, 20);
        x = b->button.getRight();
        first = false;
    }
}
