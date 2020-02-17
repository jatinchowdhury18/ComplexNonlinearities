#ifndef GRU_H_INCLUDED
#define GRU_H_INCLUDED

#include "JuceHeader.h"

class Gru
{
public:
    Gru()
    {
        Wf.reset (numSteps);
        Wh.reset (numSteps);
        Uf.reset (numSteps);
        Uh.reset (numSteps);
        bf.reset (numSteps);
    }

    void setParams (float newWf, float newWh, float newUf, float newUh, float newbf)
    {
        Wf.setTargetValue (newWf);
        Wh.setTargetValue (newWh);
        Uf.setTargetValue (newUf);
        Uh.setTargetValue (newUh);
        bf.setTargetValue (newbf);
    }

    void reset()
    {
        y1 = 0.0f;

        Wf.skip (numSteps);
        Wh.skip (numSteps);
        Uf.skip (numSteps);
        Uh.skip (numSteps);
        bf.skip (numSteps);
    }

    void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            buffer[n] = processSample (buffer[n], y1);
            y1 = buffer[n];
        }
    }

    inline float processSample (float x, float yPrev)
    {
        float f = sigmoid (Wf.getNextValue()*x + Uf.getNextValue()*yPrev + bf.getNextValue());
        return f*yPrev + (1.0f-f) * tanhf (Wh.getNextValue()*x + Uh.getNextValue()*f*yPrev);
    }

    inline float sigmoid (float x)
    {
        return 1.0f / (1.0f + expf (-x));
    }

private:
    const int numSteps = 200;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Wf = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Wh = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Uf = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Uh = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> bf = 0.0f;

    float y1 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gru)
};

#endif //  GRU_H_INCLUDED
