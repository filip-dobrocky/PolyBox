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
#include "SamplerComponent.h"
#include "AppLookAndFeel.h"
#include "Tabs.h"

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

        PlayButton playButton;
        ResetButton resetButton;
        RecordButton recordButton;
        Slider bpmSlider;
        ToggleButton syncButton;
        DurationSlider durationSlider;
        SamplerComponent samplerComponent;

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
    AppLookAndFeel appLookAndFeel;
    PolyBoxAudioProcessor& audioProcessor;
    Tabs tabs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyBoxAudioProcessorEditor)
};
