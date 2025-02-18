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
        sampler(s)
    {
        formatManager.registerBasicFormats();
        audioThumbnail.addChangeListener(this);
        addChildComponent(panKnob);
        addChildComponent(gainKnob);
        addChildComponent(attackKnob);
        addChildComponent(releaseKnob);

        for (int i = 0; i < sampler.getNumSounds(); i++)
        {
            if (auto s = dynamic_cast<MicroSamplerSound*> (sampler.getSound(i).get()))
            {
                if (s->channel == channel)
                    this->sound = s;
            }
        }

        auto font = Font(10);
        panKnob.l.setFont(font);
        gainKnob.l.setFont(font);
        attackKnob.l.setFont(font);
        releaseKnob.l.setFont(font);
        panKnob.s.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        gainKnob.s.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        attackKnob.s.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        releaseKnob.s.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        panKnob.s.setDoubleClickReturnValue(true, 0.0f);
        gainKnob.s.setDoubleClickReturnValue(true, 1.0f);
        attackKnob.s.setDoubleClickReturnValue(true, 0.01f);
        releaseKnob.s.setDoubleClickReturnValue(true, 0.01f);
        panKnob.s.onValueChange = [&] { updatePan(); };
        gainKnob.s.onValueChange = [&] { updateGain(); };
        attackKnob.s.onValueChange = [&] { updateAttack(); };
        releaseKnob.s.onValueChange = [&] { updateRelease(); };

        for (auto c : getChildren())
            c->addMouseListener(this, true);

        if (sound)
        {
            audioThumbnail.setSource(new FileInputSource(File(sound->sourcePath)));
            attackKnob.s.setValue(sound->getAttack());
            releaseKnob.s.setValue(sound->getRelease());
            panKnob.s.setValue(sound->pan);
            gainKnob.s.setValue(sound->gain);
        }
    }

    ~SampleSource() override
    {
    }

    void paint(Graphics& g) override
    {
        if (selected)
        {
            g.setColour(Colours::white);
            g.setOpacity(0.1);
            g.fillAll();   
        }

        panKnob.setVisible(sound);
        gainKnob.setVisible(sound);
        attackKnob.setVisible(sound);
        releaseKnob.setVisible(sound);

        g.setColour(Colours::white);
        g.setFont(14.0f);

        if (drag)
        {
            g.drawText("Drop sample", getLocalBounds(), Justification::centred, true);
        }
        else if (sound)
        {
            auto bounds = getLocalBounds();
            g.setColour(Colour(0xffBE8723));
            const auto start = sound->getStart();
            const auto end = sound->getEnd();
            const auto length = audioThumbnail.getTotalLength();
            
            g.setOpacity(0.4);
            if (start)
            {
                audioThumbnail.drawChannel(g, bounds.removeFromLeft(getWidth() * start), 0, start * length, 0, 1.0f);
            }
            if (end < 1)
            {
                audioThumbnail.drawChannel(g, bounds.removeFromRight(getWidth() * (1 - end)), end * length, length, 0, 1.0f);
            }

            g.setOpacity(0.6);
            audioThumbnail.drawChannel(g, bounds, start * length, end * length, 0, 1.0f);
        }
        else
        {
            g.drawText("Sample " + String(channel), getLocalBounds(), Justification::centred, true);
        }
    }

    void resized() override
    {
        FlexBox fb;
        fb.items.add(FlexItem(attackKnob).withFlex(1));
        fb.items.add(FlexItem(releaseKnob).withFlex(1));
        fb.items.add(FlexItem(panKnob).withFlex(1));
        fb.items.add(FlexItem(gainKnob).withFlex(1));
        fb.performLayout(getLocalBounds());
    }

    bool isInterestedInFileDrag(const StringArray& files) override
    {
        for (auto f : files)
        {
            f = f.toLowerCase();
            if (f.contains(".wav") || f.contains(".mp3") || f.contains(".aif") || f.contains(".flac"))
                return true;
        }

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
    MicroSamplerSound* sound{ nullptr };
    bool selected = false;

    class JUCE_API  Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void sampleSelected(SampleSource* sample) = 0;
    };

    void addListener(Listener* l) { listeners.add(l); }

    void removeListener(Listener* l) { listeners.remove(l); }


private:
    std::unique_ptr<FileChooser> chooser;

    void chooseFile()
    {
        chooser = std::make_unique<FileChooser> ( "Load sample", File::getSpecialLocation(File::userDesktopDirectory), "*.wav; *.mp3; *.aif; *.flac" );
		auto chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
        
		chooser->launchAsync(chooserFlags, [this](const FileChooser& fc)
			{
				if (fc.getResult().exists())
				{
					loadSample(fc.getResult());
				}
			});
    }

    void loadSample(File f)
    {
        if (auto reader = formatManager.createReaderFor(f))
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

            sound = dynamic_cast<MicroSamplerSound*>(sampler.addSound(new MicroSamplerSound(name, reader, f.getFullPathName(), channel, range,
                                                     261.63, 0.01, 0.01)));

            audioThumbnail.setSource(new FileInputSource(f));
            panKnob.s.setValue(sound->pan);
            gainKnob.s.setValue(sound->gain);
            attackKnob.s.setValue(sound->getAttack());
            releaseKnob.s.setValue(sound->getRelease());

            repaint();

            callSampleSelectedListeners();
        }
    }

    void updatePan()
    {
        if (sound)
        {
            sound->pan = panKnob.s.getValue();
        }
    }

    void updateGain()
    {
        if (sound)
        {
            sound->gain = gainKnob.s.getValue();
        }
    }

    void updateAttack()
    {
        if (sound)
        {
            sound->setAttack(attackKnob.s.getValue());
        }
    }

    void updateRelease()
    {
        if (sound)
        {
            sound->setRelease(releaseKnob.s.getValue());
        }
    }

    AudioFormatManager formatManager;
    AudioThumbnailCache audioThumbnailCache;
    AudioThumbnail audioThumbnail;
    Synthesiser& sampler;
    FloatSlider panKnob{ "PAN", Slider::RotaryHorizontalVerticalDrag, true, -1.0f, 1.0f };
    FloatSlider gainKnob{ "GAIN", Slider::RotaryHorizontalVerticalDrag, true, 0.0f, 2.0f };
    FloatSlider attackKnob{ "ATTACK", Slider::RotaryHorizontalVerticalDrag, true, 0.01f, 1.0f };
    FloatSlider releaseKnob{ "RELEASE", Slider::RotaryHorizontalVerticalDrag, true, 0.01f, 1.0f };

    bool drag{ false };

    ListenerList<Listener> listeners;
    void callSampleSelectedListeners()
    {
        Component::BailOutChecker checker(this);
        listeners.callChecked(checker, [this](Listener& l) { l.sampleSelected(this); });
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleSource)
};
