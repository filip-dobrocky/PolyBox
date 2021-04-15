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
    }

    ~SamplerComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {

    }

    void resized() override
    {
        FlexBox samplesFb;
        samplesFb.flexDirection = FlexBox::Direction::column;
        for (auto s : samples)
            samplesFb.items.add(FlexItem(*s).withFlex(1));
        samplesFb.performLayout(getLocalBounds());
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
        selectedSample = sample;
    }

private:
    OwnedArray<SampleSource> samples;
    SampleSource* selectedSample{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerComponent)
};
