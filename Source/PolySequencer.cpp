#include "PolySequencer.h"

PolySequencer::PolySequencer(int tempo, int duration, Fraction timeSignature, int sampleRate)
{
	for (int i = 0; i < NUM_VOICES; i++)
	{
		voices[i] = new SequencerVoice(i, DEFAULT_STEPS);
		voices[i]->onLengthChange = [&] { lengthChanged(); };
	}
	
	this->tempo = tempo;
	this->duration = duration;
	this->timeSignature = timeSignature;
	this->sampleRate = sampleRate;

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

void PolySequencer::lengthChanged()
{
	auto oldSteps = steps;
	steps = calculateSteps();
	auto newPos = position * ((float)steps / (float)oldSteps);
	position = truncatePositiveToUnsignedInt(newPos);
	
	for (auto voice : voices)
		voice->setPosition(position / (steps / voice->getLength()));
	
	if (isTimerRunning())
		startTimer(getInterval());
}

void PolySequencer::setSampleRate(int sampleRate)
{
	this->sampleRate = sampleRate;
}

bool PolySequencer::shouldPlay(SequencerVoice* v)
{
	return !(position % (steps / v->getLength()));
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
	startTime = Time::getMillisecondCounterHiRes() * 0.001;
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
	for (auto voice : voices)
		voice->setPosition(0);
}

void PolySequencer::hiResTimerCallback()
{
	int sample = static_cast<int>(Time::getMillisecondCounterHiRes() * 0.001 - startTime) * sampleRate;

	for (int i = 0; i < NUM_VOICES; i++)
	{
		if (shouldPlay(voices[i]))
		{
			const MessageManagerLock mmLock;
			auto buffer = voices[i]->step(sample);
			midiMessages.addEvents(buffer, buffer.getFirstEventTime(), buffer.getLastEventTime(), 0);
		}
	}

	if (++position == steps)
		position = 0;
}


