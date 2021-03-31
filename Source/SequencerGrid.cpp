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

    this->sequencer = sequencer;

    noteSlider.setEnabled(selectedStep);
    noteSlider.s.onValueChange = [&] { noteChanged(); };
    velocitySlider.s.onValueChange = [&] { velocityChanged(); };
    probabilitySlider.s.onValueChange = [&] { probabilityChanged(); };
    addAndMakeVisible(noteSlider);
    addAndMakeVisible(velocitySlider);
    addAndMakeVisible(probabilitySlider);

    playButton.onClick = [&] { togglePlay(); };
    resetButton.onClick = [&] { reset(); };
    addAndMakeVisible(playButton);
    addAndMakeVisible(resetButton);

    erase();
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
    FlexBox fbGrid, fbSliders, fbButtons;
    fbGrid.flexDirection = FlexBox::Direction::column;

    auto margin = FlexItem::Margin(10, 10, 10, 10);
    fbSliders.items.add(FlexItem(noteSlider).withFlex(1).withMargin(margin));
    fbSliders.items.add(FlexItem(velocitySlider).withFlex(1).withMargin(margin));
    fbSliders.items.add(FlexItem(probabilitySlider).withFlex(1).withMargin(margin));

    fbButtons.items.add(FlexItem(playButton).withFlex(1).withMargin(margin));
    fbButtons.items.add(FlexItem(resetButton).withFlex(1).withMargin(margin));

    for (int i = 0; i < NUM_VOICES; i++)
    {
        fbGrid.items.add(FlexItem(*rows[i]).withMinHeight(50.0f).withFlex(1));
    }

    auto area = getLocalBounds();
    fbButtons.performLayout(area.removeFromBottom(getHeight() * 0.15).toFloat());
    fbSliders.performLayout(area.removeFromBottom(getHeight() * 0.15).toFloat());
    fbGrid.performLayout(area.toFloat());
}

void SequencerGrid::stepSelected(SequencerStep* step)
{
    if (selectedStep)
        selectedStep->setSelected(false);
    step->setSelected(true);
    selectedStep = step;
    noteSlider.setEnabled(selectedStep);
    noteSlider.s.setValue(selectedStep->getNoteNumber());
    velocitySlider.s.setValue(selectedStep->getVelocity());
    probabilitySlider.s.setValue(selectedStep->getProbability());
}

void SequencerGrid::noteChanged()
{
    auto value = noteSlider.s.getValue();
    auto isNote = value != -1;
    velocitySlider.setEnabled(isNote);
    probabilitySlider.setEnabled(isNote);
    selectedStep->setNoteNumber(value);
    selectedStep->repaint();
}

void SequencerGrid::velocityChanged()
{
    auto value = velocitySlider.s.getValue();
    selectedStep->setVelocity(value);
    selectedStep->repaint();
}

void SequencerGrid::probabilityChanged()
{
    auto value = probabilitySlider.s.getValue();
    selectedStep->setProbability(value);
    selectedStep->repaint();
}

void SequencerGrid::togglePlay()
{
    if (sequencer->isPlaying())
    {
        sequencer->stop();
        playButton.setButtonText("Play");
    }
    else
    {
        sequencer->play();
        playButton.setButtonText("Stop");
    }
}

void SequencerGrid::reset()
{
    erase();
    sequencer->reset();
}

void SequencerGrid::erase()
{
    for (auto row : rows)
    {
        int i = 0;
        for (auto step : row->steps)
        {
            if (i++ == 0)
                step->setActive(true);
            else
                step->setActive(false);
            step->repaint();
        }
    }
            
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

    voice->onStep = [&] { advance(); };
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

/*void SequencerGrid::SequencerRow::positionAdvanced(int position)
{
    auto activeStep = steps[position];
    auto previousStep = position ? steps[position - 1] : steps[steps.size() - 1];
    activeStep->setActive(true);
    previousStep->setActive(false);
    activeStep->repaint();
    previousStep->repaint();
}*/

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

void SequencerGrid::SequencerRow::advance()
{
    auto activeStep = steps[voice->getPosition()];
    auto previousStep = voice->getPosition() ? steps[voice->getPosition() - 1] : steps[steps.size() - 1];
    activeStep->setActive(true);
    previousStep->setActive(false);
    activeStep->repaint();
    previousStep->repaint();
}
