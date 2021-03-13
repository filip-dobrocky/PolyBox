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
class SequencerGrid  : public juce::Component
{
public:
    SequencerGrid(PolySequencer *sequencer);
    ~SequencerGrid() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PolySequencer *sequencer;
    juce::OwnedArray<SequencerStep> steps[NUM_VOICES];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerGrid)
};
