#ifndef DOUBLESOFTCLIPPER_H_INCLUDED
#define DOUBLESOFTCLIPPER_H_INCLUDED

#include "JuceHeader.h"

class DoubleSoftClipper
{
public:
    DoubleSoftClipper() {}

    inline void processBlock(float* buffer, int numSamples)
    {
        for (int n = 0; n <  numSamples; ++n)
            buffer[n] = process (buffer[n]);
    }

    inline float process (float x)
    {
        if (abs (x - x_1) < (float) 5e-2) // too close to dividing by zero
        {
            y = function (0.5f*x + 0.5f*x_1);

            F_1 = antideriv (x);
            x_1 = x;
            return y;
        }

        auto F = antideriv (x);
        y = (F - F_1) / (x - x_1);

        F_1 = F;
        x_1 = x;
        return y;
    }

    inline float function (float x)
    {
        if (x > 0.0f)
        {
            x = (x - xOff)*upperSkew;
            if (x >= oneOverSlope)
                return upperLim;
            else if (x <= -oneOverSlope)
                return 0.0f;
            
            return threeQuartersUpperLim * cubic (slope*x) + halfUpperLim;
        }

        x = (x + xOff)*lowerSkew;
        if (x >= oneOverSlope)
            return 0.0f;
        else if (x <= -oneOverSlope)
            return lowerLim;

        return threeQuartersNegLowerLim * cubic (slope*x) + halfLowerLim;
    }

    inline float cubic (float x)
    {
        return x - x*x*x/3.0f;
    }

    inline float cubic_antideriv (float x)
    {
        auto xSquareOverTwo = x*x / 2.0f;
        return xSquareOverTwo - xSquareOverTwo*xSquareOverTwo / 3.0f;
    }

    inline float antideriv (float x)
    {
        if (x > 0.0f)
        {
            x = (x - xOff)*upperSkew;
            if (x >= oneOverSlope)
                return upperLim * x;
            else if (x <= -oneOverSlope)
                return 0.0f;
            
            slopeTimesX = slope*x;
            return (threeQuartersUpperLim * cubic_antideriv (slopeTimesX) + halfUpperLim*slopeTimesX) / slope;
        }

        x = (x + xOff)*lowerSkew;
        if (x >= oneOverSlope)
            return 0.0f;
        else if (x <= -oneOverSlope)
            return lowerLim * x;

        slopeTimesX = slope*x;
        return (threeQuartersNegLowerLim * cubic_antideriv (slopeTimesX) + halfLowerLim*slopeTimesX) / slope;
    }

    void setUpperLim (float newLim);
    void setLowerLim (float newLim);
    void setSlope (float newSlopeParam);
    void setWidth (float newWidth);
    void setUpperSkew (float skewParam);
    void setLowerSkew (float skewParam);

    static float getSlopeFromParam (float param);
    static float getSkewFromParam (float param);

private:
    float upperLim = 1.0f;
    float threeQuartersUpperLim = upperLim * 0.75f;
    float halfUpperLim = upperLim / 2.0f;

    float lowerLim = -1.0f;
    float threeQuartersNegLowerLim = -0.75f * lowerLim;
    float halfLowerLim = lowerLim / 2.0f;

    float slope = 1.0f;
    float oneOverSlope = 1.0f / slope;

    float xOffFactor = 0.0f;
    float xOff = oneOverSlope * powf (slope, xOffFactor);

    float upperSkew = 1.0f;
    float lowerSkew = 1.0f;

    float x_1 = 0.0f;
    float F_1 = 0.0f;
    float y = 0.0f;
    float slopeTimesX = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoubleSoftClipper)
};

#endif //DOUBLESOFTCLIPPER_H_INCLUDED
