#ifndef SUBHARMONICPROCESSOR_H_INCLUDED
#define SUBHARMONICPROCESSOR_H_INCLUDED

#include "JuceHeader.h"
#include "LevelDetector.h"

class SubharmonicProcessor
{
public:
    SubharmonicProcessor() {}

    void reset (float sampleRate)
    {
        rising = true;
        switchCount = 0;
        output = 1.0f;
        lastX = 0.0f;

        detector.reset (sampleRate);
    }

    void setDetector (float attackMs, float releaseMs)
    {
        detector.setAttack (attackMs);
        detector.setRelease (releaseMs);
    }

    void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
            buffer[n] = processSample (buffer[n]);
    }

    inline float processSample (float x)
    {
        float level = detector.processSample (x);

        if (rising == true && x < lastX)
        {
            switchCount++;
            rising = false;
        }
        else if (rising == false && x > lastX)
        {
            switchCount++;
            rising = true;
        }

        if (switchCount == 2)
        {
            output *= -1.0f;
            switchCount = 0;
        }

        lastX = x;
        return output * level;
    }

private:
    bool rising = true;
    int switchCount = 0;
    float output = 1.0f;
    float lastX = 0.0f;

    LevelDetector detector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubharmonicProcessor)
};

#endif //SUBHARMONICPROCESSOR_H_INCLUDED
