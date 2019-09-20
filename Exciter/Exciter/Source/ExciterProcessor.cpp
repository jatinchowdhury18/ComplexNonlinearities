#include "ExciterProcessor.h"

namespace
{
    const float attenFWR = Decibels::decibelsToGain (-3.0f);
    const float attenDiode = Decibels::decibelsToGain (11.5f);
}

void ExciterProcessor::reset (float sampleRate)
{
    detector.reset (sampleRate);

    oldDrive = drive;
    oldControlGain = controlGain;
}

void ExciterProcessor::processBlock (float* buffer, int numSamples)
{
    float atten = 1.0f;
    if (type == FWR)
        atten = attenFWR;
    else if (type == Diode)
        atten = attenDiode;

    for (int n = 0; n < numSamples; ++n)
    {
        buffer[n] *= (drive * (float) n / (float) numSamples) + (oldDrive * (1.0f - (float) n / (float) numSamples));

        level = ((controlGain * (float) n / (float) numSamples) + (oldControlGain * (1.0f - (float) n / (float) numSamples))) * detector.processSample (buffer[n]);
        buffer[n] = atten * level * std::tanh (buffer[n] / 0.0259f / 2.0f);
    }

    oldDrive = drive;
    oldControlGain = controlGain;
}
