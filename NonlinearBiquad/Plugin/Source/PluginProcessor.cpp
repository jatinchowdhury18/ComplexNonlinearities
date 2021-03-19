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
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout()),
    oversampling (2, 3, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
    eqShapeParameter    = vts.getRawParameterValue ("shape");
    eqFreqParameter     = vts.getRawParameterValue ("freq");
    eqQParameter        = vts.getRawParameterValue ("q");
    eqGainParameter     = vts.getRawParameterValue ("gain");
    driveParameter      = vts.getRawParameterValue ("drivegain");
    satParameter        = vts.getRawParameterValue ("sat");
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

    params.push_back (std::make_unique<AudioParameterInt> ("shape", "Shape", EqShape::bell, EqShape::lowPass, EqShape::lowPass));
    params.push_back (std::make_unique<AudioParameterFloat> ("freq", "Freq", freqRange, 1000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("q", "Q", qRange, 0.707f));
    params.push_back (std::make_unique<AudioParameterFloat> ("gain", "Gain", -15.0f, 15.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("drivegain", "Drive", -30.0f, 30.0f, -12.0f));
    params.push_back (std::make_unique<AudioParameterInt> ("sat", "Saturator", SatType::none, SatType::hyptan, SatType::none));

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

void NonlinearBiquadAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String NonlinearBiquadAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void NonlinearBiquadAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void NonlinearBiquadAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
    {
        filter[ch].reset (sampleRate * oversampling.getOversamplingFactor());
        filter[ch].toggleOnOff (true);

        inGain[ch].prepare();
    }
}

void NonlinearBiquadAudioProcessor::releaseResources()
{
    oversampling.reset();
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

void NonlinearBiquadAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    ScopedNoDenormals noDenormals;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        inGain[ch].setGain (Decibels::decibelsToGain (driveParameter->load()));
        inGain[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        filter[ch].setEqShape (static_cast<EqShape> ((int) eqShapeParameter->load()));
        filter[ch].setFrequency (eqFreqParameter->load());
        filter[ch].setQ (eqQParameter->load());
        filter[ch].setGain (Decibels::decibelsToGain (eqGainParameter->load()));
        filter[ch].setSaturator (static_cast<SatType> ((int) satParameter->load()));

        filter[ch].processBlock (osBuffer.getWritePointer (ch), osBuffer.getNumSamples());
    }

    oversampling.processSamplesDown (block);
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
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void NonlinearBiquadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (vts.state.getType()))
            vts.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NonlinearBiquadAudioProcessor();
}
