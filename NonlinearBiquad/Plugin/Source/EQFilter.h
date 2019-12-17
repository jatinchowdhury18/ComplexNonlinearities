#ifndef EQFILTER_H_INCLUDED
#define EQFILTER_H_INCLUDED

#include "Biquad.h"

enum EqShape
{
    bell,
    notch,
    highShelf,
    lowShelf,
    highPass,
    lowPass,
};

/* Filter for a single EQ band */
class EQFilter : public Biquad
{
public:
    EQFilter();
    virtual ~EQFilter() {}

    void setFrequency (float newFreq);
    void setQ (float newQ);
    void setGain (float newGain);
    void toggleOnOff (bool shouldBeOn);
    void setEqShape (EqShape newShape);
    void setOrder (float newOrder);
    void setSaturator (SatType type);

    /* Calculate filter coefficients for an EQ band (see "Audio EQ Cookbook") */
    void calcCoefsBell (float newFreq, float newQ, float newGain);
    void calcCoefsNotch (float newFreq, float newQ, float newGain);
    void calcCoefsLowShelf (float newFreq, float newQ, float newGain);
    void calcCoefsHighShelf (float newFreq, float newQ, float newGain);
    void calcCoefsLowPass (float newFreq, float newQ, float newGain);
    void calcCoefsHighPass (float newFreq, float newQ, float newGain);

    void processBlock (float* buffer, int numSamples) override;
    void applyFilterGain (float* buffer, int numSamples);
    void reset (double sampleRate) override;

private:
    SmoothedValue<float, ValueSmoothingTypes::Linear> freq;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Q;
    SmoothedValue<float, ValueSmoothingTypes::Linear> gain;
    float filterOrd = 1;

    EqShape eqShape = bell;
    typedef std::function<void (float, float, float)> CalcCoefsLambda;
    CalcCoefsLambda calcCoefs = {}; // lambda function to calculate coefficients for any shape

    bool isOn = false;
    bool isChanging = false;

    float fs = 48000.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQFilter)
};

#endif //EQFILTER_H_INCLUDED
