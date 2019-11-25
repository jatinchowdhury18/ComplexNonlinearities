#ifndef COPYEQ_H_INCLUDED
#define COPYEQ_H_INCLUDED

#include "FIRFilter.h"
#include "APF1.h"

class CopyEQ
{
public:
    CopyEQ();

    void setNabla (float newNabla) { nabla = newNabla; }
    void setRho (float newRho);
    void setFlip (bool shouldBeFlip) { flip = shouldBeFlip; }
    void setWarpSide (bool shouldWarpSide) { warpSide = shouldWarpSide; }

    void reset (double sampleRate);

    void processBlock (float* mainBuffer, float* sideBuffer, const int numSamples);

private:
    std::unique_ptr<FIRFilter> filter;
    float fs = 44100.0f;
    const int order = 128;
    int fsFactor = (int) (fs / 44100.0f);

    float nabla = (float) 1.0e-6; // 0.0001f;
    
    APF1 inWarp;
    APF1 outWarp;
    APF1 sideWarp;
    bool warpSide = true;

    bool flip = false;
    Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CopyEQ)
};

#endif //COPYEQ_H_INCLUDED
