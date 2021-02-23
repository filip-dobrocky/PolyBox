#pragma once

#include <JuceHeader.h>
#include "SequencerVoice.h"

#define NUM_VOICES 6

class PolySequencer : juce::HighResolutionTimer
{
public:
	SequencerVoice *voices[NUM_VOICES];

	PolySequencer();

	void hiResTimerCallback() override;

	void play();
	void stop();
	void reset();

	int getSteps();
	int getPosition();

private:
	int steps;
	int position;

	void calculateSteps();
};