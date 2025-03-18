/*
  ==============================================================================

    SequencerGrid.h
    Created: 12 Mar 2021 6:28:19pm
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PolySequencer.h"
#include "SequencerStep.h"
#include "Components.h"

//==============================================================================
/*
*/

class SequencerGrid : public juce::Component,
                      public SequencerStep::Listener
{

    class SequencerRow : public Component
    {
    public:
        SequencerRow(SequencerVoice* voice, SequencerGrid* grid);
        ~SequencerRow() override;

        juce::OwnedArray<SequencerStep> steps;

        void paint(juce::Graphics&) override;
        void resized() override;
        void refresh();

    private:
        SequencerVoice* voice;
        SequencerGrid* grid;

        Slider lengthSlider;
        int previousLength;


        void addStep();
        void removeStep();
        void lengthChanged();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerRow)
    };

public:
    SequencerGrid(PolyBoxAudioProcessor& p);
    ~SequencerGrid() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void reset();
	void refreshRows();

    void stepSelected(SequencerStep* step) override;

private:
	PolyBoxAudioProcessor& processor;
    PolySequencer& sequencer;
    SequencerRow* rows[NUM_VOICES];
    SequencerStep* selectedStep{ nullptr };
    NoteSlider noteSlider;
    FloatSlider velocitySlider{ "Velocity", Slider::LinearBar, false };
    FloatSlider probabilitySlider{ "Probability", Slider::LinearBar, false };

    void noteChanged();
    void velocityChanged();
    void probabilityChanged();
    void erase();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
