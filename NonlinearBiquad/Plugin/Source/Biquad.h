#ifndef BIQUAD_H_INCLUDED
#define BIQUAD_H_INCLUDED

#include "JuceHeader.h"
#include "Saturators.h"

class Biquad
{
public:
    Biquad()
    {
        a.reset (new float[order+1]);
        b.reset (new float[order+1]);

        z.reset (new float[order+1]);
    }
    virtual ~Biquad() {}

    virtual void reset (double sampleRate)
    {
        // clear coefficients
        for (int n = 0; n < order+1; ++n)
            z[n] = 0.0f;

        fs = (float) sampleRate;
    }

    virtual inline float process (float x)
    {
        // process input sample, direct form II transposed
        float y = z[1] + x*b[0];
        z[1] = saturator (z[2] + x*b[1] - y*a[1]);
        z[2] = saturator (x*b[2] - y*a[2]);

        return y;
    }

    virtual void calcCoefs (float /*fc*/, float /*Q*/) {}

    virtual void processBlock (float* buffer, int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
            buffer[n] = process (buffer[n]);
    }

protected:
    float fs = 48000.0f;
    std::unique_ptr<float[]> a; //IIR Coefficients
    std::unique_ptr<float[]> b; //FIR Coefficients
    std::unique_ptr<float[]> z; // Filter state

    SatFunc saturator;

private:
    const int order = 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Biquad)
};

#endif //BIQUAD_H_INCLUDED
