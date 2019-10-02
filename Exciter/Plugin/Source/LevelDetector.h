#ifndef LEVELDETECTOR_H_INCLUDED
#define LEVELDETECTOR_H_INCLUDED

#include "JuceHeader.h"

namespace Consts
{
    constexpr float alpha = 0.05f / 0.0259f;
    const float beta = 0.2f; // 1.0f / (std::expf (alpha) - 1.0f);
}

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
    
    inline float processSample (float x)
    {
        if (freq.isSmoothing())
            calcCoefs (freq.getNextValue());
        
        x = rectifier (x);
        auto y = z + x * b[0];
        z = x * b[1] - y * a[1];
        return y;
    }

private:
    // Rectifier stuff
    inline float diode (float x)
    {
        return 25.0f * Consts::beta * (std::expf (Consts::alpha * x) - 1.0f);
    }
    
    inline float fullWaveRectifier (float x)
    {
        return abs (x);
    }
    
    inline float halfWaveRectifier (float x)
    {
        return 2.0f * (x > 0.0f ? x : 0.0f);
    }
    
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

