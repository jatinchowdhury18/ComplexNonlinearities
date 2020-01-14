/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SubharmonicsAudioProcessor::SubharmonicsAudioProcessor()
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
    preCutoffParam  = vts.getRawParameterValue ("prefreq_Hz");
    attackParam     = vts.getRawParameterValue ("attack_Ms");
    releaseParam    = vts.getRawParameterValue ("release_Ms");
    postCutoffParam = vts.getRawParameterValue ("postfreq_Hz");
    mainGainParam   = vts.getRawParameterValue ("maingain_dB");
    sideGainParam   = vts.getRawParameterValue ("sidegain_dB");
}

SubharmonicsAudioProcessor::~SubharmonicsAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout SubharmonicsAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> freqRange (20.0f, 20000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> attackRange (0.1f, 1000.0f);
    attackRange.setSkewForCentre (10.0f);

    NormalisableRange<float> releaseRange (1.0f, 3000.0f);
    releaseRange.setSkewForCentre (100.0f);

    params.push_back (std::make_unique<AudioParameterFloat> ("prefreq_Hz",  "Pre Filter",  freqRange, 500.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("attack_Ms",   "Attack",      attackRange, 10.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("release_Ms",  "Release",     releaseRange, 100.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("postfreq_Hz", "Post Filter", freqRange, 500.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("maingain_dB", "Main Gain", -60.0f, 30.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("sidegain_dB", "Side Gain", -60.0f, 30.0f, 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const String SubharmonicsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SubharmonicsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SubharmonicsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SubharmonicsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SubharmonicsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SubharmonicsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SubharmonicsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SubharmonicsAudioProcessor::setCurrentProgram (int index)
{
}

const String SubharmonicsAudioProcessor::getProgramName (int index)
{
    return {};
}

void SubharmonicsAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SubharmonicsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        sub[ch].reset ((float) sampleRate);

        mainGain[ch].prepare();
        sideGain[ch].prepare();

        preEQ[ch].reset (sampleRate);
        preEQ[ch].setEqShape (EqShape::lowPass);
        preEQ[ch].toggleOnOff (true);

        dcBlocker[ch].reset (sampleRate);
        dcBlocker[ch].setEqShape (EqShape::highPass);
        dcBlocker[ch].setFrequency (35.0f);
        dcBlocker[ch].setQ (0.7071f);

        for (int i = 0; i < 3; ++i)
        {
            postEQ[i][ch].reset (sampleRate);
            postEQ[i][ch].setEqShape (EqShape::lowPass);
            postEQ[i][ch].toggleOnOff (true);
        }
    }

    sidechainBuffer.setSize (2, samplesPerBlock);
}

void SubharmonicsAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SubharmonicsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SubharmonicsAudioProcessor::updateParams()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        mainGain[ch].setGain (Decibels::decibelsToGain (*mainGainParam));
        sideGain[ch].setGain (Decibels::decibelsToGain (*sideGainParam));

        sub[ch].setDetector (*attackParam, *releaseParam);

        preEQ[ch].setFrequency (*preCutoffParam);
        preEQ[ch].setQ (0.7071f);

        for (int i = 0; i < 3; ++i)
        {
            postEQ[i][ch].setFrequency (*postCutoffParam);
            postEQ[i][ch].setQ (butterQs[i]);
        }
    }
}

void SubharmonicsAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    
    updateParams();

    sidechainBuffer.makeCopyOf (buffer, true);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto main = buffer.getWritePointer (ch);
        auto side = sidechainBuffer.getWritePointer (ch);

        preEQ[ch].processBlock (side, numSamples);
        sub[ch].processBlock (side, numSamples);
        for (int i = 0; i < 3; ++i)
            postEQ[i][ch].processBlock (side, numSamples);

        dcBlocker[ch].processBlock (side, numSamples);

        mainGain[ch].processBlock (main, numSamples);
        sideGain[ch].processBlock (side, numSamples);
        
        buffer.addFrom (ch, 0, sidechainBuffer, ch, 0, numSamples);
    }
}

//==============================================================================
bool SubharmonicsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SubharmonicsAudioProcessor::createEditor()
{
    return new SubharmonicsAudioProcessorEditor (*this);
}

//==============================================================================
void SubharmonicsAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SubharmonicsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SubharmonicsAudioProcessor();
}
