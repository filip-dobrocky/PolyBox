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
    selected = active = recording = false;
    for (auto device : MidiInput::getAvailableDevices())
    {
        if (!deviceManager.isMidiInputDeviceEnabled(device.identifier))
            deviceManager.setMidiInputDeviceEnabled(device.identifier, true);

        deviceManager.addMidiInputDeviceCallback(device.identifier, this);
    }

    const Displays::Display* screen = Desktop::getInstance().getDisplays().getPrimaryDisplay();
    dragWidth = screen->userArea.getWidth() * 0.33;
    dragHeight = screen->userArea.getHeight() * 0.33;
}


SequencerStep::~SequencerStep()
{
}

void SequencerStep::paint (juce::Graphics& g)
{
    g.setColour(Colour(active ? StepColour::cActive : StepColour::cInactive));   
    g.fillAll();

    if (note->number != -1)
    {
        g.setColour(Colour(StepColour::cSelected));
        g.setOpacity(note->probability * 0.45f + 0.3f);
        g.fillAll();
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

    g.setColour(Colour(recording ? StepColour::cRecording : selected ? StepColour::cSelected : StepColour::cBorder));
    g.drawRect(getLocalBounds(), 2);
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

void SequencerStep::mouseDoubleClick(const MouseEvent& event)
{
    setRecording(true);
}

void SequencerStep::mouseDrag(const MouseEvent& event)
{
    auto pos = event.getOffsetFromDragStart();
    static int startNote = 60;
    static float startVel = 0.5f;
    static bool draggingX = false;

    if (event.mouseWasClicked()) { // on drag start
        draggingX = false;
        startNote = (note->number > -1) ? note->number : 60;
        startVel = note->velocity;
    }

    if (abs(pos.getX()) > 0.03 * dragWidth)
        draggingX = true;

    if (draggingX) // dragging x - thresholding
    {
        auto newNote = startNote + (pos.getX() / (float)dragWidth) * 64;
        note->number = jlimit<int>(-1, 127, newNote);
    }

	if (note->number != -1) // dragging y - always
    {
		auto newVel = startVel - pos.getY() / (float)dragHeight;
        note->velocity = jlimit<float>(0.0f, 1.0f, newVel);
    }
    callStepSelectedListeners();
}

void SequencerStep::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
    if (note->number != -1) {
        auto value = note->probability;
        value += wheel.deltaY / 10.0f;
        note->probability = value > 1.0f ? 1.0f : value < 0 ? 0.0f : value;
    }
    callStepSelectedListeners();
}

void SequencerStep::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
     note->number = midiNoteNumber;
     note->velocity = velocity;
}

void SequencerStep::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    const MessageManagerLock mmLock;
    setRecording(false);
}


void SequencerStep::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    keyboardState.processNextMidiEvent(message);
}

void SequencerStep::setActive(bool active) { this->active = active; }

void SequencerStep::setSelected(bool selected)
{ 
    this->selected = selected;
    repaint();
}

void SequencerStep::setRecording(bool recording)
{
    if (recording)
        keyboardState.addListener(this);
    else
        keyboardState.removeListener(this);
    this->recording = recording;
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
