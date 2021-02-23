#include "SequencerVoice.h"

SequencerVoice::SequencerVoice(int length)
{
	this->length = length;
	this->position = 0;
	sequence.reserve(length);
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

void SequencerVoice::step()
{
	// create midi note

	if (++position == length)
		position = 0;
}



