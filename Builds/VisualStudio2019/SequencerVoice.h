#pragma once
#include <vector>

struct Note {
	int number;
	int velocity;
	double probability;
};

class SequencerVoice {

public:
	int lenght;
	std::vector<Note> sequence;
	int position;

	void insertNote(int index, Note note);
	void removeNote(int index);
	void play();
};
