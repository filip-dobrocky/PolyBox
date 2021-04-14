/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SequencerGrid.h"
#include "TuningSelector.h"
#include "SampleSource.h"

//==============================================================================
/**
*/
class PolyBoxAudioProcessorEditor : public juce::AudioProcessorEditor
{
    struct MainPage : Component
    {
        MainPage(PolyBoxAudioProcessor&);
        ~MainPage();

        void resized() override;

        SequencerGrid* sequencerGrid;
        OwnedArray<SampleSource> samples;

        TextButton playButton{ "Play" };
        TextButton resetButton{ "Reset" };
        Slider bpmSlider;
        ToggleButton syncButton;
        DurationSlider durationSlider;

        void toggleSync();
        void durationChanged();
        void bpmChanged();

        PolyBoxAudioProcessor& audioProcessor;
        PolySequencer& sequencer;
    };

    struct ConfigPage : Component,
                        ConnectionMatrix::Listener
    {
        ConfigPage(PolyBoxAudioProcessor&);

        void resized() override;

        void connectionChanged(int voice, int channel, bool state) override;

        ConnectionMatrix matrix;
        TuningSelector tuningSelector;
        PolyBoxAudioProcessor& audioProcessor;
        PolySequencer& sequencer;
    };

public:
    PolyBoxAudioProcessorEditor(PolyBoxAudioProcessor&);
    ~PolyBoxAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PolyBoxAudioProcessor& audioProcessor;
    TabbedComponent tabs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyBoxAudioProcessorEditor)
};
