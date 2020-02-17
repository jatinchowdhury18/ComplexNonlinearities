/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GatedRecurrentDistortionAudioProcessor::GatedRecurrentDistortionAudioProcessor()
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
    Wf = vts.getRawParameterValue ("wf_");
    Wh = vts.getRawParameterValue ("wh_");
    Uf = vts.getRawParameterValue ("uf_");
    Uh = vts.getRawParameterValue ("uh_");
    bf = vts.getRawParameterValue ("bf_");
}

GatedRecurrentDistortionAudioProcessor::~GatedRecurrentDistortionAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout GatedRecurrentDistortionAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterFloat> ("wf_", "Wf", 0.0f, 10.0f,0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> ("wh_", "Wh", 0.01f,5.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("uf_", "Uf", 0.0f, 5.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> ("uh_", "Uh", 0.0f, 2.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> ("bf_", "bf",-1.0f, 5.0f, 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const String GatedRecurrentDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GatedRecurrentDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GatedRecurrentDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GatedRecurrentDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GatedRecurrentDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GatedRecurrentDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GatedRecurrentDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GatedRecurrentDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const String GatedRecurrentDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void GatedRecurrentDistortionAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GatedRecurrentDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    updateParams();
    for (int ch = 0; ch < 2; ++ch)
        gru[ch].reset();
}

void GatedRecurrentDistortionAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GatedRecurrentDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GatedRecurrentDistortionAudioProcessor::updateParams()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        gru[ch].setParams (*Wf, *Wh, *Uf, *Uh, *bf);
    }
}

void GatedRecurrentDistortionAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    updateParams();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        gru[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool GatedRecurrentDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GatedRecurrentDistortionAudioProcessor::createEditor()
{
    return new GatedRecurrentDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void GatedRecurrentDistortionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GatedRecurrentDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new GatedRecurrentDistortionAudioProcessor();
}
