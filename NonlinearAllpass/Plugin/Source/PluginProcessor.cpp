/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NonlienarAllpassAudioProcessor::NonlienarAllpassAudioProcessor()
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
    gainParam = vts.getRawParameterValue ("gain");
    orderParam = vts.getRawParameterValue ("order");
    satParam = vts.getRawParameterValue ("sat");
    freqParam = vts.getRawParameterValue ("freq");

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int order = 1; order <= maxOrder; ++order)
            allpass[ch][order-1].reset (new AllpassLadder (order));
    }

    filter.state->type = Parameters::Type::lowPass;
    dcBlocker.state->type = Parameters::Type::highPass;
}

NonlienarAllpassAudioProcessor::~NonlienarAllpassAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout NonlienarAllpassAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    StringArray orderChoices;
    for (int order = 1; order <= maxOrder; ++order)
        orderChoices.add(String (order));

    NormalisableRange<float> gainRange (0.0f, 20.0f);
    gainRange.setSkewForCentre (1.0f);

    NormalisableRange<float> freqRange (20.0f, 20000.0f);
    freqRange.setSkewForCentre (1000.0f);

    params.push_back (std::make_unique<AudioParameterFloat> ("gain", "Gain", gainRange, 0.5f));
    params.push_back (std::make_unique<AudioParameterChoice> ("order", "Order", orderChoices, 0));
    params.push_back (std::make_unique<AudioParameterChoice> ("sat", "Saturator", Saturators::getSatChoices(), 0));
    params.push_back (std::make_unique<AudioParameterFloat> ("freq", "Cutoff", freqRange, 20000.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const String NonlienarAllpassAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NonlienarAllpassAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NonlienarAllpassAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NonlienarAllpassAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NonlienarAllpassAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NonlienarAllpassAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NonlienarAllpassAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NonlienarAllpassAudioProcessor::setCurrentProgram (int index)
{
}

const String NonlienarAllpassAudioProcessor::getProgramName (int index)
{
    return {};
}

void NonlienarAllpassAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NonlienarAllpassAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        for (int order = 1; order <= maxOrder; ++order)
            allpass[ch][order-1]->reset();

    dsp::ProcessSpec spec { sampleRate, (uint32) samplesPerBlock, 2 };
    filter.reset();
    filter.prepare (spec);
    filter.state->setCutOffFrequency (sampleRate, *freqParam);

    dcBlocker.reset();
    dcBlocker.prepare (spec);
    dcBlocker.state->setCutOffFrequency (sampleRate, 30.0f);
}

void NonlienarAllpassAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NonlienarAllpassAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NonlienarAllpassAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    saturator = Saturators::getSaturator (static_cast<SatType> ((int) *satParam));

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        auto thisAPF = allpass[ch][(int) *orderParam].get();

        auto* x = osBuffer.getWritePointer (ch);

        for (int n = 0; n < osBuffer.getNumSamples(); ++n)
        {
            thisAPF->setCoefs (saturator (*gainParam * x[n]));
            x[n] = thisAPF->process (x[n]);
        }
    }

    oversampling.processSamplesDown (block);

    filter.state->setCutOffFrequency (getSampleRate(), *freqParam);
    dsp::ProcessContextReplacing context (block);
    filter.process (context);

    dcBlocker.process (context);
}

//==============================================================================
bool NonlienarAllpassAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NonlienarAllpassAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this); // NonlienarAllpassAudioProcessorEditor (*this);
}

//==============================================================================
void NonlienarAllpassAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NonlienarAllpassAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NonlienarAllpassAudioProcessor();
}
