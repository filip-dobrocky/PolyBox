#include "PolySequencer.h"

PolySequencer::PolySequencer(int tempo, int duration, Fraction* timeSignature)
{
	for (int i = 0; i < NUM_VOICES; i++)
		voices[i] = new SequencerVoice(DEFAULT_STEPS);
	
	this->tempo = tempo;
	this->duration = duration;
	this->timeSignature = timeSignature;
}

int PolySequencer::getSteps() { return steps; }

int PolySequencer::getPosition() { return position; }

int PolySequencer::getTempo() { return tempo; }

int PolySequencer::getDuration() { return duration; }

bool PolySequencer::isPlaying() { return playing };

float PolySequencer::getTimeSignature() { return (float)timeSignature->a / timeSignature->b; }

void PolySequencer::setTempo(int tempo) 
{
	this->tempo = tempo;
}

void PolySequencer::setDuration(int duration)
{
	this->duration = duration;
}

void PolySequencer::setTimeSignature(int a, int b)
{
	timeSignature->a = a;
	timeSignature->b = b;
}

int PolySequencer::getInterval()
{
	return (240000 / steps) * tempo * duration * timeSignature->a / timeSignature->b;
}

bool PolySequencer::shouldPlay(SequencerVoice* v)
{
	return !(position % v->getLength());
}

int PolySequencer::calculateSteps()
{
	int result = voices[0]->getLength();
	for (int i = 1; i < NUM_VOICES; i++)
		result = math::lcm(result, voices[i]->getLength());
	return result;
}

void PolySequencer::play()
{
	steps = calculateSteps();
	startTimer(getInterval());
	playing = true;
}

void PolySequencer::stop()
{
	stopTimer();
	playing = false;
}

void PolySequencer::reset()
{
	position = 0;
}

void PolySequencer::hiResTimerCallback()
{
	for (int i = 0; i < NUM_VOICES; i++)
	{
		if (shouldPlay(voices[i]))
			auto message = voices[i]->step();
		//post to buffer
	}

	if (++position == steps)
		position = 0;
}


