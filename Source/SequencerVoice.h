#pragma once
#include <vector>
#include <JuceHeader.h>

#define DEFAULT_STEPS 8
#define NUM_VOICES 6

struct Note {
	int number;
	float velocity;
	double probability;

	Note::Note() : number(-1), velocity (0.5f), probability(1) {}
	Note::Note(int num, float vel, double prob) : number(num), velocity(vel), probability(prob) {}
};

class SequencerVoice {

public:
	SequencerVoice(int index, int length);
	~SequencerVoice();

	int getPosition();
	void setPosition(int position);
	int getLength();
	Note* getNotePtr(int index);
	Note* getLastNotePtr();
	//void setLength(int length);

	void assignChannel(int channel);
	void deassignChannel(int channel);
	void eraseNote(int index);
	void grow();
	void shrink();
	MidiBuffer step(int sample);

	std::function<void()> onStep;
	std::function<void()> onLengthChange;

private:
	int index;
	int position;
	bool channels[NUM_VOICES];

	OwnedArray<Note> sequence;
	Random random;

	bool chance(double probability);
};
