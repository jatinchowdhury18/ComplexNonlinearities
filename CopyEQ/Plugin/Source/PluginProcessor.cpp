/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CopyEqAudioProcessor::CopyEqAudioProcessor() : 
    AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                                     .withInput  ("Sidechain",  AudioChannelSet::stereo(), true)),
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout())
{
    eqs[0].reset (new CopyEQ (filter[0], filter[1]));
    eqs[1].reset (new CopyEQ (filter[1], filter[0]));

    contParam   = vts.getRawParameterValue (Tags::continID);
    nablaParam  = vts.getRawParameterValue (Tags::nablaID);
    rhoParam    = vts.getRawParameterValue (Tags::rhoID);
    flipParam   = vts.getRawParameterValue (Tags::flipID);
    lpfParam    = vts.getRawParameterValue (Tags::lpfID);
    bypassParam = vts.getRawParameterValue (Tags::bypassID);
    dwParam     = vts.getRawParameterValue (Tags::dwID);
    stParam     = vts.getRawParameterValue (Tags::stID);
}

CopyEqAudioProcessor::~CopyEqAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout CopyEqAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> filtRange (20.0f, 22000.0f);
    filtRange.setSkewForCentre (1000.0f);

    params.push_back (std::make_unique<AudioParameterBool> (Tags::continID, "Continuous", false));
    params.push_back (std::make_unique<AudioParameterBool> (Tags::flipID,   "Flip",       false));
    params.push_back (std::make_unique<AudioParameterBool> (Tags::bypassID, "Bypass",     false));
                                                            
    params.push_back (std::make_unique<AudioParameterFloat> (Tags::nablaID, "Learning Rate",   0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (Tags::rhoID,   "Warping Factor", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (Tags::lpfID,   "Side Filter",     filtRange,  22000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (Tags::dwID,    "Dry/Wet",         0.0f, 1.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat> (Tags::stID,    "Stereo",          0.0f, 1.0f, 1.0f));

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
        eqs[ch]->reset (sampleRate, samplesPerBlock);

    samplesLearned = 0;

    dryBuffer.setSize (2, samplesPerBlock);
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

    for (int ch = 0; ch < mainInputOutput.getNumChannels(); ++ch)
        dryBuffer.copyFrom (ch, 0, mainInputOutput, ch, 0, buffer.getNumSamples());

    // set EQ params
    for (int ch = 0; ch < mainInputOutput.getNumChannels(); ++ch)
    {
        eqs[ch]->setNabla (*nablaParam * *nablaParam * 0.1f);
        eqs[ch]->setRho (0.9f * *rhoParam);
        eqs[ch]->setFlip ((bool) *flipParam);
        eqs[ch]->setSideCutoff (*lpfParam);
        eqs[ch]->setStereoFactor (*stParam);
    
        if (*contParam || learn) // learning mode
            eqs[ch]->processBlockLearn (mainInputOutput.getWritePointer (ch), 
                sidechainInput.getWritePointer (ch), buffer.getNumSamples());
        else
            eqs[ch]->processBlock (mainInputOutput.getWritePointer (ch), buffer.getNumSamples());
    }

    // update whether or not to keep training
    if (learn)
    {
        samplesLearned += buffer.getNumSamples();

        if (samplesLearned > (int) (lengthLearnSeconds * (float) getSampleRate()))
        {
            samplesLearned = 0;
            learn = false;
        }
    }

    // apply dry/wet
    mainInputOutput.applyGain (*dwParam);
    for (int ch = 0; ch < mainInputOutput.getNumChannels(); ++ch)
        mainInputOutput.addFrom (ch, 0, dryBuffer, ch, 0, buffer.getNumSamples(), 1.0f - *dwParam);
}

//==============================================================================
bool CopyEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CopyEqAudioProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void CopyEqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CopyEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new CopyEqAudioProcessor();
}
