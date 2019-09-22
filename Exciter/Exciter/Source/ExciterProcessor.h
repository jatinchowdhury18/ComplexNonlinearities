#ifndef EXCITERPROCESSOR_H_INCLUDED
#define EXCITERPROCESSOR_H_INCLUDED

#include "LevelDetector.h"

enum SaturatorType
{
    HardClip,
    SoftClip,
    Tanh,
};

class ExciterProcessor
{
public:
    ExciterProcessor() {}
    ~ExciterProcessor() {}

    void reset (float sampleRate);
    void processBlock (float* buffer, int numSamples);

    void setRectifierType (RectifierType newType) { type = newType; detector.setRectifier (newType); }
    void setDetectorFreq (float freq) { detector.setFreq (freq); }
    void setDrive (float newDrive) { drive = newDrive; }
    void setControlGain (float newGain) { controlGain = newGain; }
    void setSaturator (SaturatorType type);

    static float getControlGainFromDrive (float driveGain) noexcept { return 30.0f / (0.1f / driveGain); }

private:
    std::function<float (float)> saturate = [this] (float x) { return hardClip (x); };
    inline float hardClip (float x)
    {
        if (x > 1.0f)
            return 1.0f;

        if (x < -1.0f)
            return -1.0f;

        return x;
    }

    inline float softClip (float x)
    {
        if (x > 1.0f)
            return 1.0f;

        if (x < -1.0f)
            return -1.0f;

        return 1.5f * (x - x*x*x/3.0f);
    }

    float controlGain = 1.0f;
    float oldControlGain = 1.0f;

    float drive = 1.0f;
    float oldDrive = 1.0f;

    LevelDetector detector;
    RectifierType type = FWR;

    float level = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExciterProcessor)
};

#endif //EXCITERPROCESSOR_H_INCLUDED
