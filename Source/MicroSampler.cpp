/*
*	MicroSampler.cpp
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

#include "MicroSampler.h"

MicroSamplerSound::MicroSamplerSound(const String& soundName,
	AudioFormatReader& source,
	const BigInteger& notes,
	double frequencyForNormalPitch,
	double attackTimeSecs,
	double releaseTimeSecs,
	double maxSampleLengthSeconds)
	: name(soundName),
	sourceSampleRate(source.sampleRate),
	midiNotes(notes),
	rootFrequency(frequencyForNormalPitch)
{
	if (sourceSampleRate > 0 && source.lengthInSamples > 0)
	{
		length = jmin((int)source.lengthInSamples,
			(int)(maxSampleLengthSeconds * sourceSampleRate));

		data.reset(new AudioBuffer<float>(jmin(2, (int)source.numChannels), length + 4));

		source.read(data.get(), 0, length + 4, 0, true, true);

		params.attack = static_cast<float> (attackTimeSecs);
		params.release = static_cast<float> (releaseTimeSecs);
	}
}

MicroSamplerSound::~MicroSamplerSound()
{
}

bool MicroSamplerSound::appliesToNote(int midiNoteNumber)
{
	return midiNotes[midiNoteNumber];
}

bool MicroSamplerSound::appliesToChannel(int /*midiChannel*/)
{
	return true;
}

//==============================================================================
MicroSamplerVoice::MicroSamplerVoice(Tunings::Tuning* tuning) : tuning(tuning) {}
MicroSamplerVoice::~MicroSamplerVoice() {}

bool MicroSamplerVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<const MicroSamplerSound*> (sound) != nullptr;
}

void MicroSamplerVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
	if (auto* sound = dynamic_cast<const MicroSamplerSound*> (s))
	{
		pitchRatio = (tuning->frequencyForMidiNote(midiNoteNumber) / sound->rootFrequency)
			* sound->sourceSampleRate / getSampleRate();

		sourceSamplePosition = 0.0;
		lgain = velocity;
		rgain = velocity;

		adsr.setSampleRate(sound->sourceSampleRate);
		adsr.setParameters(sound->params);

		adsr.noteOn();
	}
	else
	{
		jassertfalse; // this object can only play MicroSamplerSounds!
	}
}

void MicroSamplerVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
	if (allowTailOff)
	{
		adsr.noteOff();
	}
	else
	{
		clearCurrentNote();
		adsr.reset();
	}
}

void MicroSamplerVoice::pitchWheelMoved(int /*newValue*/) {}
void MicroSamplerVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void MicroSamplerVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	if (auto* playingSound = static_cast<MicroSamplerSound*> (getCurrentlyPlayingSound().get()))
	{
		auto& data = *playingSound->data;
		const float* const inL = data.getReadPointer(0);
		const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

		float* outL = outputBuffer.getWritePointer(0, startSample);
		float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

		while (--numSamples >= 0)
		{
			auto pos = (int)sourceSamplePosition;
			auto alpha = (float)(sourceSamplePosition - pos);
			auto invAlpha = 1.0f - alpha;

			// just using a very simple linear interpolation here..
			float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
			float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
				: l;

			auto envelopeValue = adsr.getNextSample();

			l *= lgain * envelopeValue;
			r *= rgain * envelopeValue;

			if (outR != nullptr)
			{
				*outL++ += l;
				*outR++ += r;
			}
			else
			{
				*outL++ += (l + r) * 0.5f;
			}

			sourceSamplePosition += pitchRatio;

			if (sourceSamplePosition > playingSound->length)
			{
				stopNote(0.0f, false);
				break;
			}
		}
	}
}

