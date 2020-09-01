#ifndef HYSTERESISPROCESSOR_H_INCLUDED
#define HYSTERESISPROCESSOR_H_INCLUDED

#include "HysteresisProcessing.h"
#include "DCFilters.h"

/* Hysteresis Processor. */
class HysteresisProcessor
{
public:
    HysteresisProcessor (AudioProcessorValueTreeState& vts);

    /* Reset fade buffers, filters, and processors. Prepare oversampling */
    void prepareToPlay (double sampleRate, int samplesPerBlock);

    /* Reset oversampling */
    void releaseResources();

    /* Proceess a buffer. */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiBuffer);

    static void createParameterLayout (std::vector<std::unique_ptr<RangedAudioParameter>>& params);

    float getLatencySamples() const noexcept;

private:
    void setSolver (int newSolver);
    void setDrive (float newDrive);
    void setSaturation (float newSat);
    void setWidth (float newWidth);
    void setOversampling();
    float calcMakeup();

    void process (dsp::AudioBlock<float>& block);
    void processSmooth (dsp::AudioBlock<float>& block);
    void applyDCBlockers (AudioBuffer<float>& buffer);

    float* driveParam = nullptr;
    float* satParam = nullptr;
    float* widthParam = nullptr;
    float* osParam = nullptr;
    float* modeParam = nullptr;

    SmoothedValue<float, ValueSmoothingTypes::Linear> drive[2];
    SmoothedValue<float, ValueSmoothingTypes::Linear> width[2];
    SmoothedValue<float, ValueSmoothingTypes::Linear> sat[2];
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> makeup[2];

    float fs = 44100.0f;
    int curOS = 0, prevOS = 0;
    HysteresisProcessing hProcs[2];
    std::unique_ptr<dsp::Oversampling<float>> overSample[5]; // needs oversampling to avoid aliasing
    TransformerHPF dcBlocker[2];
    // TransformerShelf dcLower[2];

    int overSamplingFactor = 2;
    const float dcFreq = 35.0f;
    const float dcShelfFreq = 60.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HysteresisProcessor)
};

#endif //HYSTERESISPROCESSOR_H_INCLUDED
