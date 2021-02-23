#include "PolySequencer.h"

PolySequencer::PolySequencer()
{
	for (int i; i < NUM_VOICES; i++)
		voices[i] = new SequencerVoice(DEFAULT_STEPS);
}

int PolySequencer::getSteps()
{
	return steps;
}

int PolySequencer::getPosition()
{
	return position;
}

void PolySequencer::calculateSteps()
{
	//lowest common multiple
}

void PolySequencer::play()
{
	
}

void PolySequencer::stop()
{

}

void PolySequencer::reset()
{
	position = 0;
}

void PolySequencer::hiResTimerCallback()
{

}

