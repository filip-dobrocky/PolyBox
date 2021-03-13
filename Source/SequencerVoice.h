#pragma once
#include "PolySequencer.h"
#include <vector>
#include <JuceHeader.h>

#define DEFAULT_STEPS 8

struct Note {
	int number;
	float velocity;
	double probability;
};

using namespace juce;

class SequencerVoice {

private:
	int index;
	int length;
	int position;
	bool channels[NUM_VOICES];

	Random *random;

	bool chance(double probability);

public:
	SequencerVoice(int index, int length);

	std::vector<Note*> sequence;
	int getPosition();
	int getLength();
	void setLength(int length);

	void assignChannel(int channel);
	void deassignChannel(int channel);
	void insertNote(int index, int noteNumber, float velocity, double probability);
	void removeNote(int index);
	MidiBuffer step(int sample);
};
