#include "SequencerVoice.h"

SequencerVoice::SequencerVoice(int length)
{
	this->length = length;
	sequence.reserve(length);
}

