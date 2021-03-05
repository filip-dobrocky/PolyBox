#pragma once

#include <JuceHeader.h>
#include "SequencerVoice.h"
#include "Math.h"

#define NUM_VOICES 6

struct Fraction {
	int a;
	int b;
};

using namespace juce;

class PolySequencer : HighResolutionTimer
{

public:
	SequencerVoice *voices[NUM_VOICES];
	MidiBuffer midiMessages;

	PolySequencer(int tempo, int duration, Fraction *timeSignature);

	void hiResTimerCallback() override;

	void play();
	void stop();
	void reset();

	int getSteps();
	int getPosition();
	int getTempo();
	int getDuration();
	float getTimeSignature();
	bool isPlaying();

	void setTempo(int tempo);
	void setDuration(int duration);
	void setTimeSignature(int a, int b);

private:
	int steps;
	int position;
	int tempo;
	int duration;
	Fraction *timeSignature;
	bool playing;

	int calculateSteps();
	int getInterval();
	bool shouldPlay(SequencerVoice *v);
};