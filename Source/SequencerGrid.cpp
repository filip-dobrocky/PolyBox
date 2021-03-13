#include <JuceHeader.h>
#include "SequencerGrid.h"

using namespace juce;
//==============================================================================
SequencerGrid::SequencerGrid(PolySequencer *sequencer)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        for (auto note : sequencer->voices[i]->sequence)
        {
            addAndMakeVisible(steps[i].add(note ? new SequencerStep(*note) : new SequencerStep()));
        }
    }
}

SequencerGrid::~SequencerGrid()
{
}

void SequencerGrid::paint (juce::Graphics& g)
{
    
}

void SequencerGrid::resized()
{
    FlexBox fbRows;
    fbRows.flexDirection = FlexBox::Direction::column;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        FlexBox fbVoice;
        for (auto step : steps[i])
        {
            fbVoice.items.add(FlexItem(*step)); // withFlex?
        }
        fbRows.items.add(FlexItem(fbVoice).withMinHeight(50.0f));
        fbVoice.performLayout(getLocalBounds().toFloat());
    }
    fbRows.performLayout(getLocalBounds().toFloat());
}
