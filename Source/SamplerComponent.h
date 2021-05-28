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

//==============================================================================
/*
*/
class SamplerComponent  : public Component,
                          public SampleSource::Listener
{
public:
    SamplerComponent(Synthesiser &s)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            auto sample = new SampleSource(s, i + 1);
            sample->addListener(this);
            addAndMakeVisible(samples.add(sample));
        }

        timeSlider.setRange(0.0f, 1.0f);
        setSlidersEnabled(false);

        frequencySlider.s.onValueChange = [&] { frequencyChanged(); };
        timeSlider.onValueChange = [&] { timeChanged(); };
        reverseButton.onClick = [&] { reverseClicked(); };

        addAndMakeVisible(timeSlider);
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
        timeSlider.setBounds(bounds.removeFromBottom(25).reduced(5));
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
            setSliderValues(sample);
            setSlidersEnabled(true);
        }
        else
        {
            setSlidersEnabled(false);
        }

        selectedSample = sample;
    }

private:
    OwnedArray<SampleSource> samples;
    SampleSource* selectedSample{ nullptr };

    Slider timeSlider{ Slider::SliderStyle::TwoValueHorizontal, Slider::NoTextBox };
    FrequencySlider frequencySlider;
    ToggleButton reverseButton{ "Reverse sample" };

    void setSlidersEnabled(bool enabled)
    {
        timeSlider.setEnabled(enabled);
        frequencySlider.setEnabled(enabled);
        reverseButton.setEnabled(enabled);
    }

    void setSliderValues(SampleSource* sample)
    {
        timeSlider.setMinAndMaxValues(sample->sound->getStart(), sample->sound->getEnd(), NotificationType::dontSendNotification);
        frequencySlider.s.setValue(sample->sound->getRoot());
        reverseButton.setToggleState(sample->sound->reversed, NotificationType::dontSendNotification);
    }

    void frequencyChanged()
    {
        selectedSample->sound->setRoot(frequencySlider.s.getValue());
    }

    void timeChanged()
    {
        selectedSample->sound->setStart(timeSlider.getMinValue());
        selectedSample->sound->setEnd(timeSlider.getMaxValue());
        selectedSample->repaint();
    }

    void reverseClicked()
    {
        if (selectedSample->sound)
        {
            selectedSample->sound->reverse();
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerComponent)
};
