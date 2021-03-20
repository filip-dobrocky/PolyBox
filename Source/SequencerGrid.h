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

        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        SequencerVoice* voice;
        SequencerGrid* grid;

        TextButton plusButton{ "+" };
        TextButton minusButton{ "-" };

        juce::OwnedArray<SequencerStep> steps;

        void addStep();
        void removeStep();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerRow)
    };

public:
    SequencerGrid(PolySequencer* sequencer);
    ~SequencerGrid() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void stepSelected(SequencerStep* step) override;

private:
    PolySequencer* sequencer;
    SequencerRow* rows[NUM_VOICES];
    SequencerStep* selectedStep{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
