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
SequencerStep::SequencerStep(Note* note)
{
    this->note = note;
    selected = active = false;
}


SequencerStep::~SequencerStep()
{
}

void SequencerStep::paint (juce::Graphics& g)
{
    

    g.fillAll (Colour(active ? StepColour::cActive : StepColour::cInactive));

    g.setColour (Colour(selected ? StepColour::cSelected : StepColour::cBorder));
    g.drawRect (getLocalBounds(), 2);

    if (note)
    {
        float velocityLineH = note->velocity * getHeight();
        g.setColour(juce::Colours::lightslategrey);
        g.setOpacity(0.7f);
        Line<float> velocityLine(Point<float>(0, velocityLineH),
            Point<float>(static_cast<float>(getWidth()), velocityLineH));
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

void SequencerStep::setNote(Note* note)
{ 
    this->note = note;
}

void SequencerStep::erase()
{
    delete note;
    note = nullptr;
}

void SequencerStep::mouseDown(const MouseEvent& event)
{
    if (event.mouseWasClicked() && event.mods.isLeftButtonDown())
        setSelected(true);
}

void SequencerStep::setActive(bool active) { this->active = active; }

void SequencerStep::setSelected(bool selected)
{ 
    if (this->selected = selected)
        callStepSelectedListeners();
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
