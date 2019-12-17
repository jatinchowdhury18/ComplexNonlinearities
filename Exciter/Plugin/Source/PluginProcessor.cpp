/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ExciterAudioProcessor::ExciterAudioProcessor()
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
    rectParameter = vts.getRawParameterValue ("rect");
    freqParameter = vts.getRawParameterValue ("freq");
    driveParameter = vts.getRawParameterValue ("drivegain");
    satParameter = vts.getRawParameterValue ("sat");
}

ExciterAudioProcessor::~ExciterAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout ExciterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterInt> ("rect", "Rectifier", RectifierType::FWR, RectifierType::Diode, RectifierType::HWR));
    params.push_back (std::make_unique<AudioParameterFloat> ("freq", "Freq", 1.0f, 30.0f, 10.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("drivegain", "Drive", 1.0f, 12.0f, 8.0f));
    params.push_back (std::make_unique<AudioParameterInt> ("sat", "Saturator", SaturatorType::HardClip, SaturatorType::Tanh, SaturatorType::SoftClip));

    return { params.begin(), params.end() };
}

//==============================================================================
const String ExciterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ExciterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ExciterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ExciterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ExciterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ExciterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ExciterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ExciterAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String ExciterAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void ExciterAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void ExciterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
    {
        exciter[ch].setDrive (*driveParameter / 100.0f);
        exciter[ch].setControlGain (ExciterProcessor::getControlGainFromDrive (*driveParameter / 100.0f));
        exciter[ch].setDetectorFreq (*freqParameter);
        exciter[ch].setRectifierType (static_cast<RectifierType> ((int) *rectParameter));
        exciter[ch].setSaturator (static_cast<SaturatorType> ((int) *satParameter));
        exciter[ch].reset (oversampling.getOversamplingFactor() * (float) sampleRate);
    }
}

void ExciterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExciterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ExciterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    ScopedNoDenormals noDenormals;

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        exciter[ch].setDrive (*driveParameter / 100.0f);
        exciter[ch].setControlGain (ExciterProcessor::getControlGainFromDrive (*driveParameter / 100.0f));
        exciter[ch].setDetectorFreq (*freqParameter);
        exciter[ch].setRectifierType (static_cast<RectifierType> ((int) *rectParameter));
        exciter[ch].setSaturator (static_cast<SaturatorType> ((int) *satParameter));
        exciter[ch].processBlock (osBuffer.getWritePointer (ch), osBuffer.getNumSamples());
    }

    oversampling.processSamplesDown (block);
}

//==============================================================================
bool ExciterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ExciterAudioProcessor::createEditor()
{
    return new ExciterAudioProcessorEditor (*this);
}

//==============================================================================
void ExciterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ExciterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new ExciterAudioProcessor();
}
