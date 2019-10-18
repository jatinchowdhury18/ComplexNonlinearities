#ifndef GAIN_H_INCLUDED
#define GAIN_H_INCLUDED

#include "JuceHeader.h"

class Gain
{
public:
    Gain() {}
    ~Gain() {}

    void setGain (float newGain)
    { 
        curGain = newGain;
    }

    void prepare()
    {
        prevGain = curGain;
    }

    void processBlock (float* buffer, int numSamples)
    {
        if (curGain == prevGain)
        {
            for (int n = 0; n < numSamples; ++n)
                buffer[n] *= curGain;
        }
        else
        {
            for (int n = 0; n < numSamples; ++n)
                buffer[n] *= (curGain * (float) n / (float) numSamples) + (prevGain * (1.0f - (float) n / (float) numSamples));

            prevGain = curGain;
        }
    }

private:
    float curGain = 1.0f;
    float prevGain = curGain;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gain)
};

#endif //GAIN_H_INCLUDED
