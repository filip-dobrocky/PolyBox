/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace Tunings;

//==============================================================================
PolyBoxAudioProcessor::PolyBoxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    tuning(std::make_shared<Tuning>())
{
    for (int i = 0; i < 72; i++)
        sampler.addVoice(new MicroSamplerVoice(*tuning));
}

PolyBoxAudioProcessor::~PolyBoxAudioProcessor()
{
}

//==============================================================================
const juce::String PolyBoxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolyBoxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PolyBoxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PolyBoxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PolyBoxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolyBoxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolyBoxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolyBoxAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PolyBoxAudioProcessor::getProgramName (int index)
{
    return {};
}

bool PolyBoxAudioProcessor::canSync()
{
    return getPlayHead();
}

void PolyBoxAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PolyBoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);
    sequencer.setSampleRate(sampleRate);
}

void PolyBoxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PolyBoxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void PolyBoxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //Sequencer Control
    auto interval = sequencer.getIntervalInSamples();
    if (clockInterval != interval)
    {
        DBG("INTERVAL " + String(interval));
        clockInterval = interval;
    }

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        if (sequencer.isPlaying())
        {
            if (sampleCounter++ == 0)
                sequencer.tick(i+1);
            if (sampleCounter >= clockInterval)
                sampleCounter = 0;
        }
        else
        {
            sampleCounter = 0;
        }
    }

    midiMessages.clear();

    midiMessages.addEvents(sequencer.midiMessages,
                           sequencer.midiMessages.getFirstEventTime(),
                           sequencer.midiMessages.getLastEventTime(), 0);

    sequencer.midiMessages.clear();

    if (syncOn)
    {
        if (auto ph = getPlayHead())
        {
            AudioPlayHead::CurrentPositionInfo info;
            if (ph->getCurrentPosition(info))
            {
                sequencer.setTempo(info.bpm);
                sequencer.setTimeSignature(info.timeSigNumerator, info.timeSigDenominator);
            }
        }
    }

    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool PolyBoxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolyBoxAudioProcessor::createEditor()
{
    return new PolyBoxAudioProcessorEditor (*this);
}

//==============================================================================
void PolyBoxAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PolyBoxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyBoxAudioProcessor();
}
