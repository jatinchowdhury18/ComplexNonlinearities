#ifndef SATURATORS_H_INCLUDED
#define SATURATORS_H_INCLUDED

#include"JuceHeader.h"

enum SatType
{
    none,
    hard,
    soft,
    hyptan,
    ahypsin,
};

using SatFunc = std::function<float (float)>;

class Saturators
{
public:
    static SatFunc getSaturator (SatType type)
    {
        if (type == SatType::hard)
            return [] (float x) { return hardClip (x); };

        if (type == SatType::soft)
            return [] (float x) { return softClip (x); };

        if (type == SatType::hyptan)
            return [] (float x) { return tanhClip (x); };

        if (type == SatType::ahypsin)
            return [] (float x) { return aSinhClip (x); };

        // None
        return [] (float x) { return x; };
    }

    static inline float hardClip (float x)
    {
        if (x > 1.0f)
            return 1.0f;

        if (x < -1.0f)
            return -1.0f;

        return x;
    }

    static inline float softClip (float x)
    {
        if (x > 1.0f)
            return 2.0f / 3.0f;

        if (x < -1.0f)
            return -2.0f / 3.0f;

        return x - x*x*x/3.0f;
    }

    static inline float tanhClip (float x)
    {
        return tanhf (x);
    }

    static inline float aSinhClip (float x)
    {
        return asinhf (x);
    }
};

#endif //SATURATORS_H_INCLUDED
