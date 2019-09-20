#ifndef LEVELDETECTOR_H_INCLUDED
#define LEVELDETECTOR_H_INCLUDED

#include "JuceHeader.h"

enum RectifierType
{
    FWR,
    HWR,
    Diode
};

class LevelDetector
{
public:
    LevelDetector() { freq.reset (50); }
    ~LevelDetector() {}

    void setFreq (float newFreq);
    void setRectifier (RectifierType type);

    void reset (float sampleRate);
    void processBlock (float* buffer, int numSamples);
    inline float processSample (float x);

private:
    // Rectifier stuff
    inline float diode (float x);
    inline float fullWaveRectifier (float x);
    inline float halfWaveRectifier (float x);
    std::function<float (float)> rectifier = [this] (float x) { return fullWaveRectifier (x); };
    RectifierType currentType = FWR;
    
    // Filter stuff
    void calcCoefs (float fc);
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> freq = 10.0f;
    float rawFreq = freq.getCurrentValue();
    float b[2] = { 1.0f, 0.0f };
    float a[2] = { 1.0f, 0.0f };
    float z = 0.0f;
    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelDetector)
};

#endif // !LEVELDETECTOR_H_INCLUDED

