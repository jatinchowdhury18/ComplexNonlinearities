#include "LevelDetector.h"

void LevelDetector::setAttack (float attMs)
{
    if (attMs != attackMs)
    {
        attackMs = attMs;
        a1_a = expf (-1.0f / (fs * attackMs / 1000.0f));
        b0_a = 1.0f - a1_a;
    }
}

void LevelDetector::setRelease (float relMs)
{
    if (relMs != releaseMs)
    {
        releaseMs = relMs;
        a1_r = expf (-1.0f / (fs * releaseMs / 1000.0f));
        b0_r = 1.0f - a1_r;
    }
}

void LevelDetector::reset (float sampleRate)
{
    fs = sampleRate;
    levelEst = 0.0f;

    setAttack (attackMs);
    setRelease (releaseMs);
}
