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

        frequencySlider.setSliderStyle(Slider::SliderStyle::LinearBar);

        attackSlider.setRange(0.0f, 2.0f);
        releaseSlider.setRange(0.0f, 2.0f);
        timeSlider.setRange(0.0f, 1.0f);
        frequencySlider.setRange(20, 5000);
        frequencySlider.setTextValueSuffix(" Hz");
        setSlidersEnabled(false);

        attackSlider.onValueChange = [&] { attackChanged(); };
        releaseSlider.onValueChange = [&] { releaseChanged(); };
        frequencySlider.onValueChange = [&] { frequencyChanged(); };
        timeSlider.onValueChange = [&] { timeChanged(); };

        addAndMakeVisible(attackSlider);
        addAndMakeVisible(releaseSlider);
        addAndMakeVisible(timeSlider);
        addAndMakeVisible(frequencySlider);
    }

    ~SamplerComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        FlexBox samplesFb, controlFb;
        
        samplesFb.flexDirection = FlexBox::Direction::column;
        for (auto s : samples)
            samplesFb.items.add(FlexItem(*s).withFlex(1));
        
        controlFb.items.add(FlexItem(frequencySlider).withFlex(1));
        controlFb.items.add(FlexItem(attackSlider).withFlex(0.5));
        controlFb.items.add(FlexItem(releaseSlider).withFlex(0.5));

        auto bounds = getLocalBounds();
        timeSlider.setBounds(bounds.removeFromBottom(10));
        controlFb.performLayout(bounds.removeFromBottom(30));
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
            setSlidersEnabled(true);
            setSliderValues(sample);
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

    Slider attackSlider{ Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::NoTextBox};
    Slider releaseSlider{ Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::NoTextBox };
    Slider timeSlider{ Slider::SliderStyle::TwoValueHorizontal, Slider::NoTextBox};
    Slider frequencySlider;

    void setSlidersEnabled(bool enabled)
    {
        attackSlider.setEnabled(enabled);
        releaseSlider.setEnabled(enabled);
        timeSlider.setEnabled(enabled);
        frequencySlider.setEnabled(enabled);
    }

    void setSliderValues(SampleSource* sample)
    {
        attackSlider.setValue(sample->sound->getAttack());
        releaseSlider.setValue(sample->sound->getRelease());
        timeSlider.setMinValue(sample->sound->getStart());
        timeSlider.setMaxValue(sample->sound->getEnd());
        frequencySlider.setValue(sample->sound->getRoot());
    }

    void attackChanged()
    {
        selectedSample->sound->setAttack(attackSlider.getValue());
    }

    void releaseChanged()
    {
        selectedSample->sound->setRelease(releaseSlider.getValue());
    }

    void frequencyChanged()
    {
        selectedSample->sound->setRoot(frequencySlider.getValue());
    }

    void timeChanged()
    {
        selectedSample->sound->setStart(timeSlider.getMinValue());
        selectedSample->sound->setEnd(timeSlider.getMaxValue());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerComponent)
};
