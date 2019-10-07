/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DoubleSoftClipperAudioProcessor::DoubleSoftClipperAudioProcessor()
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
    upperLim = vts.getRawParameterValue ("upperlim");
    lowerLim = vts.getRawParameterValue ("lowerlim");
    slope = vts.getRawParameterValue ("slope");
    width = vts.getRawParameterValue ("width");
    upperSkew = vts.getRawParameterValue ("upperskew");
    lowerSkew = vts.getRawParameterValue ("lowerskew");
}

DoubleSoftClipperAudioProcessor::~DoubleSoftClipperAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DoubleSoftClipperAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterFloat> ("upperlim", "Upper Lim", 0.0f, 1.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("lowerlim", "Lower Lim", 0.0f, 1.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("slope", "Slope", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("width", "Width", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("upperskew", "Upper Skew", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("lowerskew", "Lower Skew", 0.0f, 1.0f, 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const String DoubleSoftClipperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DoubleSoftClipperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DoubleSoftClipperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DoubleSoftClipperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DoubleSoftClipperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DoubleSoftClipperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DoubleSoftClipperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DoubleSoftClipperAudioProcessor::setCurrentProgram (int index)
{
}

const String DoubleSoftClipperAudioProcessor::getProgramName (int index)
{
    return {};
}

void DoubleSoftClipperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DoubleSoftClipperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);
}

void DoubleSoftClipperAudioProcessor::releaseResources()
{
    oversampling.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DoubleSoftClipperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DoubleSoftClipperAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    // update params
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        dsc[ch].setUpperLim (*upperLim);
        dsc[ch].setLowerLim (*lowerLim);
        dsc[ch].setSlope (*slope);
        dsc[ch].setWidth (*width);
        dsc[ch].setUpperSkew (*upperSkew);
        dsc[ch].setLowerSkew (*lowerSkew);
    }

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock = oversampling.processSamplesUp (block);
    
    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        dsc[ch].processBlock (osBuffer.getWritePointer (ch), osBuffer.getNumSamples());
    }

    oversampling.processSamplesDown (block);
}

//==============================================================================
bool DoubleSoftClipperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DoubleSoftClipperAudioProcessor::createEditor()
{
    return new DoubleSoftClipperAudioProcessorEditor (*this);
}

//==============================================================================
void DoubleSoftClipperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DoubleSoftClipperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DoubleSoftClipperAudioProcessor();
}
