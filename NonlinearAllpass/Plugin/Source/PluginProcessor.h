/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "NonlinearAllpass.h"
#include "Saturators.h"

using Filter = dsp::StateVariableFilter::Filter<float>;
using Parameters = dsp::StateVariableFilter::Parameters<float>;
using DSPFilter = dsp::ProcessorDuplicator<Filter, Parameters>;

//==============================================================================
/**
*/
class NonlienarAllpassAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    NonlienarAllpassAudioProcessor();
    ~NonlienarAllpassAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState& getVTS() {  return vts; }
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    const static int maxOrder = 10;
    AudioProcessorValueTreeState vts;

    float* gainParam;
    float* orderParam;
    float* satParam;
    float* freqParam;

    SatFunc saturator;
    std::unique_ptr<AllpassLadder> allpass[2][10];

    DSPFilter filter;
    DSPFilter dcBlocker;

    dsp::Oversampling<float> oversampling;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonlienarAllpassAudioProcessor)
};
