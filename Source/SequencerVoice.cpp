#include "SequencerVoice.h"

using namespace juce;

SequencerVoice::SequencerVoice(int index, int length)
{
	this->length = length;
	this->position = 0;

	sequence.reserve(length);
	for (int i = 0; i < length; i++)
		sequence.push_back(nullptr);

	for (int i = 0; i < NUM_VOICES; i++)
		channels[i] = false;
	
	random.setSeed(Time::currentTimeMillis());
}

int SequencerVoice::getLength()
{
	return length;
}

void SequencerVoice::setLength(int length)
{
	this->length = length;
	sequence.resize(length);
}

int SequencerVoice::getPosition()
{
	return position;
}

void SequencerVoice::insertNote(int position, int noteNumber, float velocity, double probability)
{
	if (position < length)
		sequence[position] = new Note{noteNumber, velocity, probability};
	else
		throw "Index out of bounds";
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


void SequencerVoice::removeNote(int position)
{
	if (position < length)
	{
		delete sequence[position];
		sequence.erase(sequence.begin() + position);
	}
}

MidiBuffer SequencerVoice::step(int sample)
{
	Note *note = sequence[position];
	MidiBuffer buffer;

	if (note)
	{
		if (chance(note->probability))
		{
			for (int i = 0; i < NUM_VOICES; i++)
				if (channels[i])
					buffer.addEvent(MidiMessage::noteOn(i, note->number, note->velocity), sample);
		}
	}

	if (++position == length)
		position = 0;

	return buffer;
}

bool SequencerVoice::chance(double probability)
{
	return random.nextDouble() >= probability;
}

