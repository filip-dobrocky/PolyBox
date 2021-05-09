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
	AudioFormatReader* source,
	int midiChannel,
	const BigInteger& notes,
	double frequencyForNormalPitch,
	double attack,
	double release)
	: name(soundName),
	sourceSampleRate(source->sampleRate),
	midiNotes(notes),
	rootFrequency(frequencyForNormalPitch),
	channel(midiChannel)
{
	if (sourceSampleRate > 0 && source->lengthInSamples > 0)
	{
		endSample = source->lengthInSamples;

		data.reset(new AudioBuffer<float>(jmin(2, (int)source->numChannels), endSample + 4));

		source->read(data.get(), 0, endSample + 4, 0, true, true);

		sourceLengthInSeconds = source->lengthInSamples / sourceSampleRate;

		setAttack(attack);
		setRelease(release);
		params.decay = 0.0f;
	}

	delete source;
}

MicroSamplerSound::~MicroSamplerSound()
{
}

bool MicroSamplerSound::appliesToNote(int midiNoteNumber)
{
	return midiNotes[midiNoteNumber];
}

bool MicroSamplerSound::appliesToChannel(int midiChannel)
{
	return channel == midiChannel;
}

void MicroSamplerSound::setAttack(double a)
{
	// range: 0..1
	params.attack = a * 0.5f * getPlayingLengthInSeconds();
}

void MicroSamplerSound::setRelease(double r)
{
	// range: 0..1
	params.release = r * 0.5f * getPlayingLengthInSeconds();
}

void MicroSamplerSound::setStart(double s)
{
	startSample = static_cast<int>(data->getNumSamples() * s);
	start = s;
}

void MicroSamplerSound::setEnd(double e)
{
	endSample = static_cast<int>(data->getNumSamples() * e);
	end = e;
}

void MicroSamplerSound::setRoot(double frequency)
{
	rootFrequency = frequency;
}

double MicroSamplerSound::getAttack()
{
	return params.attack;
}

double MicroSamplerSound::getRelease()
{
	return params.release;
}

double MicroSamplerSound::getStart()
{
	return start;
}

double MicroSamplerSound::getEnd()
{
	return end;
}

double MicroSamplerSound::getPlayingLengthInSeconds()
{
	return (static_cast<double>(endSample) - static_cast<double>(startSample)) / sourceSampleRate;
}

double MicroSamplerSound::getRoot()
{
	return rootFrequency;
}

//==============================================================================
MicroSamplerVoice::MicroSamplerVoice(Tunings::Tuning& tuning) : tuning(tuning) {}
MicroSamplerVoice::~MicroSamplerVoice() {}

bool MicroSamplerVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<const MicroSamplerSound*> (sound) != nullptr && !adsr.isActive();
}

void MicroSamplerVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
	if (auto* sound = dynamic_cast<const MicroSamplerSound*> (s))
	{
		pitchRatio = (tuning.frequencyForMidiNote(midiNoteNumber) / sound->rootFrequency)
			* sound->sourceSampleRate / getSampleRate();

		sourceSamplePosition = sound->startSample;
		lgain = velocity * jmin(1 - sound->pan, 1.0) * sound->gain;
		rgain = velocity * jmin(1 + sound->pan, 1.0) * sound->gain;

		adsr.setSampleRate(sound->sourceSampleRate);
		adsr.setParameters(sound->params);
		stopped = false;
		
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
		if (!stopped)
		{
			stopped = true;
			adsr.noteOff();
		}
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

			if (sourceSamplePosition > playingSound->endSample)
			{
				stopNote(0.0f, false);
			}
			else if (sourceSamplePosition >= playingSound->endSample - adsr.getParameters().release * playingSound->sourceSampleRate)
			{
				stopNote(0.0f, true);
			}
		}
	}
}

