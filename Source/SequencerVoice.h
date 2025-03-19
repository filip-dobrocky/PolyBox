#pragma once
#include <vector>
#include <JuceHeader.h>
#include "Constants.h"

#define DEFAULT_STEPS 8

struct Note {
	int number;
	float velocity;
	double probability;

	Note() : number(-1), velocity (0.5f), probability(1) {}
	Note(int num, float vel, double prob) : number(num), velocity(vel), probability(prob) {}
};

class SequencerVoice {

public:
	SequencerVoice(int index, int length);
	~SequencerVoice();

	int getPosition();
	void setPosition(int position);
	int getLength();
	void setLength(int length);
	Note* getNotePtr(int index);
	Note* getLastNotePtr();
	MidiBuffer getNoteOn(int sample, int transposition);
	MidiBuffer getNoteOff(int sample);

	void assignChannel(int channel);
	void deassignChannel(int channel);
	bool hasChannel(int channel);

	void eraseNote(int index);
	void grow();
	void shrink();
	void advance();

	std::function<void()> onLengthChange;

private:
	int index;
	int position;
	int playedNote = -1;
	bool channels[NUM_VOICES];

	OwnedArray<Note> sequence;
	Random random;

	bool chance(double probability);
};
