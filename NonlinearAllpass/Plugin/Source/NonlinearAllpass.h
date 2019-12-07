#ifndef NONLINEARALLPASS_H_INCLUDED
#define NONLINEARALLPASS_H_INCLUDED

#include "JuceHeader.h"

class DelayElement
{
public:
    DelayElement() {}
    virtual ~DelayElement() {}

    virtual void reset() { z = 0.0f; }

    virtual inline void write (float x) { z = x; }

    virtual inline float read() const noexcept { return z; }

    virtual inline void setCoefs (float /*theta*/) {}

    virtual inline float process (float x)
    {
        float y = z;
        z = x;
        return y;
    }

protected:
    float z = 0.0f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayElement)
};

class AllpassLadder : public DelayElement
{
public:
    AllpassLadder (int order = 1)
    {
        if (order <= 1)
            delay.reset (new DelayElement);
        else
            delay.reset (new AllpassLadder (order - 1));
    }

    void write (float x) override { z = delay->process (x); }

    float read() const noexcept override { return z; }

    inline void setCoefs (float theta) override
    {
        s =  dsp::FastMathApproximations::sin (theta);
        c =  dsp::FastMathApproximations::cos (theta);
        delay->setCoefs (theta);
    }

    virtual inline float process (float x) override
    {
        float y = s * x + c * delay->read();
        delay->write (c * x - s * delay->read());
        return y;
    }

private:
    float s = 1.0f;
    float c = 0.0f;

    std::unique_ptr<DelayElement> delay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AllpassLadder)
};

#endif //NONLINEARALLPASS_H_INCLUDED
