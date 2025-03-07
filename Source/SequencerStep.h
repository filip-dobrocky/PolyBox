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

class SequencerStep  : public Component,
                       public MidiInputCallback,
                       public MidiKeyboardStateListener
{
public:
    SequencerStep(Note* note);
    ~SequencerStep() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDoubleClick(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    
    void erase();

    void setNoteNumber(int number);
    void setVelocity(float velocity);
    void setProbability(double probability);

    int getNoteNumber();
    float getVelocity();
    double getProbability();
    
    void setActive(bool active);
    void setSelected(bool selected);
    void setRecording(bool recording);

    bool isActive();
    bool isSelected();

    enum StepColour
    {
        cInactive = 0xff235ABE,
        cActive = 0xff123b7b,
        cBorder = 0xc8ffffff,
        cSelected = 0xffBE8723,
        cRecording = 0xe0f40600
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
    AudioDeviceManager deviceManager;
    MidiKeyboardState keyboardState;

    float dragWidth;
    float dragHeight;

    bool active;
    bool recording;
    bool selected;

    ListenerList<Listener> listeners;
    void callStepSelectedListeners();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerStep)
};
