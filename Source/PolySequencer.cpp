#include "PolySequencer.h"

PolySequencer::PolySequencer(int tempo, int duration, Fraction timeSignature)
{
	for (int i = 0; i < NUM_VOICES; i++)
	{
		voices[i] = new SequencerVoice(i, DEFAULT_STEPS);
		voices[i]->onLengthChange = [&] { lengthChanged(); };
	}
	
	this->tempo = tempo;
	this->duration = duration;
	this->timeSignature = timeSignature;

	playing = false;
	position = 0;
	steps = calculateSteps();
}

PolySequencer::~PolySequencer()
{
	for (int i = 0; i < NUM_VOICES; i++)
		delete voices[i];
}

int PolySequencer::getSteps() { return steps; }

int PolySequencer::getPosition() { return position; }

int PolySequencer::getTempo() { return tempo; }

int PolySequencer::getDuration() { return duration; }

bool PolySequencer::isPlaying() { return playing; };

float PolySequencer::getTimeSignature() { return (float)timeSignature.a / (float)timeSignature.b; }

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
	timeSignature.a = a;
	timeSignature.b = b;
}

int PolySequencer::getInterval()
{
	auto interval = ((240000 / tempo) * duration * timeSignature.a / timeSignature.b) / steps;
	return interval;
}

int PolySequencer::getIntervalInSamples()
{
	int interval = sampleRate * (((240.0f / (double)tempo) * duration * (double)timeSignature.a / (double)timeSignature.b) / (double)steps);
	return interval;
}

void PolySequencer::lengthChanged()
{
	auto oldSteps = steps;
	steps = calculateSteps();
	auto newPos = position * ((float)steps / (float)oldSteps);
	position = truncatePositiveToUnsignedInt(newPos);
	
	for (auto voice : voices)
	{
		voice->setPosition(position / (steps / voice->getLength()));
	}
}

void PolySequencer::setSampleRate(int sampleRate)
{
	this->sampleRate = sampleRate;
}

bool PolySequencer::shouldPlay(SequencerVoice* v)
{
	return !(position % (steps / v->getLength()));
}

bool PolySequencer::shouldStop(SequencerVoice* v)
{
	auto noteLen = steps / v->getLength();
	return noteLen - (position % noteLen) == 1;
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
	playing = true;
}

void PolySequencer::stop()
{
	playing = false;
}

void PolySequencer::reset()
{
	position = 0;
	for (auto voice : voices)
		voice->setPosition(0);
}

void PolySequencer::tick(int sample)
{
	for (int i = 0; i < NUM_VOICES; i++)
	{
		if (shouldPlay(voices[i]))
		{
			// To prevent shift on length change
			if (voices[i]->getPosition() != (position / (steps / voices[i]->getLength())))
				voices[i]->advance();

			auto buffer = voices[i]->getNoteOn(sample);
			midiMessages.addEvents(buffer, buffer.getFirstEventTime(), buffer.getLastEventTime(), 0);
		}
		else if (shouldStop(voices[i]))
		{
			auto buffer = voices[i]->getNoteOff(sample);
			midiMessages.addEvents(buffer, buffer.getFirstEventTime(), buffer.getLastEventTime(), 0);
		}
	}

	if (++position == steps)
		position = 0;
}


