/*
  ==============================================================================

    SequencerStep.h
    Created: 12 Mar 2021 7:26:14pm
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SequencerVoice.h"

//==============================================================================
/*
*/

class SequencerStep  : public juce::Component
{
public:
    SequencerStep(Note note);
    SequencerStep();
    ~SequencerStep() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void erase();

    void setNote(Note note);
    void setActive(bool active);
    void setSelected(bool selected);

    bool isActive();
    bool isSelected();

    enum StepColour
    {
        inactive = 0xff696969,
        active = 0xffadd8e6,
        border = 0xfffffff0,
        selected = 0xfff5f5f5
    };

private:
    Note note;

    bool active;
    bool selected;
    bool rest;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerStep)
};
