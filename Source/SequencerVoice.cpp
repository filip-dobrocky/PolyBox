#include "SequencerVoice.h"

using namespace juce;

SequencerVoice::SequencerVoice(int length)
{
	this->length = length;
	this->position = 0;
	sequence.reserve(length); // initialize? / define empty note 
	random = new Random(Time::currentTimeMillis());
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

void SequencerVoice::insertNote(int position, Note note)
{
	if (position < length)
		sequence[position] = note;
	else
		throw "Index out of bounds";
}

void SequencerVoice::removeNote(int position)
{
	if (position < length)
		sequence.erase(sequence.begin() + position);
}

MidiMessage SequencerVoice::step()
{
	Note note = sequence[position];

	//TODO: null check
	//if (note)
	//{
		if (chance(note.probability))
		{
			// create midi note
		}
	//}

	if (++position == length)
		position = 0;

	//return midi note
}

bool SequencerVoice::chance(double probability)
{
	return random->nextDouble() >= probability;
}

