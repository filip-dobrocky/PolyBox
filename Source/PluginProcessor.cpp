/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace Tunings;

//==============================================================================
PolyBoxAudioProcessor::PolyBoxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    tuning(std::make_shared<Tuning>()),
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    scl = tuning->scale;
    kbm = tuning->keyboardMapping;

    for (int i = 0; i < NUM_VOICES * NUM_VOICES * 2; i++)
        sampler.addVoice(new MicroSamplerVoice(tuning));

	formatManager.registerBasicFormats();
}

PolyBoxAudioProcessor::~PolyBoxAudioProcessor()
{
}

//==============================================================================

AudioProcessorValueTreeState::ParameterLayout PolyBoxAudioProcessor::createParameterLayout()
{
	AudioProcessorValueTreeState::ParameterLayout layout;

    auto masterRange = NormalisableRange<float>(-96.0f, 12.0f);
    masterRange.setSkewForCentre(-12.0f);
    auto gainRange = NormalisableRange<float>(-96.0f, 12.0f);
    gainRange.setSkewForCentre(0.0f);
	auto tempoRange = NormalisableRange<float>(MIN_TEMPO, MAX_TEMPO, 1.0f);
	tempoRange.setSkewForCentre(160.0f);

    layout.add(std::make_unique<AudioParameterFloat>("masterLevel", "Master Level", masterRange, 0.0f));
	layout.add(std::make_unique<AudioParameterFloat>("tempo", "Tempo", tempoRange, 120.0f));
	layout.add(std::make_unique<AudioParameterInt>("duration", "Pattern Duration", MIN_BARS, MAX_BARS, 1));
    layout.add(std::make_unique<AudioParameterBool>("syncOn", "Sync On", false));
    layout.add(std::make_unique<AudioParameterBool>("transposeOn", "Transpose On", false));

    // Add parameters for each voice
    for (int i = 1; i <= NUM_VOICES; i++)
    {
        layout.add(std::make_unique<AudioParameterFloat>(
            "s" + String(i) + "Pan", "Sound " + String(i) + " Pan",
            NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
        layout.add(std::make_unique<AudioParameterFloat>(
            "s" + String(i) + "Gain", "Sound " + String(i) + " Gain",
            gainRange, 0.0f));
        layout.add(std::make_unique<AudioParameterFloat>(
            "s" + String(i) + "Attack", "Sound " + String(i) + " Attack",
            NormalisableRange<float>(MIN_ATTACK, MAX_ATTACK), DEFAULT_ATTACK));
        layout.add(std::make_unique<AudioParameterFloat>(
            "s" + String(i) + "Release", "Sound " + String(i) + " Release",
            NormalisableRange<float>(MIN_RELEASE, MAX_RELEASE), DEFAULT_RELEASE));
		layout.add(std::make_unique<AudioParameterFloat>(
			"s" + String(i) + "Root", "Sound " + String(i) + " Root Frequency",
			NormalisableRange<float>(MIN_ROOT_F, MAX_ROOT_F), DEFAULT_ROOT_F));
        layout.add(std::make_unique<AudioParameterFloat>(
			"s" + String(i) + "Start", "Sound " + String(i) + " Start",
			NormalisableRange<float>(0.0f, 1.0f), 0.0f));
		layout.add(std::make_unique<AudioParameterFloat>(
			"s" + String(i) + "End", "Sound " + String(i) + " End",
			NormalisableRange<float>(0.0f, 1.0f), 1.0f));
		layout.add(std::make_unique<AudioParameterBool>(
			"s" + String(i) + "Reversed", "Sound " + String(i) + " Reversed", false));
    }

	return layout;
}

const juce::String PolyBoxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolyBoxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PolyBoxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PolyBoxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PolyBoxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolyBoxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolyBoxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolyBoxAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PolyBoxAudioProcessor::getProgramName (int index)
{
    return {};
}

bool PolyBoxAudioProcessor::canSync()
{
    return getPlayHead();
}

MicroSamplerSound* PolyBoxAudioProcessor::loadSample(AudioFormatReader* source, String path, int midiChannel)
{
	MicroSamplerSound* sound = nullptr;
    
    for (int i = 0; i < sampler.getNumSounds(); i++)
    {
        if (sampler.getSound(i))
        {
            if (dynamic_cast<MicroSamplerSound*>(sampler.getSound(i).get())->midiChannel == midiChannel)
            {
                sampler.removeSound(i);
            }
        }
    }
    const String name = "s" + midiChannel;

    BigInteger range;
    range.setRange(0, 128, true);

    sound = dynamic_cast<MicroSamplerSound*>(sampler.addSound(
        new MicroSamplerSound(name, source, path, midiChannel, range,
            DEFAULT_ROOT_F, DEFAULT_ATTACK, DEFAULT_RELEASE)));
	
    auto paths = parameters.state.getOrCreateChildWithName("paths", nullptr);
    paths.setProperty("s" + String(midiChannel) + "Path", path, nullptr);

    return sound;
}

bool PolyBoxAudioProcessor::loadScl(String path)
{
    auto file = File(path);
    bool success = false;
    if (file.exists())
    {
        try
        {
            scl = readSCLFile(path.toStdString());
            *tuning = Tuning(scl, kbm);
            sclPath = path;
            success = true;
        } catch (...) { }
    }
    return success;
}

bool PolyBoxAudioProcessor::loadKbm(String path)
{
    auto file = File(path);
    bool success = false;
    if (file.exists())
    {
        try
        {
            kbm = readKBMFile(path.toStdString());
            *tuning = Tuning(scl, kbm);
            kbmPath = path;
            success = true;
        }
        catch (...) {}
    }
    return success;
}

void PolyBoxAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PolyBoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampler.setCurrentPlaybackSampleRate(sampleRate);
    sequencer.setSampleRate(sampleRate);
    previousLevel = parameters.getRawParameterValue("masterLevel")->load();
}

void PolyBoxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PolyBoxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PolyBoxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto syncOn = parameters.getRawParameterValue("syncOn")->load();
    auto transposeOn = parameters.getRawParameterValue("transposeOn")->load();
    auto masterLevel = parameters.getRawParameterValue("masterLevel")->load();
	auto tempo = parameters.getRawParameterValue("tempo")->load();
	auto duration = parameters.getRawParameterValue("duration")->load();

	sequencer.setDuration(duration);

	for (int i = 1; i <= NUM_VOICES; i++)
	{	
        MicroSamplerSound* sound = nullptr;
		for (int j = 0; j < sampler.getNumSounds(); j++)
		{
			if (auto s = dynamic_cast<MicroSamplerSound*>(sampler.getSound(j).get()))
			{
				if (s->midiChannel == i)
				{
					sound = s;
					break;
				}
			}
		}

		if (sound)
		{
            auto pan = parameters.getRawParameterValue("s" + String(i) + "Pan")->load();
            auto gain = parameters.getRawParameterValue("s" + String(i) + "Gain")->load();
            auto attack = parameters.getRawParameterValue("s" + String(i) + "Attack")->load();
            auto release = parameters.getRawParameterValue("s" + String(i) + "Release")->load();
            auto root = parameters.getRawParameterValue("s" + String(i) + "Root")->load();
            auto start = parameters.getRawParameterValue("s" + String(i) + "Start")->load();
            auto end = parameters.getRawParameterValue("s" + String(i) + "End")->load();
            auto reversed = parameters.getRawParameterValue("s" + String(i) + "Reversed")->load();

			sound->pan = pan;
			sound->gain = Decibels::decibelsToGain(gain);
			sound->setAttack(attack);
			sound->setRelease(release);
			sound->setRoot(root);
			sound->setStart(start);
			sound->setEnd(end);
			sound->setReversed(reversed);
		}
	}

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto m : midiMessages)
    {
        auto message = m.getMessage();
        if (message.isNoteOn())
        {
            playedNote = message.getNoteNumber();
            DBG("Note: " + String(playedNote));
            break;
        }
        if (message.isNoteOff() && message.getNoteNumber() == playedNote)
        {
            playedNote = -1;
            DBG("Note: " + String(playedNote));
        }
    }

    midiMessages.clear();

    if (transposeOn && playedNote != -1)
    {
        sequencer.transpose(60, playedNote);
    }
    else
    {
        sequencer.transposeOff();
    }

    if (syncOn)
    {
        if (auto ph = getPlayHead())
        {
            AudioPlayHead::CurrentPositionInfo info;
            if (ph->getCurrentPosition(info))
            {
                sequencer.setTempo(info.bpm);
                sequencer.setTimeSignature(info.timeSigNumerator, info.timeSigDenominator);
            }
        }
	}
	else
	{
		sequencer.setTempo(tempo);
	}

    //Sequencer Control
    auto interval = sequencer.getIntervalInSamples();
    if (clockInterval != interval)
    {
        DBG("INTERVAL " + String(interval));
        clockInterval = interval;
    }

    if (sequencer.isPlaying())
    {
        stopped = false;
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {

            if (sampleCounter++ == 0)
                sequencer.tick(i + 1);
            if (sampleCounter >= clockInterval)
                sampleCounter = 0;

        }
    }

    midiMessages.swapWith(sequencer.midiMessages);
    sequencer.midiMessages.clear();

    // turn all notes off on stop
    if (!stopped && !sequencer.isPlaying())
    {
        for (int i = 1; i <= NUM_VOICES; i++)
            for (int j = 0; j < 128; j++)
                midiMessages.addEvent(MidiMessage::noteOff(i, j), 1);
        sampleCounter = 0;
        stopped = true;
    }

    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

	if (approximatelyEqual(masterLevel, previousLevel))
	{
		buffer.applyGain(Decibels::decibelsToGain(masterLevel));
	}
    else
    {
		buffer.applyGainRamp(0, buffer.getNumSamples(),
            Decibels::decibelsToGain(previousLevel), Decibels::decibelsToGain(masterLevel));
		previousLevel = masterLevel;
    }
}

//==============================================================================
bool PolyBoxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolyBoxAudioProcessor::createEditor()
{
    return new PolyBoxAudioProcessorEditor (*this);
}

//==============================================================================
void PolyBoxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();

    auto tuningProp = state.getOrCreateChildWithName("tuning", nullptr);
    tuningProp.setProperty("equalTuning", equalTuning, nullptr);
    tuningProp.setProperty("equalTuningSpan", equalTuningSpan, nullptr);
    tuningProp.setProperty("equalTuningDivision", equalTuningDivision, nullptr);
    tuningProp.setProperty("sclPath", sclPath, nullptr);
    tuningProp.setProperty("kbmPath", kbmPath, nullptr);

    for (int i = 0; i < NUM_VOICES; i++)
    {
        auto voice = sequencer.voices[i];
        auto length = voice->getLength();
        auto voiceProp = state.getOrCreateChildWithName("seqV" + String(i), nullptr);
        voiceProp.setProperty("length", length, nullptr);
        
        String channels = "";
        for (int j = 0; j < NUM_VOICES; j++)
            channels.append(voice->hasChannel(j) ? "1" : "0", 1);
        voiceProp.setProperty("channels", channels, nullptr);
        
        for (int j = 0; j < length; j++) {
            auto note = voice->getNotePtr(j);
            auto noteProp = voiceProp.getOrCreateChildWithName("note" + String(j), nullptr);
            noteProp.setProperty("num", note->number, nullptr);
            noteProp.setProperty("vel", note->velocity, nullptr);
            noteProp.setProperty("prob", note->probability, nullptr);
        }
    }

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolyBoxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }

    auto tuningProp = parameters.state.getOrCreateChildWithName("tuning", nullptr);
    equalTuning = (bool)tuningProp.getProperty("equalTuning", false);
    equalTuningSpan = (int)tuningProp.getProperty("equalTuningSpan", 0);
    equalTuningDivision = (int)tuningProp.getProperty("equalTuningDivision", 0);
    sclPath = tuningProp.getProperty("sclPath", String()).toString();
    kbmPath = tuningProp.getProperty("kbmPath", String()).toString();

    if (equalTuning)
    {
        if (equalTuningSpan > 0 && equalTuningDivision > 0)
            *tuning = Tuning(evenDivisionOfSpanByM(equalTuningSpan, equalTuningDivision));
        else
            *tuning = Tuning();
    }
    else if (sclPath.isEmpty() && kbmPath.isEmpty())
    {
        *tuning = Tuning();
    }
    else
    {
        if (sclPath.isNotEmpty())
            loadScl(sclPath);
        if (kbmPath.isNotEmpty())
            loadKbm(kbmPath);
        *tuning = Tuning(scl, kbm);
    }

    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (isSoundLoaded(i + 1)) {
            DBG("Sound already loaded");
            continue;
        }

        auto paths = parameters.state.getOrCreateChildWithName("paths", nullptr);
        auto path = paths.getProperty("s" + String(i + 1) + "Path", String()).toString();

        if (path.isNotEmpty())
        {
            auto file = File(path);
            if (file.exists())
            {
                DBG("Loading sample: " + file.getFullPathName());
                if (auto reader = formatManager.createReaderFor(file))
                {
                    loadSample(reader, file.getFullPathName(), i + 1);
                }
            }
        }

        auto voiceProp = parameters.state.getOrCreateChildWithName("seqV" + String(i), nullptr);
        auto length = (int)voiceProp.getProperty("length", DEFAULT_STEPS);
        auto voice = sequencer.voices[i];
        auto channels = voiceProp.getProperty("channels", String()).toString();
        if (channels.isNotEmpty()) {
            for (int j = 0; j < NUM_VOICES; j++) {
                if (channels[j] == '1')
                    voice->assignChannel(j);
                else
                    voice->deassignChannel(j);
            }
        }

        voice->setLength(length);

        for (int j = 0; j < length; j++)
        {
            auto noteProp = voiceProp.getOrCreateChildWithName("note" + String(j), nullptr);
            auto num = (int)noteProp.getProperty("num", -1);
            auto vel = (float)noteProp.getProperty("vel", 0.5f);
            auto prob = (float)noteProp.getProperty("prob", 1.0f);
            auto note = voice->getNotePtr(j);
            note->number = num;
            note->velocity = vel;
            note->probability = prob;
            DBG("note loaded " + String(num) + " " + String(vel) + " " + String(prob));
        }
    }
}

bool PolyBoxAudioProcessor::isSoundLoaded(int midiChannel)
{
    for (int j = 0; j < sampler.getNumSounds(); j++)
    {
        if (auto sound = dynamic_cast<MicroSamplerSound*>(sampler.getSound(j).get()))
        {
            if (sound->midiChannel == midiChannel)
            {
                return true;
            }
        }
    }
    return false;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyBoxAudioProcessor();
}
