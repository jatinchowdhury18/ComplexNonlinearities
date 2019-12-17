#ifndef COPYEQ_H_INCLUDED
#define COPYEQ_H_INCLUDED

#include "FIRFilter.h"
#include "APF1.h"

using Filter = dsp::StateVariableFilter::Filter<float>;
using Parameters = dsp::StateVariableFilter::Parameters<float>;

class CopyEQ
{
public:
    CopyEQ (std::unique_ptr<FIRFilter>& thisF, std::unique_ptr<FIRFilter>& otherF);

    void setNabla (float newNabla) { nabla = newNabla; }
    void setRho (float newRho);
    void setFlip (bool shouldBeFlip) { flip = shouldBeFlip; }
    void setSideCutoff (float freq);
    void setStereoFactor (float factor) { stFactor = factor; }

    void reset (double sampleRate, int samplesPerBlock);

    void processBlock (float* buffer, const int numSamples);
    void processBlockLearn (float* mainBuffer, float* sideBuffer, const int numSamples);

private:
    std::unique_ptr<FIRFilter>& filter;
    std::unique_ptr<FIRFilter>& otherFilter;
    float fs = 44100.0f;
    const int order = 192;
    int fsFactor = (int) (fs / 44100.0f);

    float nabla = (float) 1.0e-6; // 0.0001f;
    
    APF1 inWarp;
    APF1 outWarp;
    APF1 sideWarp;
    Filter sideFilter;

    float stFactor = 1.0f;

    bool flip = false;
    Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CopyEQ)
};

#endif //COPYEQ_H_INCLUDED
