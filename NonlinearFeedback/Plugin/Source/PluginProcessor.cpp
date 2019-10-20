/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NonlinearFeedbackAudioProcessor::NonlinearFeedbackAudioProcessor()
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
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout()),
    oversampling (2, 3, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
    shapeParameter = vts.getRawParameterValue ("shape");
    freqParameter  = vts.getRawParameterValue ("freq");
    qParameter     = vts.getRawParameterValue ("q");
    driveParameter = vts.getRawParameterValue ("drivegain");
    satParameter   = vts.getRawParameterValue ("sat");
}

NonlinearFeedbackAudioProcessor::~NonlinearFeedbackAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout NonlinearFeedbackAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> freqRange (20.0f, 20000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> qRange (0.1f, 18.0f);
    qRange.setSkewForCentre (0.707f);

    params.push_back (std::make_unique<AudioParameterInt> ("shape", "Shape", EqShape::bell, EqShape::lowPass, EqShape::lowPass));
    params.push_back (std::make_unique<AudioParameterFloat> ("freq", "Freq", freqRange, 1000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("q", "Q", qRange, 0.707f));
    params.push_back (std::make_unique<AudioParameterFloat> ("drivegain", "Drive", -60.0f, 0.0f, -30.0f));
    params.push_back (std::make_unique<AudioParameterInt> ("sat", "Saturator", SatType::none, SatType::ahypsin, SatType::none));

    return { params.begin(), params.end() };
}

//==============================================================================
const String NonlinearFeedbackAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NonlinearFeedbackAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NonlinearFeedbackAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NonlinearFeedbackAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NonlinearFeedbackAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NonlinearFeedbackAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NonlinearFeedbackAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NonlinearFeedbackAudioProcessor::setCurrentProgram (int index)
{
}

const String NonlinearFeedbackAudioProcessor::getProgramName (int index)
{
    return {};
}

void NonlinearFeedbackAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NonlinearFeedbackAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
    {
        filter[ch].reset ((float) sampleRate * oversampling.getOversamplingFactor());
        filter[ch].toggleOnOff (true);

        inGain[ch].prepare();
        outGain[ch].prepare();
    }
}

void NonlinearFeedbackAudioProcessor::releaseResources()
{
    oversampling.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NonlinearFeedbackAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NonlinearFeedbackAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        inGain[ch].setGain (Decibels::decibelsToGain (*driveParameter));
        inGain[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        filter[ch].setEqShape (EqShape::lowPass);
        filter[ch].setFrequency (*freqParameter);
        filter[ch].setQ (*qParameter);
        filter[ch].setSaturator (static_cast<SatType> ((int) *satParameter));
        filter[ch].processBlock (osBuffer.getWritePointer (ch), osBuffer.getNumSamples());
    }

    oversampling.processSamplesDown (block);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        outGain[ch].setGain (Decibels::decibelsToGain (-1.0f * *driveParameter));
        outGain[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool NonlinearFeedbackAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NonlinearFeedbackAudioProcessor::createEditor()
{
    return new NonlinearFeedbackAudioProcessorEditor (*this);
}

//==============================================================================
void NonlinearFeedbackAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NonlinearFeedbackAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NonlinearFeedbackAudioProcessor();
}
