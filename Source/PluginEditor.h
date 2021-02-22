/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SamplerTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SamplerTestAudioProcessorEditor (SamplerTestAudioProcessor&);
    ~SamplerTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TextButton mLoadSampleButton { "Load Sample" };
    TextButton mLoadTuningButton { "Load Tuning" };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SamplerTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerTestAudioProcessorEditor)
};
