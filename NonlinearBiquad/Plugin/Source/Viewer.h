#ifndef VIEWER_H_INCLUDED
#define VIEWER_H_INCLUDED

#include "EQFilter.h"

class Viewer : public Component,
               private Timer
{
public:
    Viewer (AudioProcessorValueTreeState& vts);
    ~Viewer();

    void paint (Graphics&) override;
    void resized() override;

    void setNeedsCurveUpdate (bool needsUpdate) { needsCurveUpdate.exchange (needsUpdate); }
    void updateCurve();

private:
    void timerCallback() override;
    Path curvePath;
    std::atomic_bool needsCurveUpdate;

    EQFilter processor;
    AudioProcessorValueTreeState& vts;

    void processBuffer();
    AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;

    float getMagnitudeForX (float freq);
    dsp::FFT forwardFFT;

    std::unique_ptr<float[]> dryFFTBuffer;
    std::unique_ptr<float[]> wetFFTBuffer;
    std::unique_ptr<float[]> H;

    std::atomic<float>*  eqShapeParameter;
    std::atomic<float>*  eqFreqParameter;
    std::atomic<float>*  eqQParameter;
    std::atomic<float>*  eqGainParameter;
    std::atomic<float>*  driveParameter;
    std::atomic<float>*  satParameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Viewer)
};

#endif //VIEWER_H_INCLUDED
