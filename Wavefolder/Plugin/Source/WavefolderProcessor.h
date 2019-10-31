#ifndef WAVEFOLDERPROCESSOR_H_INCLUDED
#define WAVEFOLDERPROCESSOR_H_INCLUDED

#include "Saturators.h"
#include "Waves.h"

class WavefolderProcessor
{
public:
    WavefolderProcessor();

    void reset (float sampleRate);
    void processBlock (float* buffer, int numSamples);
    
    void setFreq (float newFreq) { freq.setTargetValue (newFreq); }
    void setDepth (float newDepth) { depth.setTargetValue (newDepth); }
    void setFF (float newFF) { feedforward.setTargetValue (newFF); }
    void setFB (float newFB) { feedback.setTargetValue (newFB); }
    void setSatType (SatType type) { ffSat = Saturators::getSaturator (type); }
    void setWaveType (WaveType type) { wave = Waves::getWave (type); }

private:
    float fs = 44100.0f;
    SatFunc ffSat = Saturators::getSaturator (SatType::none);
    WaveFunc wave = Waves::getWave (WaveType::zero);

    SmoothedValue<float, ValueSmoothingTypes::Linear> freq = 0.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> depth = 0.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> feedforward = 1.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> feedback = 0.0f;

    float y1 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavefolderProcessor)
};

#endif //WAVEFOLDERPROCESSOR_H_INCLUDED
