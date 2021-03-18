#include <JuceHeader.h>
#include "SequencerGrid.h"

using namespace juce;
//==============================================================================
SequencerGrid::SequencerGrid(PolySequencer* sequencer)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        addAndMakeVisible(rows[i] = new SequencerRow(sequencer->voices[i]));
    }
}

SequencerGrid::~SequencerGrid()
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        delete rows[i];
    }
}

void SequencerGrid::paint (juce::Graphics& g)
{
    //g.fillAll(Colours::aquamarine);
}

void SequencerGrid::resized()
{
    FlexBox fb;
    fb.flexDirection = FlexBox::Direction::column;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        fb.items.add(FlexItem(*rows[i]).withMinHeight(50.0f).withFlex(1));
    }
    fb.performLayout(getLocalBounds().toFloat());
}

SequencerRow::SequencerRow(SequencerVoice* voice)
{
    this->voice = voice;
    for (auto step : voice->sequence)
        addAndMakeVisible(steps.add(new SequencerStep(step)));
}

SequencerRow::~SequencerRow()
{
}

void SequencerRow::paint(juce::Graphics& g)
{
    //g.fillAll(Colours::aquamarine);
}

void SequencerRow::resized()
{
    FlexBox fb;
    for (auto step : steps)
    {
        fb.items.add(FlexItem(*step).withFlex(1)); // withFlex?
    }
    fb.performLayout(getLocalBounds().toFloat());
}
