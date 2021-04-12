#include <JuceHeader.h>
#include "SequencerGrid.h"

using namespace juce;
//==============================================================================
SequencerGrid::SequencerGrid(PolySequencer& s) : sequencer(s)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        addAndMakeVisible(rows[i] = new SequencerRow(sequencer.voices[i], this));
    }

    noteSlider.setEnabled(selectedStep);
    noteSlider.s.onValueChange = [&] { noteChanged(); };
    velocitySlider.s.onValueChange = [&] { velocityChanged(); };
    probabilitySlider.s.onValueChange = [&] { probabilityChanged(); };
    velocitySlider.setEnabled(false);
    probabilitySlider.setEnabled(false);
    addAndMakeVisible(noteSlider);
    addAndMakeVisible(velocitySlider);
    addAndMakeVisible(probabilitySlider);

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
    FlexBox fbGrid, fbSliders;
    fbGrid.flexDirection = FlexBox::Direction::column;

    auto margin = FlexItem::Margin(10, 10, 10, 10);
    fbSliders.items.add(FlexItem(noteSlider).withFlex(1).withMargin(margin));
    fbSliders.items.add(FlexItem(velocitySlider).withFlex(1).withMargin(margin));
    fbSliders.items.add(FlexItem(probabilitySlider).withFlex(1).withMargin(margin));

    for (int i = 0; i < NUM_VOICES; i++)
    {
        fbGrid.items.add(FlexItem(*rows[i]).withMinHeight(50.0f).withFlex(1));
    }

    auto area = getLocalBounds();
    fbSliders.performLayout(area.removeFromBottom(getHeight() * 0.15).toFloat());
    fbGrid.performLayout(area.toFloat());
}

void SequencerGrid::timerCallback()
{
    for (auto row : rows)
        row->refresh();
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
    if (sequencer.isPlaying())
    {
        sequencer.stop();
        stopTimer();
    }
    else
    {
        sequencer.play();
        startTimer(30);
    }
}

void SequencerGrid::reset()
{
    erase();
    sequencer.reset();
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

SequencerGrid::SequencerRow::SequencerRow(SequencerVoice* v, SequencerGrid* g) : voice(v), grid(g)
{
    lengthSlider.setSliderStyle(Slider::SliderStyle::IncDecButtons);
    lengthSlider.setRange(1, 32, 1);
    lengthSlider.setValue(previousLength = voice->getLength());
    lengthSlider.setIncDecButtonsMode(Slider::IncDecButtonMode::incDecButtonsDraggable_Horizontal);
    lengthSlider.onValueChange = [&] { lengthChanged(); };
    addAndMakeVisible(lengthSlider);


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
        fb.items.add(FlexItem(*step).withFlex(1));
    }

    fbButtons.items.add(FlexItem(lengthSlider).withFlex(1).withMargin(FlexItem::Margin(0, 5, 0, 5)));
    lengthSlider.setTextBoxStyle(Slider::TextBoxAbove, false, lengthSlider.getWidth(), 20);
    
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

void SequencerGrid::SequencerRow::lengthChanged()
{
    const auto difference = lengthSlider.getValue() - previousLength;
    previousLength = lengthSlider.getValue();

    if (difference > 0)
    {
        for (int i = 0; i < difference; i++)
        {
            voice->grow();
            auto step = new SequencerStep(voice->getLastNotePtr());
            step->addListener(grid);
            addAndMakeVisible(steps.add(step));
        }
    }
    else if (difference < 0)
    {
        for (int i = 0; i < -difference; i++)
        {
            steps.removeLast();
            voice->shrink();
        }
    }

    resized();
}

void SequencerGrid::SequencerRow::refresh()
{
    for (int i = 0; i < steps.size(); i++)
    {
        if (i == voice->getPosition())
        {
            if (!steps[i]->isActive())
            {
                steps[i]->setActive(true);
                steps[i]->repaint();
            }
        }
        else if (steps[i]->isActive())
        {
            steps[i]->setActive(false);
            steps[i]->repaint();
        }
    }
}
