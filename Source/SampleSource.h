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
#include "Constants.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState APVTS;

//==============================================================================
/*
*/
class SampleSource : public Component,
    public FileDragAndDropTarget,
    public ChangeListener
{
public:
    SampleSource(PolyBoxAudioProcessor& p, int ch) : audioThumbnailCache(5),
        audioThumbnail(512, formatManager, audioThumbnailCache),
        midiChannel(ch),
        sampler(p.sampler),
        parameters(p.parameters),
		processor(p)
    {
        formatManager.registerBasicFormats();
        audioThumbnail.addChangeListener(this);
        addChildComponent(panKnob);
        addChildComponent(gainKnob);
        addChildComponent(attackKnob);
        addChildComponent(releaseKnob);

        auto font = Font(10);
        panKnob.l.setFont(font);
        gainKnob.l.setFont(font);
        attackKnob.l.setFont(font);
        releaseKnob.l.setFont(font);
        panKnob.s.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        gainKnob.s.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        attackKnob.s.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        releaseKnob.s.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

		panAttachment
            .reset(new SliderAttachment(parameters, "s" + String(midiChannel) + "Pan", panKnob.s));
		gainAttachment
            .reset(new SliderAttachment(parameters, "s" + String(midiChannel) + "Gain", gainKnob.s));
		attackAttachment
            .reset(new SliderAttachment(parameters, "s" + String(midiChannel) + "Attack", attackKnob.s));
		releaseAttachment
            .reset(new SliderAttachment(parameters, "s" + String(midiChannel) + "Release", releaseKnob.s));

        for (auto c : getChildren())
            c->addMouseListener(this, true);

        for (int i = 0; i < sampler.getNumSounds(); i++)
        {
            if (sampler.getSound(i))
            {
				auto sound = dynamic_cast<MicroSamplerSound*>(sampler.getSound(i).get());
                if (sound->midiChannel == midiChannel)
                {
					this->sound = sound;
                }
            }
        }

        if (sound)
        {
            DBG("rerendering");
            rerenderThumbnail();
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
            DBG("drawing wave");
            auto bounds = getLocalBounds();
            g.setColour(Colour(0xffBE8723));
            const auto start = sound->getStart();
            const auto end = sound->getEnd();
            const auto length = audioThumbnail.getTotalLength();
            
            g.setOpacity(0.4f);
            if (start > 0.0f)
            {
                audioThumbnail.drawChannel(g, bounds.removeFromLeft(getWidth() * start), 0, start * length, 0, 1.0f);
            }
            if (end < 1.0f)
            {
                audioThumbnail.drawChannel(g, bounds.removeFromRight(getWidth() * (1 - end)), end * length, length, 0, 1.0f);
            }

            g.setOpacity(0.6f);
            audioThumbnail.drawChannel(g, bounds, start * length, end * length, 0, 1.0f);
        }
        else
        {
            g.drawText("Sample " + String(midiChannel), getLocalBounds(), Justification::centred, true);
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

    void loadSample(File f)
    {
        if (auto reader = formatManager.createReaderFor(f))
        {
			sound = processor.loadSample(reader, f.getFullPathName(), midiChannel);

            audioThumbnail.setSource(new FileInputSource(f));

            gainKnob.s.setValue(Decibels::gainToDecibels(sound->gain));
            panKnob.s.setValue(sound->pan);
            attackKnob.s.setValue(sound->getAttack());
            releaseKnob.s.setValue(sound->getRelease());

            repaint();

            callSampleSelectedListeners();
        }
    }

	void rerenderThumbnail()
	{
		DBG("rerendering thumbnail " + sound->getSourcePath());
		audioThumbnail.setSource(new FileInputSource(File(sound->getSourcePath())));
		repaint();
	}

    const int midiChannel;
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

	PolyBoxAudioProcessor& processor;
    Synthesiser& sampler;
    AudioFormatManager formatManager;
    AudioThumbnailCache audioThumbnailCache;
    AudioThumbnail audioThumbnail;
    FloatSlider panKnob{ "PAN", Slider::RotaryHorizontalVerticalDrag, true };
    FloatSlider gainKnob{ "GAIN", Slider::RotaryHorizontalVerticalDrag, true };
    FloatSlider attackKnob{ "ATTACK", Slider::RotaryHorizontalVerticalDrag, true };
    FloatSlider releaseKnob{ "RELEASE", Slider::RotaryHorizontalVerticalDrag, true };

	APVTS& parameters;
	std::unique_ptr<SliderAttachment> panAttachment;
	std::unique_ptr<SliderAttachment> gainAttachment;
	std::unique_ptr<SliderAttachment> attackAttachment;
	std::unique_ptr<SliderAttachment> releaseAttachment;

    bool drag{ false };

    ListenerList<Listener> listeners;
    void callSampleSelectedListeners()
    {
        Component::BailOutChecker checker(this);
        listeners.callChecked(checker, [this](Listener& l) { l.sampleSelected(this); });
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleSource)
};
