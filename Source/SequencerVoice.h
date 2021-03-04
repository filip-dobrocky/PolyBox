#pragma once
#include <vector>
#include <JuceHeader.h>

#define DEFAULT_STEPS 8

struct Note {
	int number;
	int velocity;
	double probability;
};

class SequencerVoice {

private:
	int length;
	int position;

	juce::Random *random;

	bool chance(double probability);

public:
	SequencerVoice(int length);

	std::vector<Note> sequence;
	int getPosition();
	int getLength();
	void setLength(int length);

	void insertNote(int index, Note note);
	void removeNote(int index);
	void step();
};
