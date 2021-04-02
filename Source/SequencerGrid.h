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
    public SequencerStep::Listener
{

    class SequencerRow : public Component/*,
                         public SequencerVoice::Listener*/
    {
    public:
        SequencerRow(SequencerVoice* voice, SequencerGrid* grid);
        ~SequencerRow() override;

        juce::OwnedArray<SequencerStep> steps;

        void paint(juce::Graphics&) override;
        void resized() override;
        //void positionAdvanced(int position) override;

    private:
        SequencerVoice* voice;
        SequencerGrid* grid;

        TextButton plusButton{ "+" };
        TextButton minusButton{ "-" };

        void addStep();
        void removeStep();
        void refresh();

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
    NoteSlider noteSlider;
    FloatSlider velocitySlider{ "Velocity" };
    FloatSlider probabilitySlider{ "Probability" };
    TextButton playButton{ "Play" };
    TextButton resetButton{ "Reset" };

    void noteChanged();
    void velocityChanged();
    void probabilityChanged();
    void togglePlay();
    void reset();
    void erase();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
