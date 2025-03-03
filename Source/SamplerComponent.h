/*
  ==============================================================================

    SamplerComponent.h
    Created: 15 Apr 2021 10:22:21am
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SampleSource.h"
#include "PolySequencer.h"
#include "Components.h"
#include "Constants.h"

typedef juce::AudioProcessorValueTreeState APVTS;
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/*
*/
class SamplerComponent  : public Component,
                          public SampleSource::Listener
{
public:
	SamplerComponent(PolyBoxAudioProcessor& p) : apvts(p.parameters)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            auto sample = new SampleSource(p, i + 1);
            sample->addListener(this);
            addAndMakeVisible(samples.add(sample));
        }

        startEndSlider.setRange(0.0f, 1.0f);
        setComponentsEnabled(false);

        addAndMakeVisible(startEndSlider);
        addAndMakeVisible(frequencySlider);
        addAndMakeVisible(reverseButton);
    }

    ~SamplerComponent() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(findColour(ResizableWindow::backgroundColourId));
        g.fillAll();
    }

    void resized() override
    {
        FlexBox samplesFb;
        
        samplesFb.flexDirection = FlexBox::Direction::column;
        for (auto s : samples)
            samplesFb.items.add(FlexItem(*s).withFlex(1));
        
        auto bounds = getLocalBounds();
        frequencySlider.setBounds(bounds.removeFromBottom(55).reduced(5));
        startEndSlider.setBounds(bounds.removeFromBottom(25).reduced(5));
        reverseButton.setBounds(bounds.removeFromBottom(25).reduced(5));
        samplesFb.performLayout(bounds);
    }

    void sampleSelected(SampleSource* sample) override
    {
        if (selectedSample)
        {
            selectedSample->selected = false;
            selectedSample->repaint();
        }
        sample->selected = true;
        sample->repaint();

        if (sample->sound)
        {
			rootAttachment.reset();
			reverseAttachment.reset();
			startEndAttachment.reset();

            rootAttachment.reset(new SliderAttachment(
                apvts, "s" + String(sample->midiChannel) + "Root", frequencySlider.s));
            reverseAttachment.reset(new ButtonAttachment(
                apvts, "s" + String(sample->midiChannel) + "Reversed", reverseButton));
            startEndAttachment.reset(new TwoValueSliderAttachment(
				apvts, "s" + String(sample->midiChannel) + "Start", "s" + String(sample->midiChannel) + "End", startEndSlider));
                
			startEndAttachment->onValueChange = [&] { selectedSample->repaint(); };

            setComponentsEnabled(true);
        }
        else
        {
            rootAttachment.reset();
            reverseAttachment.reset();
            setComponentsEnabled(false);
        }

        selectedSample = sample;
    }

	void loadSample(String path, int midiChannel)
	{
		for (auto s : samples)
		{
			if (s->midiChannel == midiChannel)
			{
				File f(path);
				s->loadSample(f);
				break;
			}
		}
	}

private:
    OwnedArray<SampleSource> samples;
    SampleSource* selectedSample{ nullptr };

    Slider startEndSlider{ Slider::SliderStyle::TwoValueHorizontal, Slider::NoTextBox };
    FrequencySlider frequencySlider;
    ToggleButton reverseButton{ "Reverse sample" };

	APVTS& apvts;
	std::unique_ptr<SliderAttachment> rootAttachment;
	std::unique_ptr<ButtonAttachment> reverseAttachment;
	std::unique_ptr<TwoValueSliderAttachment> startEndAttachment;

    void setComponentsEnabled(bool enabled)
    {
        startEndSlider.setEnabled(enabled);
        frequencySlider.setEnabled(enabled);
        reverseButton.setEnabled(enabled);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerComponent)
};
