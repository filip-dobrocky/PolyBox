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


//==============================================================================
/**
	A subclass of SynthesiserSound that represents a sampled audio clip.
	This is a pretty basic sampler, and just attempts to load the whole audio stream
	into memory.
	To use it, create a Synthesiser, add some SamplerVoice objects to it, then
	give it some SampledSound objects to play.
	@see SamplerVoice, Synthesiser, SynthesiserSound
	@tags{Audio}
*/

using namespace juce;

class MicroSamplerSound : public SynthesiserSound
{
public:
	//==============================================================================
	/** Creates a sampled sound from an audio reader.
		This will attempt to load the audio from the source into memory and store
		it in this object.
		@param name         a name for the sample
		@param source       the audio to load. This object can be safely deleted by the
							caller after this constructor returns
		@param midiNotes    the set of midi keys that this sound should be played on. This
							is used by the SynthesiserSound::appliesToNote() method
		@param midiNoteForNormalPitch   the midi note at which the sample should be played
										with its natural rate. All other notes will be pitched
										up or down relative to this one
		@param attackTimeSecs   the attack (fade-in) time, in seconds
		@param releaseTimeSecs  the decay (fade-out) time, in seconds
		@param maxSampleLengthSeconds   a maximum length of audio to read from the audio
										source, in seconds
	*/
	MicroSamplerSound(const String& name,
		AudioFormatReader* source,
		int midiChannel,
		const BigInteger& midiNotes,
		double frequencyForNormalPitch,
		double attack,
		double release);

	/** Destructor. */
	~MicroSamplerSound();

	//==============================================================================
	/** Returns the sample's name */
	const String& getName() const noexcept { return name; }

	/** Returns the audio sample data.
		This could return nullptr if there was a problem loading the data.
	*/
	AudioBuffer<float>* getAudioData() const noexcept { return data.get(); }

	//==============================================================================
	/** Changes the parameters of the ADSR envelope which will be applied to the sample. */
	void setEnvelopeParameters(ADSR::Parameters parametersToUse) { params = parametersToUse; }

	//==============================================================================
	bool appliesToNote(int midiNoteNumber) override;
	bool appliesToChannel(int midiChannel) override;

	void setAttack(double a);
	void setRelease(double r);
	void setStart(double s);
	void setEnd(double e);
	void setRoot(double frequency);

	double getAttack();
	double getRelease();
	double getStart();
	double getEnd();
	double getPlayingLengthInSeconds();
	double getRoot();

	double pan = 0.0f;
	double gain = 1.0f;

private:
	//==============================================================================
	friend class MicroSamplerVoice;

	String name;
	std::unique_ptr<AudioBuffer<float>> data;
	double sourceSampleRate;
	BigInteger midiNotes;
	int startSample = 0;
	int endSample = 0;
	double start = 0.0f;
	double end = 1.0f;
	double rootFrequency = 0;
	double sourceLengthInSeconds = 0;
	int channel;

	ADSR::Parameters params;

	JUCE_LEAK_DETECTOR(MicroSamplerSound)
};


//==============================================================================
/**
	A subclass of SynthesiserVoice that can play a SamplerSound.
	To use it, create a Synthesiser, add some SamplerVoice objects to it, then
	give it some SampledSound objects to play.
	@see SamplerSound, Synthesiser, SynthesiserVoice
	@tags{Audio}
*/

class MicroSamplerVoice : public SynthesiserVoice
{
public:
	//==============================================================================
	/** Creates a SamplerVoice. */
	MicroSamplerVoice(Tunings::Tuning& tuning);

	/** Destructor. */
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

	Tunings::Tuning& tuning;

	JUCE_LEAK_DETECTOR(MicroSamplerVoice)
};
