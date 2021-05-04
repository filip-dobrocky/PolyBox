/*
  ==============================================================================

    SequencerStep.cpp
    Created: 12 Mar 2021 7:26:14pm
    Author:  Filip

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SequencerStep.h"

using namespace juce;

//==============================================================================
SequencerStep::SequencerStep(Note* n) : note(n)
{
    selected = active = false;
}


SequencerStep::~SequencerStep()
{
}

void SequencerStep::paint (juce::Graphics& g)
{
    g.setColour(Colour(active ? StepColour::cActive : StepColour::cInactive));
    g.setOpacity(note->probability);
    g.fillAll();
    g.setColour(Colour(selected ? StepColour::cSelected : StepColour::cBorder));
    g.drawRect(getLocalBounds(), 2);

    if (note->number != -1)
    {
        float height = getHeight() - 2;
        float velocityLineH = height - note->velocity * (height - 2);
        g.setColour(Colour(StepColour::cBorder));
        g.setOpacity(0.4f);
        Line<float> velocityLine(Point<float>(2, velocityLineH),
            Point<float>(static_cast<float>(getWidth() - 2), velocityLineH));
        g.drawLine(velocityLine, 3.0f);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(String(note->number), getLocalBounds(),
            juce::Justification::centred, true);
    }
}

void SequencerStep::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void SequencerStep::erase()
{
    note->number = -1;
}

void SequencerStep::mouseDown(const MouseEvent& event)
{
    if (event.mouseWasClicked() && event.mods.isLeftButtonDown())
        callStepSelectedListeners();
}

void SequencerStep::setActive(bool active) { this->active = active; }

void SequencerStep::setSelected(bool selected)
{ 
    this->selected = selected;
    repaint();
}

bool SequencerStep::isActive() { return active; };

bool SequencerStep::isSelected() { return selected; };

void SequencerStep::addListener(SequencerStep::Listener* l)
{
    listeners.add(l);
}

void SequencerStep::removeListener(SequencerStep::Listener* l)
{
    listeners.remove(l);
}

void SequencerStep::callStepSelectedListeners()
{
    Component::BailOutChecker checker(this);
    listeners.callChecked(checker, [this](Listener& l) { l.stepSelected(this); });
}

void SequencerStep::setNoteNumber(int number) { note->number = number; }

void SequencerStep::setVelocity(float velocity) { note->velocity = velocity; }

void SequencerStep::setProbability(double probability) { note->probability = probability; }

int SequencerStep::getNoteNumber() { return note->number; }

float SequencerStep::getVelocity() { return note->velocity; }

double SequencerStep::getProbability() { return note->probability; }
