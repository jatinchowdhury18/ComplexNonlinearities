#include "CopyEQ.h"

CopyEQ::CopyEQ()
{
    filter.reset (nullptr);
}

void CopyEQ::setRho (float newRho)
{
    inWarp.setRho (newRho);
    outWarp.setRho (-newRho);
}

void CopyEQ::reset (double sampleRate)
{
    fs = (float) sampleRate;
    fsFactor = (int) (fs / 44100.0f);
    filter.reset (new FIRFilter (order * fsFactor));
    filter->reset();

    for (int n = 0; n < filter->order; ++n)
        filter->h[n] = 0.0f;

    inWarp.reset();
    outWarp.reset();

    random.setSeedRandomly();
}

void CopyEQ::processBlock (float* mainBuffer, float* sideBuffer, const int numSamples)
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
        float desSamp = flip ? 2.0f * (random.nextFloat() - 0.5f) - sideBuffer[n]
                             : sideBuffer[n];
        float err = desSamp - y;

        // Update coefficients
        for (int m = 0; m < filter->order; ++m)
            filter->h[m] += nabla * err * filter->z[negativeAwareModulo<int> (filter->zPtr - m, filter->order)];
        
        // update filter buffer
        filter->zPtr = (filter->zPtr + 1) % filter->order;

        // if (nabla > 0.0f)
        mainBuffer[n] = outWarp.process (y * 10.0f);
    }
}
