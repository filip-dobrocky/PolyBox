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

	assignChannel(index);
	
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

bool SequencerVoice::hasChannel(int channel)
{
	return channels[channel];
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

void SequencerVoice::advance()
{
	if (++position == sequence.size())
		position = 0;
}

MidiBuffer SequencerVoice::getNoteOn(int sample, int transposition)
{
	auto note = sequence[position];
	MidiBuffer buffer;

	if (note->number != -1)
	{
		if (chance(note->probability))
		{
			playedNote = jlimit<int>(0, 127, note->number + transposition);

			for (int i = 0; i < NUM_VOICES; i++)
			{
				if (channels[i])
				{
					buffer.addEvent(MidiMessage::noteOn(i + 1, playedNote, note->velocity), sample);
				}
			}

			
		}
	}

	return buffer;
}

MidiBuffer SequencerVoice::getNoteOff(int sample)
{
	MidiBuffer buffer;

	for (int i = 0; i < NUM_VOICES; i++)
	{
		if (channels[i])
		{
			if (playedNote != -1)
			{
				buffer.addEvent(MidiMessage::noteOff(i + 1, playedNote), sample);
			}
		}
	}

	playedNote = -1;

	return buffer;
}

bool SequencerVoice::chance(double probability)
{
	return random.nextDouble() <= probability;
}

