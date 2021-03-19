/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CopyEQ.h"
#include "Saturators.h"

namespace Tags
{
    const String nablaID  = "nabla";
    const String continID = "continuous";
    const String rhoID    = "rho";
    const String flipID   = "flip";
    const String lpfID    = "lpf";
    const String bypassID = "bypass";
    const String dwID     = "drywet";
    const String stID     = "stereo";
}

class CopyEqAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    CopyEqAudioProcessor();
    ~CopyEqAudioProcessor();

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

    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    AudioProcessorValueTreeState& getVTS() { return vts; }
    
    void triggerLearn() { learn = true; }
    bool getLearnState() const noexcept { return learn; }
    void setLearnLength (float newLength) { lengthLearnSeconds = newLength; }
    float getLearnLength() const noexcept { return lengthLearnSeconds; }
    std::unique_ptr<FIRFilter>& getFilter (int ch) { return filter[ch]; }

private:
    AudioProcessorValueTreeState vts;

    std::atomic<float>* contParam;
    std::atomic<float>* nablaParam;
    std::atomic<float>* rhoParam;
    std::atomic<float>* flipParam;
    std::atomic<float>* bypassParam;
    std::atomic<float>* dwParam;
    std::atomic<float>* lpfParam;
    std::atomic<float>* stParam;

    bool learn = false;
    float lengthLearnSeconds = 1.0f;
    int samplesLearned = 0;

    std::unique_ptr<FIRFilter> filter[2];
    std::unique_ptr<CopyEQ> eqs[2];

    AudioBuffer<float> dryBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CopyEqAudioProcessor)
};
