#include "CopyEQ.h"

CopyEQ::CopyEQ (std::unique_ptr<FIRFilter>& thisF, std::unique_ptr<FIRFilter>& otherF) :
    filter (thisF),
    otherFilter (otherF)
{
    filter.reset (nullptr);
    sideFilter.parameters->type = Parameters::Type::lowPass;
}

void CopyEQ::setRho (float newRho)
{
    inWarp.setRho (newRho);
    outWarp.setRho (-newRho);
    sideWarp.setRho (newRho);
}

void CopyEQ::setSideCutoff (float freq)
{
    sideFilter.parameters->setCutOffFrequency ((double) fs, freq);
}

void CopyEQ::reset (double sampleRate, int samplesPerBlock)
{
    fs = (float) sampleRate;
    fsFactor = (int) (fs / 44100.0f);
    filter.reset (new FIRFilter (order * fsFactor));
    filter->reset();

    for (int n = 0; n < filter->order; ++n)
        filter->h[n] = 0.0f;
    filter->h[0] = 1.0f;

    inWarp.reset();
    outWarp.reset();
    sideWarp.reset();

    dsp::ProcessSpec spec { sampleRate, (uint32) samplesPerBlock, 1 };
    sideFilter.reset();
    sideFilter.prepare (spec);

    random.setSeedRandomly();
}

void CopyEQ::processBlock (float* buffer, const int numSamples)
{
    for (int n = 0; n < numSamples; ++n)
    {
        // add new sample to filter buffer
        filter->z[filter->zPtr] = inWarp.process (buffer[n] / 10.0f);

        // apply filter
        float y = 0.0f;
        for (int m = 0; m < filter->order; ++m)
            y += (filter->h[m] * stFactor + otherFilter->h[m] * (1.0f - stFactor))
               * filter->z[negativeAwareModulo<int> (filter->zPtr - m, filter->order)];

        // update filter buffer
        filter->zPtr = (filter->zPtr + 1) % filter->order;

        // output
        buffer[n] = outWarp.process (y * 10.0f);
    }
}

void CopyEQ::processBlockLearn (float* mainBuffer, float* sideBuffer, const int numSamples)
{
    for (int n = 0; n < numSamples; ++n)
    {
        // add new sample to filter buffer
        filter->z[filter->zPtr] = inWarp.process (mainBuffer[n] / 10.0f);

        // apply filter
        float y = 0.0f;
        for (int m = 0; m < filter->order; ++m)
            y += filter->h[m] * filter->z[negativeAwareModulo<int> (filter->zPtr - m, filter->order)];

        // calculate error
        float sideSamp = sideWarp.process (sideBuffer[n]);
        float desSamp = flip ? 2.0f * (random.nextFloat() - 0.5f) - sideSamp : sideSamp;
        float err = sideFilter.processSample (desSamp) - y;

        // Update coefficients
        for (int m = 0; m < filter->order; ++m)
            filter->h[m] += nabla * err * filter->z[negativeAwareModulo<int> (filter->zPtr - m, filter->order)];

        // update filter buffer
        filter->zPtr = (filter->zPtr + 1) % filter->order;

        // output
        mainBuffer[n] = outWarp.process (y * 10.0f);
    }
}
