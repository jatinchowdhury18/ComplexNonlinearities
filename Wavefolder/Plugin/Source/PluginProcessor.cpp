/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavefolderAudioProcessor::WavefolderAudioProcessor()
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
    freqParam = vts.getRawParameterValue ("freq");
    depthParam = vts.getRawParameterValue ("depth");
    ffParam = vts.getRawParameterValue ("feedforward");
    fbParam = vts.getRawParameterValue ("feedback");
    satParam = vts.getRawParameterValue ("sat");
    waveParam = vts.getRawParameterValue ("wave");
}

WavefolderAudioProcessor::~WavefolderAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout WavefolderAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back (std::make_unique<AudioParameterFloat> ("freq",  "Freq",  0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> ("depth", "Depth", 0.0f, 0.5f, 0.1f));
    params.push_back (std::make_unique<AudioParameterFloat> ("feedback", "Feedback", 0.0f, 0.9f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("feedforward", "Feedforward", 0.0f, 1.0f, 1.0f));

    params.push_back (std::make_unique<AudioParameterInt> ("sat", "Saturator", SatType::none, SatType::ahypsin, SatType::none));
    params.push_back (std::make_unique<AudioParameterInt> ("wave", "Wave", WaveType::zero, WaveType::sine, WaveType::zero));

    return { params.begin(), params.end() };
}

//==============================================================================
const String WavefolderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavefolderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavefolderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavefolderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavefolderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavefolderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavefolderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavefolderAudioProcessor::setCurrentProgram (int index)
{
}

const String WavefolderAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavefolderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WavefolderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    wfProc[0].reset ((float) sampleRate * (float) oversampling.getOversamplingFactor());
    wfProc[1].reset ((float) sampleRate * (float) oversampling.getOversamplingFactor());
}

void WavefolderAudioProcessor::releaseResources()
{
    oversampling.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavefolderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WavefolderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        wfProc[ch].setFreq (*freqParam);
        wfProc[ch].setDepth (*depthParam);
        wfProc[ch].setFF (*ffParam);
        wfProc[ch].setFB (*fbParam);
        wfProc[ch].setSatType (static_cast<SatType> ((int) *satParam));
        wfProc[ch].setWaveType (static_cast<WaveType> ((int) *waveParam));

        wfProc[ch].processBlock (osBuffer.getWritePointer (ch), osBuffer.getNumSamples());
    }

    oversampling.processSamplesDown (block);
}

//==============================================================================
bool WavefolderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WavefolderAudioProcessor::createEditor()
{
    return new WavefolderAudioProcessorEditor (*this);
}

//==============================================================================
void WavefolderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void WavefolderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new WavefolderAudioProcessor();
}
