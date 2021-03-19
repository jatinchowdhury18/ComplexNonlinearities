/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HysteresisAudioProcessor::HysteresisAudioProcessor()
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
    processor (vts)
{
}

HysteresisAudioProcessor::~HysteresisAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout HysteresisAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    HysteresisProcessor::createParameterLayout (params);
    
    return { params.begin(), params.end() };
}

//==============================================================================
const String HysteresisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HysteresisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HysteresisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HysteresisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HysteresisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HysteresisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HysteresisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HysteresisAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String HysteresisAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void HysteresisAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void HysteresisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processor.prepareToPlay (sampleRate, samplesPerBlock);
}

void HysteresisAudioProcessor::releaseResources()
{
    processor.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HysteresisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void HysteresisAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    processor.processBlock (buffer, midiMessages);
}

//==============================================================================
bool HysteresisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HysteresisAudioProcessor::createEditor()
{
    return new HysteresisAudioProcessorEditor (*this);
}

//==============================================================================
void HysteresisAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void HysteresisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new HysteresisAudioProcessor();
}
