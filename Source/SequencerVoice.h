#pragma once
#include <vector>

struct Note {
	int number;
	int velocity;
	double probability;
};

class SequencerVoice {

private:
	int lenght;
	int position;

public:
	std::vector<Note> sequence;

	int getPosition();
	int getLength();
	void setLength(int length);

	SequencerVoice(int length);

	void insertNote(int index, Note note);
	void removeNote(int index);
	void step();
};
