#ifndef APF1_H_INCLUDED
#define APF1_H_INCLUDED

#include "JuceHeader.h"

class APF1
{
public:
    APF1() {}

    void setRho (float rho) { p = rho; }

    void reset()
    {
        z = 0.0f;
    }

    inline float process (float x)
    {
        float y = z + p*x;
        z = x + p*y;
        return y;
    }

private:
    float p = 0.0f;
    float z = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APF1)
};

#endif //APF1_H_INCLUDED
