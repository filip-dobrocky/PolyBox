#pragma once

#include <JuceHeader.h>
#include "SequencerVoice.h"
#include "Math.h"

struct Fraction {
	int a;
	int b;
};

using namespace juce;

class PolySequencer
{

public:
	PolySequencer();
	~PolySequencer();

	SequencerVoice* voices[6];
	MidiBuffer midiMessages;

	void tick(int sample);

	void play();
	void stop();
	void reset();

	int getSteps();
	int getPosition();
	float getTempo();
	int getDuration();
	float getTimeSignature();
	bool isPlaying();
	int getIntervalInSamples();

	void setTempo(float tempo);
	void setDuration(int duration);
	void setTimeSignature(int a, int b);
	void setSampleRate(int sampleRate);
	void transpose(int rootNote, int transposeNote);
	void transposeOff();
	void setNormalizedPosition(float position);

private:
	int steps;
	int position;
	int duration;
	int sampleRate = 48000;
	int lastSample;
	int transposition = 0;
	Fraction timeSignature;
	float tempo;
	double startTime;
	bool playing;

	int calculateSteps();
	void lengthChanged();
	bool shouldPlay(SequencerVoice* v);
};