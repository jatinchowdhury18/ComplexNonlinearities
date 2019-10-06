/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NonlinearBiquadAudioProcessor::NonlinearBiquadAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout())
{
    eqShapeParameter    = vts.getRawParameterValue ("shape");
    eqFreqParameter     = vts.getRawParameterValue ("freq");
    eqQParameter        = vts.getRawParameterValue ("q");
    eqGainParameter     = vts.getRawParameterValue ("gain");
    driveParameter      = vts.getRawParameterValue ("drivegain");
    satParameter         = vts.getRawParameterValue ("sat");
}

NonlinearBiquadAudioProcessor::~NonlinearBiquadAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout NonlinearBiquadAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> freqRange (20.0f, 20000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> qRange (0.1f, 18.0f);
    qRange.setSkewForCentre (0.707f);

    params.push_back (std::make_unique<AudioParameterInt> ("shape", "Shape", EqShape::bell, EqShape::lowPass, EqShape::bell));
    params.push_back (std::make_unique<AudioParameterFloat> ("freq", "Freq", freqRange, 1000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("q", "Q", qRange, 0.707f));
    params.push_back (std::make_unique<AudioParameterFloat> ("gain", "Gain", -15.0f, 15.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("drivegain", "Drive", -30.0f, 6.0f, -12.0f));
    params.push_back (std::make_unique<AudioParameterInt> ("sat", "Saturator", SatType::none, SatType::hyptan, SatType::soft));

    return { params.begin(), params.end() };
}

//==============================================================================
const String NonlinearBiquadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NonlinearBiquadAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NonlinearBiquadAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NonlinearBiquadAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NonlinearBiquadAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NonlinearBiquadAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NonlinearBiquadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NonlinearBiquadAudioProcessor::setCurrentProgram (int index)
{
}

const String NonlinearBiquadAudioProcessor::getProgramName (int index)
{
    return {};
}

void NonlinearBiquadAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NonlinearBiquadAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        filter[ch].reset (sampleRate);
        filter[ch].toggleOnOff (true);
    }
}

void NonlinearBiquadAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NonlinearBiquadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NonlinearBiquadAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        gain[ch].setGain (Decibels::decibelsToGain (*driveParameter));

        filter[ch].setEqShape (static_cast<EqShape> ((int) *eqShapeParameter));
        filter[ch].setFrequency (*eqFreqParameter);
        filter[ch].setQ (*eqQParameter);
        filter[ch].setGain (Decibels::decibelsToGain (*eqGainParameter));
        filter[ch].setSaturator (static_cast<SatType> ((int) *satParameter));

        gain[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
        filter[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool NonlinearBiquadAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NonlinearBiquadAudioProcessor::createEditor()
{
    return new NonlinearBiquadAudioProcessorEditor (*this);
}

//==============================================================================
void NonlinearBiquadAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NonlinearBiquadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NonlinearBiquadAudioProcessor();
}
