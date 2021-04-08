/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SequencerGrid.h"

//==============================================================================
/**
*/
class PolyBoxAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PolyBoxAudioProcessorEditor(PolyBoxAudioProcessor&);
    ~PolyBoxAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SequencerGrid* mSequencerGrid;

    TextButton playButton{ "Play" };
    TextButton resetButton{ "Reset" };
    Slider bpmSlider;
    ToggleButton syncButton;
    DurationSlider durationSlider;

    PolyBoxAudioProcessor& audioProcessor;

    void toggleSync();
    void durationChanged();
    void bpmChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyBoxAudioProcessorEditor)
};
