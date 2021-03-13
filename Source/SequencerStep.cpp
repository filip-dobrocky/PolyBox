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
SequencerStep::SequencerStep(Note note)
{
    this->note = note;
    rest = false;
}

SequencerStep::SequencerStep()
{
    rest = true;
}

SequencerStep::~SequencerStep()
{
}

void SequencerStep::paint (juce::Graphics& g)
{
    

    g.fillAll (Colour(active ? StepColour::active : StepColour::inactive));   // clear the background

    g.setColour (Colour(selected ? StepColour::selected : StepColour::border));
    g.drawRect (getLocalBounds(), 1);

    if (!rest)
    {
        float velocityLineH = note.velocity * getHeight();
        g.setColour(juce::Colours::lightslategrey);
        g.setOpacity(0.7f);
        Line<float> velocityLine(Point<float>(0, velocityLineH),
            Point<float>(static_cast<float>(getWidth()), velocityLineH));
        g.drawLine(velocityLine, 3.0f);

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(String(note.number), getLocalBounds(),
            juce::Justification::centred, true);
    }
}

void SequencerStep::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void SequencerStep::setNote(Note note)
{ 
    this->note = note;
    rest = false;
}

void SequencerStep::erase()
{
    rest = true;
}

void SequencerStep::setActive(bool active) { this->active = active; }

void SequencerStep::setSelected(bool selected) { this->selected = selected; }

bool SequencerStep::isActive() { return active; };

bool SequencerStep::isSelected() { return selected; };
