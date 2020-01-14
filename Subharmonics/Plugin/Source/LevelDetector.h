#ifndef LEVELDETECTOR_H_INCLUDED
#define LEVELDETECTOR_H_INCLUDED

#include "JuceHeader.h"

/** Simple peak level detector */
class LevelDetector
{
public:
    LevelDetector() {}

    void setAttack (float attMs);
    void setRelease (float relMs);

    void reset (float sampleRate);
    inline float processSample (float x)
    {
        if (fabs (x) > levelEst)
            levelEst += b0_a * (fabs (x) - levelEst);
        else
            levelEst += b0_r * (fabs (x) - levelEst);

        return levelEst;
    }

private:
    float attackMs = 1.0f;
    float a1_a = 0.0f;
    float b0_a = 1.0f;

    float releaseMs = 50.0f;
    float a1_r = 0.0f;
    float b0_r = 1.0f;

    float levelEst = 0.0f;
    float fs = 48000.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelDetector)
};

#endif //LEVELDETECTOR_H_INCLUDED
