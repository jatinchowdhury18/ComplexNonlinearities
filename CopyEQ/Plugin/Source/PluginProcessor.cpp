/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================
CopyEqAudioProcessor::CopyEqAudioProcessor() : 
    AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                                     .withInput  ("Sidechain",  AudioChannelSet::stereo(), true)),
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout())
{
    nablaParam = vts.getRawParameterValue ("nabla");
    rhoParam   = vts.getRawParameterValue ("rho");
    driveParam = vts.getRawParameterValue ("drive");
    satParam   = vts.getRawParameterValue ("sat");
    flipParam  = vts.getRawParameterValue ("flip");
    warpSideParam = vts.getRawParameterValue ("warpside");
    bypassParam   = vts.getRawParameterValue ("bypass");
}

CopyEqAudioProcessor::~CopyEqAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout CopyEqAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterFloat> ("nabla", "Learning Rate", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("rho", "Warping Factor", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("drive", "Drive [dB]", 0.0f, 36.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterChoice> ("sat", "Saturator", Saturators::getSatChoices(), 0));
    params.push_back (std::make_unique<AudioParameterBool>   ("flip", "Flip", false));
    params.push_back (std::make_unique<AudioParameterBool>   ("warpside", "Warp Sidechain", true));
    params.push_back (std::make_unique<AudioParameterBool>   ("bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

//==============================================================================
const String CopyEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CopyEqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CopyEqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CopyEqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CopyEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CopyEqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CopyEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CopyEqAudioProcessor::setCurrentProgram (int index)
{
}

const String CopyEqAudioProcessor::getProgramName (int index)
{
    return {};
}

void CopyEqAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CopyEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        eqs[ch].reset (sampleRate);
}

void CopyEqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool CopyEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // the sidechain can take any layout, the main bus needs to be the same on the input and output
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
        && ! layouts.getMainInputChannelSet().isDisabled();
}

void CopyEqAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    if (*bypassParam)
        return;
    
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto sidechainInput  = getBusBuffer (buffer, true, 1);

    // apply saturation
    saturator = Saturators::getSaturator (static_cast<SatType> ((int) *satParam));
    float driveGain = Decibels::decibelsToGain (*driveParam);

    buffer.applyGain (driveGain);
    for (int ch = 0; ch < mainInputOutput.getNumChannels(); ++ch)
        for (int n = 0; n <  buffer.getNumSamples(); ++n)
            buffer.setSample (ch, n, saturator (buffer.getSample (ch, n)));
    buffer.applyGain (1.0f / driveGain);

    for (int ch = 0; ch < mainInputOutput.getNumChannels(); ++ch)
    {
        eqs[ch].setNabla (*nablaParam * *nablaParam * 0.1f);
        eqs[ch].setRho (0.9f * *rhoParam);
        eqs[ch].setFlip ((bool) *flipParam);
        eqs[ch].setWarpSide ((bool) *warpSideParam);
    
        eqs[ch].processBlock (mainInputOutput.getWritePointer (ch), sidechainInput.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool CopyEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CopyEqAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void CopyEqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CopyEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CopyEqAudioProcessor();
}
