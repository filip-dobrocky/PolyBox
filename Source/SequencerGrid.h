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

class SequencerRow : public Component
{
public:
    SequencerRow(SequencerVoice* voice);
    ~SequencerRow() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SequencerVoice* voice;
    juce::OwnedArray<SequencerStep> steps;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerRow)
};

class SequencerGrid  : public juce::Component
{
public:
    SequencerGrid(PolySequencer* sequencer);
    ~SequencerGrid() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PolySequencer* sequencer;
    SequencerRow* rows[NUM_VOICES];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
