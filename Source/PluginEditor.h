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
class PolyBoxAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PolyBoxAudioProcessorEditor (PolyBoxAudioProcessor&);
    ~PolyBoxAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TextButton mLoadSampleButton { "Load Sample" };
    TextButton mLoadTuningButton { "Load Tuning" };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PolyBoxAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyBoxAudioProcessorEditor)
};
