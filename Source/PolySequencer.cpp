#include "PolySequencer.h"

PolySequencer::PolySequencer() : tempo(120),
								 duration(1),
								 timeSignature(Fraction{ 4, 4 })
{
	for (int i = 0; i < NUM_VOICES; i++)
	{
		voices[i] = new SequencerVoice(i, DEFAULT_STEPS);
		voices[i]->onLengthChange = [&] { lengthChanged(); };
	}

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

float PolySequencer::getTempo() { return tempo; }

int PolySequencer::getDuration() { return duration; }

bool PolySequencer::isPlaying() { return playing; };

float PolySequencer::getTimeSignature() { return (float)timeSignature.a / (float)timeSignature.b; }

void PolySequencer::setTempo(float tempo) 
{
	this->tempo = tempo;
	if (getIntervalInSamples() == 1)
	{
		steps = calculateSteps();
		lengthChanged();
	}
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

int PolySequencer::getIntervalInSamples()
{
	int interval = sampleRate * (((240.0f / (double)tempo) * duration * (double)timeSignature.a / (double)timeSignature.b) / (double)steps);
	return (interval > 0) ? interval : 1;
}

void PolySequencer::lengthChanged()
{
	auto oldSteps = steps;
	steps = calculateSteps();
	auto newPos = position * ((float)steps / (float)oldSteps);
	position = jlimit<int>(0, steps - 1, truncatePositiveToUnsignedInt(newPos));

	for (auto voice : voices)
	{
		voice->setPosition(position / (steps / voice->getLength()));
	}
}

void PolySequencer::setSampleRate(int sampleRate)
{
	this->sampleRate = sampleRate;
	calculateSteps();
}

void PolySequencer::transpose(int rootNote, int transposeNote)
{
	transposition = transposeNote > -1 && transposeNote < 128 
					? transposeNote - rootNote
					: 0;
}

void PolySequencer::transposeOff()
{
	transposition = 0;
}

void PolySequencer::setNormalizedPosition(float position)
{
	this->position = (int)(position * steps);
	for (auto voice : voices)
		voice->setPosition((int)(position * voice->getLength()));
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

	int max = sampleRate * (240.0f / tempo) * duration * ((double)timeSignature.a / (double)timeSignature.b);

	return result > max ? max : result;
}

void PolySequencer::play()
{
	steps = calculateSteps();
	playing = true;
}

void PolySequencer::stop()
{
	playing = false;
	midiMessages.clear();
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

			auto buffer = voices[i]->getNoteOff(sample);
			midiMessages.addEvents(buffer, buffer.getFirstEventTime(), buffer.getLastEventTime(), 0);
			
			MidiBuffer noteOnBuffer = voices[i]->getNoteOn(sample + 1, transposition);
			buffer.swapWith(noteOnBuffer);
			midiMessages.addEvents(buffer, buffer.getFirstEventTime(), buffer.getLastEventTime(), 0);
		}
	}

	if (++position == steps)
		position = 0;

	lastSample = sample;
}


