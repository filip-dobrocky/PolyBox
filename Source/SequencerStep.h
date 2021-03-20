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
    SequencerStep(Note* note);
    ~SequencerStep() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& event) override;
    
    void erase();

    void setNote(Note* note);
    void setActive(bool active);
    void setSelected(bool selected);

    bool isActive();
    bool isSelected();

    enum StepColour
    {
        cInactive = 0xff696969,
        cActive = 0xffadd8e6,
        cBorder = 0xfffffff0,
        cSelected = 0xffff0000
    };

    class JUCE_API  Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void stepSelected(SequencerStep* step) = 0;
    };

    void addListener(Listener* listener);

    void removeListener(Listener* listener);


private:
    Note* note;

    bool active;
    bool selected;

    ListenerList<Listener> listeners;
    void callStepSelectedListeners();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerStep)
};
