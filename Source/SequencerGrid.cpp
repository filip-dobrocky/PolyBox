#include <JuceHeader.h>
#include "SequencerGrid.h"

using namespace juce;
//==============================================================================
SequencerGrid::SequencerGrid(PolySequencer* sequencer)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        addAndMakeVisible(rows[i] = new SequencerRow(sequencer->voices[i], this));
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

void SequencerGrid::stepSelected(SequencerStep* step)
{
    if (selectedStep)
        selectedStep->setSelected(false);
    step->setSelected(true);
    selectedStep = step;
    //step->setNote(new Note(60, 0.6, 0.95));
}

SequencerGrid::SequencerRow::SequencerRow(SequencerVoice* voice, SequencerGrid* grid)
{
    this->voice = voice;
    this->grid = grid;

    plusButton.onClick = [&] { addStep(); };
    minusButton.onClick = [&] { removeStep(); };

    addAndMakeVisible(plusButton);
    addAndMakeVisible(minusButton);

    for (int i = 0; i < voice->getLength(); i++)
    {
        auto step = new SequencerStep(voice->getNotePtr(i));
        step->addListener(grid);
        addAndMakeVisible(steps.add(step));
    }
}

SequencerGrid::SequencerRow::~SequencerRow()
{
}

void SequencerGrid::SequencerRow::paint(juce::Graphics& g)
{
    //g.fillAll(Colours::aquamarine);
}

void SequencerGrid::SequencerRow::resized()
{
    FlexBox fb;
    FlexBox fbButtons;

    for (auto step : steps)
    {
        fb.items.add(FlexItem(*step).withFlex(1)); // withFlex?
    }

    fbButtons.items.add(FlexItem(plusButton).withFlex(1));
    fbButtons.items.add(FlexItem(minusButton).withFlex(1));
    
    auto area = getLocalBounds();
    fbButtons.performLayout(area.removeFromRight(getWidth() * 0.1).toFloat());
    fb.performLayout(area.toFloat());
}

void SequencerGrid::SequencerRow::addStep()
{
    voice->grow();
    auto step = new SequencerStep(voice->getLastNotePtr());
    step->addListener(grid);
    addAndMakeVisible(steps.add(step));
    resized();
}

void SequencerGrid::SequencerRow::removeStep()
{
    steps.removeLast();
    voice->shrink();
    resized();
}
