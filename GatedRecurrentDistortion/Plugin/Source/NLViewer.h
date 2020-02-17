#ifndef NLVIEWER_H_INCLUDED
#define NLVIEWER_H_INCLUDED

#include "GRU.h"

class NLViewer : public Component,
                 private Timer
{
public:
    NLViewer (AudioProcessorValueTreeState& vts);

    void updateCurve();

    void paint (Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    AudioProcessorValueTreeState& vts;

    AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;
    Gru gru;

    Path curvePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NLViewer)
};

#endif // NLVIEWER_H_INCLUDED
