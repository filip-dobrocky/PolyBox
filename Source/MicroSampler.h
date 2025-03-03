/*
  ==============================================================================

	MicroSampler.h
	Created: 10 Dec 2020 1:26:03pm
	Author:  Filip

  ==============================================================================
*/

/*
  ==============================================================================
   Copyright (c) 2017 - ROLI Ltd.
   JUCE is an open source library subject to commercial or open-source
   licensing.
   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).
   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy
   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).
   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.
  ==============================================================================
*/


#pragma once
#include "JuceHeader.h"
#include "Tunings.h"

using namespace juce;

class MicroSamplerSound : public SynthesiserSound
{
public:
	MicroSamplerSound(const String& name,
		AudioFormatReader* source,
		String path,
		int midiChannel,
		const BigInteger& midiNotes,
		double frequencyForNormalPitch,
		double attack,
		double release);

	~MicroSamplerSound();

	const String& getName() const noexcept { return name; }


	AudioBuffer<float>* getAudioData() const noexcept { return data.get(); }


	void setEnvelopeParameters(ADSR::Parameters parametersToUse) { params = parametersToUse; }

	//==============================================================================
	bool appliesToNote(int midiNoteNumber) override;
	bool appliesToChannel(int midiChannel) override;

	void setAttack(double a);
	void setRelease(double r);
	void setStart(double s);
	void setEnd(double e);
	void setRoot(double frequency);
	void setReversed(bool reversed);

	double getAttack();
	double getRelease();
	double getStart();
	double getEnd();
	double getPlayingLengthInSeconds();
	double getRoot();
	bool getReversed();
	String getSourcePath();

	double pan = 0.0f;
	double gain = 1.0f;
	int midiChannel;

private:
	//==============================================================================
	friend class MicroSamplerVoice;

	String name;
	String sourcePath;
	std::unique_ptr<AudioBuffer<float>> data;
	double sourceSampleRate;
	BigInteger midiNotes;
	int startSample = 0;
	int endSample = 0;
	double start = 0.0f;
	double end = 1.0f;
	double rootFrequency = 0;
	double sourceLengthInSeconds = 0;
	bool reversed = false;

	ADSR::Parameters params;

	JUCE_LEAK_DETECTOR(MicroSamplerSound)
};



class MicroSamplerVoice : public SynthesiserVoice
{
public:

	MicroSamplerVoice(std::shared_ptr<Tunings::Tuning> tuning);

	~MicroSamplerVoice();

	//==============================================================================
	bool canPlaySound(SynthesiserSound*) override;

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
	void stopNote(float velocity, bool allowTailOff) override;

	void pitchWheelMoved(int newValue) override;
	void controllerMoved(int controllerNumber, int newValue) override;

	void renderNextBlock(AudioBuffer<float>&, int startSample, int numSamples) override;

private:
	//==============================================================================
	double pitchRatio = 0;
	double sourceSamplePosition = 0;
	float lgain = 0, rgain = 0;
	bool stopped = false;

	ADSR adsr;

	std::shared_ptr<Tunings::Tuning> tuning;

	JUCE_LEAK_DETECTOR(MicroSamplerVoice)
};
