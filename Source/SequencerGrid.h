/*
  ==============================================================================

    SequencerGrid.h
    Created: 12 Mar 2021 6:28:19pm
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PolySequencer.h"
#include "SequencerStep.h"
#include "Components.h"

//==============================================================================
/*
*/

class SequencerGrid : public juce::Component,
                      public SequencerStep::Listener,
                      public Timer
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
    SequencerGrid(PolySequencer& sequencer);
    ~SequencerGrid() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void togglePlay();
    void reset();

    void stepSelected(SequencerStep* step) override;

private:
    PolySequencer& sequencer;
    SequencerRow* rows[NUM_VOICES];
    SequencerStep* selectedStep{ nullptr };
    NoteSlider noteSlider;
    FloatSlider velocitySlider{ "Velocity", false, 0.0f, 1.0f };
    FloatSlider probabilitySlider{ "Probability", false, 0.0f, 1.0f };

    void noteChanged();
    void velocityChanged();
    void probabilityChanged();
    void erase();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
