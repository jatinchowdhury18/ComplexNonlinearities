#ifndef EQVIEWER_H_INCLUDED
#define EQVIEWER_H_INCLUDED

#include "JuceHeader.h"
#include "PluginProcessor.h"

class EQViewer : public Component,
                 private Timer
{
public:
    EQViewer (CopyEqAudioProcessor& proc);
    ~EQViewer();

    void paint (Graphics&) override;
    void resized() override;

    void updateCurve();

private:
    void timerCallback() override;
    void doFFT();
    float getMagForX (float x);

    Path curvePath;
    CopyEqAudioProcessor& proc;

    dsp::FFT forwardFFT;
    std::unique_ptr<float[]> fftBuffer;
    std::unique_ptr<float[]> H;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQViewer)
};

#endif //EQVIEWER_H_INCLUDED
