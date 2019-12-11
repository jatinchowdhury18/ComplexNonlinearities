#ifndef KNOBSCOMPONENT_H_INCLUDED
#define KNOBSCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;

struct SliderWithAttachment
{
    Slider slider;
    std::unique_ptr<SliderAttachment> attachment;
};

struct BoxWithAttachment
{
    ComboBox box;
    std::unique_ptr<ComboBoxAttachment> attachment;
};

struct ButtonWithAttachment
{
    TextButton button;
    std::unique_ptr<ButtonAttachment> attachment;
};

class KnobsComponent : public Component
{
public:
    KnobsComponent (AudioProcessor& p, AudioProcessorValueTreeState& vts, std::function<void()> paramLambda = {});

    void paint (Graphics& g) override;
    void resized() override;

private:
    OwnedArray<SliderWithAttachment> sliders;
    OwnedArray<BoxWithAttachment> boxes;
    OwnedArray<ButtonWithAttachment> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobsComponent)
};

#endif //KNOBSCOMPONENT_H_INCLUDED
