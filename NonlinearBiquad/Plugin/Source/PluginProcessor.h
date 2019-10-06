/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gain.h"
#include "EQFilter.h"

//==============================================================================
/**
*/
class NonlinearBiquadAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    NonlinearBiquadAudioProcessor();
    ~NonlinearBiquadAudioProcessor();

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
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    AudioProcessorValueTreeState vts;

    float*  eqShapeParameter;
    float*  eqFreqParameter;
    float*  eqQParameter;
    float*  eqGainParameter;
    float*  driveParameter;
    float*  satParameter;

    Gain gain[2];
    EQFilter filter[2];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonlinearBiquadAudioProcessor)
};
