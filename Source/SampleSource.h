/*
  ==============================================================================

    SampleSource.h
    Created: 13 Apr 2021 2:02:31pm
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MicroSampler.h"

//==============================================================================
/*
*/
class SampleSource : public Component,
    public FileDragAndDropTarget,
    public ChangeListener
{
public:
    SampleSource(Synthesiser& s, int ch) : audioThumbnailCache(5),
        audioThumbnail(512, formatManager, audioThumbnailCache),
        channel(ch),
        sampler(s),
        attackTime(0.1),
        releaseTime(0.3),
        sampleLength(10),
        rootFrequency(262)
    {
        formatManager.registerBasicFormats();
        audioThumbnail.addChangeListener(this);
    }

    ~SampleSource() override
    {
    }

    void paint(Graphics& g) override
    {
        if (selected)
        {
            g.setColour(juce::Colours::white);
            g.setOpacity(0.1);
            g.fillAll();
        }

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        if (drag)
        {
            g.drawText("Drop sample", getLocalBounds(),
                juce::Justification::centred, true);
        }
        else if (sound)
        {
            g.setColour(juce::Colours::aquamarine);
            audioThumbnail.drawChannel(g, getLocalBounds(), 0, audioThumbnail.getTotalLength(), 0, 1.0f);
        }
        else
        {
            g.drawText("Sample " + String(channel), getLocalBounds(),
                juce::Justification::centred, true);
        }
    }

    void resized() override
    {

    }

    bool isInterestedInFileDrag(const StringArray& files) override
    {
        for (auto f : files)
            if (f.contains(".wav") || f.contains(".mp3") || f.contains(".aif"))
                return true;

        return false;
    }

    void fileDragEnter(const StringArray& files, int x, int y) override
    {
        if (isInterestedInFileDrag(files))
        {
            drag = true;
            repaint();
        }
    }

    void fileDragExit(const StringArray& files) override
    {
        drag = false;
        repaint();
    }

    void filesDropped(const StringArray& files, int x, int y) override
    {
        for (auto f : files)
        {
            if (isInterestedInFileDrag(f))
            {
                loadSample(File(f));
                break;
            }
        }
        drag = false;
    }

    void mouseDown(const MouseEvent& event) override
    {
        if (event.mouseWasClicked() && event.mods.isLeftButtonDown())
            callSampleSelectedListeners();
    }

    void mouseDoubleClick(const MouseEvent& event) override
    {
        if (event.mods.isLeftButtonDown())
            chooseFile();
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &audioThumbnail)
            repaint();
    }

    const int channel;
    double attackTime;
    double releaseTime;
    double sampleLength;
    double rootFrequency;
    bool selected{ false };

    class JUCE_API  Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void sampleSelected(SampleSource* sample) = 0;
    };

    void addListener(Listener* l) { listeners.add(l); }

    void removeListener(Listener* l) { listeners.remove(l); }

private:
    void chooseFile()
    {
        FileChooser chooser{ "Load sample", File::getSpecialLocation(File::userDesktopDirectory), "*.wav; *.mp3; *.aif" };
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            loadSample(file);
        }
    }

    void loadSample(File f)
    {
        const auto name = "s" + channel;
        for (int i = 0; i < sampler.getNumSounds(); i++)
        {
            if (sampler.getSound(i) == sound)
            {
                sampler.removeSound(i);
            }
        }

        BigInteger range;
        range.setRange(0, 128, true);
        auto reader = formatManager.createReaderFor(f);
        sound = sampler.addSound(new MicroSamplerSound(name, reader, channel, range, 
                                                       rootFrequency, attackTime, releaseTime, sampleLength));

        audioThumbnail.setSource(new FileInputSource(f));
        repaint();
    }

    AudioFormatManager formatManager;
    AudioThumbnailCache audioThumbnailCache;
    AudioThumbnail audioThumbnail;
    Synthesiser& sampler;
    SynthesiserSound* sound{ nullptr };

    bool drag{ false };

    ListenerList<Listener> listeners;
    void callSampleSelectedListeners()
    {
        Component::BailOutChecker checker(this);
        listeners.callChecked(checker, [this](Listener& l) { l.sampleSelected(this); });
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleSource)
};
