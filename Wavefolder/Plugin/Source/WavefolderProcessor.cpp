#include "WavefolderProcessor.h"

enum
{
    numSteps = 200,
};

WavefolderProcessor::WavefolderProcessor()
{
    freq.reset (numSteps);
    depth.reset (numSteps);
    feedforward.reset (numSteps);
    feedback.reset (numSteps);
}

void WavefolderProcessor::reset (float sampleRate)
{
    fs = sampleRate;
    y1 = 0.0f;

    freq.setCurrentAndTargetValue (freq.getTargetValue());
    depth.setCurrentAndTargetValue (depth.getTargetValue());
    feedforward.setCurrentAndTargetValue (feedforward.getTargetValue());
    feedback.setCurrentAndTargetValue (feedback.getTargetValue());
}

void WavefolderProcessor::processBlock(float* buffer, int numSamples)
{
    for (int n = 0; n < numSamples; ++n)
    {
        auto curFF = feedforward.getNextValue();
        auto curFB = feedback.getNextValue();

        float ff = curFF * ffSat (buffer[n]) + (1.0f - curFF) * buffer[n];
        float fb = curFB * ffSat (y1);
        y1 = (ff + fb) - depth.getNextValue() * wave (buffer[n], jmax (freq.getNextValue(), 0.00001f)*fs/2, fs);
        buffer[n] = y1 / (1.0f + curFB);
    }
}
