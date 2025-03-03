/*
  ==============================================================================

    Tabs.h
    Created: 10 May 2021 10:57:43am
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Components.h"

//==============================================================================
/*
*/
typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class Tabs  : public TabbedComponent
{
public:
    Tabs(AudioProcessorValueTreeState& apvts) : TabbedComponent(TabbedButtonBar::TabsAtTop),
        valueTreeState(apvts)
    {
        addAndMakeVisible(masterSlider);
        masterSlider.setRange(-96, 12);
        masterSlider.setSkewFactor(4);
        masterSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);

		masterAttachment.reset(new SliderAttachment(valueTreeState, "masterLevel", masterSlider));
    }

    ~Tabs() override
    {
    }

    void resized() override
    {
        TabbedComponent::resized();
        auto x = 0.8f * getWidth();
        masterSlider.setBounds(x, 5, getWidth() - x - 10, 20);
    }

private:
	AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr<SliderAttachment> masterAttachment;
    DecibelSlider masterSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tabs)
};
