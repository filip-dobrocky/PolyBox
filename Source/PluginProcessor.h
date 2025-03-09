/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Tunings.h"
#include "MicroSampler.h"
#include "PolySequencer.h"
#include "Constants.h"

using namespace juce;
using namespace Tunings;

//==============================================================================
/**
*/
class PolyBoxAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PolyBoxAudioProcessor();
    ~PolyBoxAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    bool isSoundLoaded(int midiChannel);
	
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool canSync();
	MicroSamplerSound* loadSample(AudioFormatReader* source, String path, int midiChannel);
    bool loadScl(String path);
    bool loadKbm(String path);

    PolySequencer sequencer;
    Synthesiser sampler;
    AudioProcessorValueTreeState parameters;

    std::shared_ptr<Tuning> tuning;
    Scale scl;
    KeyboardMapping kbm;
    String sclPath;
    String kbmPath;
    bool equalTuning = false;
    int equalTuningSpan = 0;
    int equalTuningDivision = 0;

private:
    int sampleCounter = 0;
    int clockInterval = 0;
    int playedNote = -1;
    bool stopped = true;

    float previousLevel;

    AudioFormatManager formatManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolyBoxAudioProcessor)
};
