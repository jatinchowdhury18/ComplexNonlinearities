#ifndef WAVES_H_INCLUDED
#define WAVES_H_INCLUDED

#include "JuceHeader.h"

enum WaveType
{
    zero,
    tri,
    sine,
};

using WaveFunc = std::function<float (float /*x*/, float /*freq*/, float /*fs*/)>;

class Waves
{
public:
    static WaveFunc getWave (WaveType type)
    {
        if (type == WaveType::zero)
            return [] (float x, float, float) { return 0.0f * x; };

        if (type == WaveType::tri)
            return [] (float x, float freq, float fs) { return triWave (x, freq, fs); };

        if (type == WaveType::sine)
            return [] (float x, float freq, float fs) { return sineWave (x, freq, fs); };

        return [] (float x, float, float) { return 0.0f * x; };
    }

    static inline float triWave (float x, float freq, float fs)
    {
        auto p = float ((1 / freq) * fs);
        x = x + p / 4.0f;
        return 4.0f * abs ((x / p) - floorf ((x / p) + 0.5f)) - 1.0f;
    }

    static inline float sineWave (float x, float freq, float fs)
    {
        return sinf (MathConstants<float>::twoPi * x * freq / fs);
    }
};

#endif //WAVES_H_INCLUDED
