#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "PluginProcessor.h"

using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;

struct ButtonWithAttach
{
    TextButton b;
    std::unique_ptr<ButtonAttachment> att;
};

struct SliderWithAttach
{
    Slider s;
    std::unique_ptr<SliderAttachment> att;
};

class PluginEditor : public AudioProcessorEditor,
                     private Timer
{
public:
    PluginEditor (CopyEqAudioProcessor& proc);
    ~PluginEditor();

    void timerCallback() override;

    void paint (Graphics&) override;
    void resized() override;

private:
    CopyEqAudioProcessor& proc;

    ButtonWithAttach learnButton;
    ButtonWithAttach contButton;
    ButtonWithAttach flipButton;
    ButtonWithAttach bypassButton;

    SliderWithAttach rateSlider;
    SliderWithAttach lengthSlider;
    SliderWithAttach filtSlider;
    SliderWithAttach warpSlider;
    SliderWithAttach dryWetSlider;
    SliderWithAttach stereoSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

#endif //PLUGINEDITOR_H_INCLUDED
