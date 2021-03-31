#include "SequencerVoice.h"

using namespace juce;

SequencerVoice::SequencerVoice(int index, int length)
{
	position = 0;
	this->index = index;

	sequence.ensureStorageAllocated(length);
	for (int i = 0; i < length; i++)
		sequence.add(new Note());

	for (int i = 0; i < NUM_VOICES; i++)
		channels[i] = false;
	
	random.setSeed(Time::currentTimeMillis());
}

SequencerVoice::~SequencerVoice()
{
	sequence.clear();
}

int SequencerVoice::getLength()
{
	return sequence.size();
}

/*void SequencerVoice::setLength(int length)
{
	sequence.resize(length);
}*/

int SequencerVoice::getPosition()
{
	return position;
}

void SequencerVoice::setPosition(int position)
{
	this->position = position;
}

Note* SequencerVoice::getNotePtr(int index)
{
	if (index < sequence.size() && index >= 0)
		return sequence[index];
	else
		throw "Index out of bounds";
}

Note* SequencerVoice::getLastNotePtr()
{
	return sequence.getLast();
}

void SequencerVoice::assignChannel(int channel)
{
	if (channel < NUM_VOICES && channel >= 0)
		channels[channel] = true;
	else
		throw "Channel out of bounds";
}

void SequencerVoice::deassignChannel(int channel)
{
	if (channel < NUM_VOICES && channel >= 0)
		channels[channel] = false;
	else
		throw "Channel out of bounds";
}


void SequencerVoice::eraseNote(int position)
{
	if (position < sequence.size())
	{
		auto note = sequence[position];
		note->number = -1;
	}
}

void SequencerVoice::grow()
{
	sequence.add(new Note());
	onLengthChange();
}

void SequencerVoice::shrink()
{
	sequence.removeLast();
	onLengthChange();
}

MidiBuffer SequencerVoice::step(int sample)
{
	auto note = sequence[position];
	MidiBuffer buffer;

	if (note->number != -1)
	{
		if (chance(note->probability))
		{
			for (int i = 0; i < NUM_VOICES; i++)
				if (channels[i])
					buffer.addEvent(MidiMessage::noteOn(i, note->number, note->velocity), sample);
		}
	}

	onStep();

	if (++position == sequence.size())
		position = 0;

	return buffer;
}

bool SequencerVoice::chance(double probability)
{
	return random.nextDouble() >= probability;
}

