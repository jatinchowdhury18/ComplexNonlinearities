#include "LevelDetector.h"

namespace Consts
{
    constexpr float alpha = 0.05f / 0.0259f;
    const float beta = 0.2f; // 1.0f / (std::expf (alpha) - 1.0f);
}

void LevelDetector::setFreq (float newFreq)
{
    rawFreq = newFreq;
    
    if (currentType == HWR)
        freq.setTargetValue (rawFreq / 2.0f);
    else if (currentType == FWR)
        freq.setTargetValue (rawFreq * 2.0f);
    else if (currentType == Diode)
        freq.setTargetValue (rawFreq / 10.0f);
}

void LevelDetector::setRectifier (RectifierType type)
{
    currentType = type;

    if (type == FWR)
        rectifier = [this] (float x) { return fullWaveRectifier (x); };
    else if (type == HWR)
        rectifier = [this] (float x) { return halfWaveRectifier (x); };
    else if (type == Diode)
        rectifier = [this] (float x) { return diode (x); };

    setFreq (rawFreq);
}

void LevelDetector::reset (float sampleRate)
{
    fs = sampleRate;
    z = 0.0f;

    freq.setCurrentAndTargetValue (freq.getTargetValue());
    calcCoefs (freq.getCurrentValue());
}

void LevelDetector::processBlock (float* buffer, int numSamples)
{
    for (int n = 0; n < numSamples; ++n)
        buffer[n] = processSample (buffer[n]);
}

float LevelDetector::processSample (float x)
{
    if (freq.isSmoothing())
        calcCoefs (freq.getNextValue());

    x = rectifier (x);
    auto y = z + x * b[0];
    z = x * b[1] - y * a[1];
    return y;
}

void LevelDetector::calcCoefs (float fc)
{
    auto wc = MathConstants<float>::twoPi * fc / fs;
    auto c = 1.0f / std::tanf (wc / 2.0f);
    a[0] = c + 1.0f;

    b[0] = 1.0f / a[0];
    b[1] = b[0];
    a[1] = (1.0f - c) / a[0];
}

float LevelDetector::fullWaveRectifier (float x)
{
    return abs (x);
}

float LevelDetector::halfWaveRectifier (float x)
{
    return 2.0f * (x > 0.0f ? x : 0.0f);
}

float LevelDetector::diode (float x)
{
    return 25.0f * Consts::beta * (std::expf (Consts::alpha * x) - 1.0f);
}
