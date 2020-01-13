/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SubharmonicProcessor.h"
#include "EQFilter.h"
#include "Gain.h"

//==============================================================================
/**
*/
class SubharmonicsAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SubharmonicsAudioProcessor();
    ~SubharmonicsAudioProcessor();

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

    AudioProcessorValueTreeState& getVTS() { return vts; }
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateParams();

private:
    AudioProcessorValueTreeState vts;

    AudioBuffer<float> sidechainBuffer;

    SubharmonicProcessor sub[2];
    EQFilter preEQ[2];
    EQFilter postEQ[3][2];
    Gain mainGain[2];
    Gain sideGain[2];

    float* preCutoffParam;
    float* postCutoffParam;
    float* mainGainParam;
    float* sideGainParam;

    const float butterQs[3] = { 0.51763809f, 0.70710678f, 1.93185165f };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubharmonicsAudioProcessor)
};
