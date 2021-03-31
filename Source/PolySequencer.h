#pragma once

#include <JuceHeader.h>
#include "SequencerVoice.h"
#include "Math.h"

struct Fraction {
	int a;
	int b;
};

using namespace juce;

class PolySequencer : HighResolutionTimer
{

public:
	PolySequencer(int tempo, int duration, Fraction timeSignature, int sampleRate);
	~PolySequencer();

	SequencerVoice* voices[6];
	MidiBuffer midiMessages;

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
	void setSampleRate(int sampleRate);

private:
	int steps;
	int position;
	int tempo;
	int duration;
	int sampleRate;
	Fraction timeSignature;
	double startTime;
	bool playing;

	int calculateSteps();
	int getInterval();
	void lengthChanged();
	bool shouldPlay(SequencerVoice* v);
};