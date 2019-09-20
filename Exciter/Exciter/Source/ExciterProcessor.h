#ifndef EXCITERPROCESSOR_H_INCLUDED
#define EXCITERPROCESSOR_H_INCLUDED

#include "LevelDetector.h"

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

    static float getControlGainFromDrive (float driveGain) noexcept { return 30.0f / (0.1f / driveGain); }

private:
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
