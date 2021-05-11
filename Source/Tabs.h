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
class Tabs  : public TabbedComponent
{
public:
    Tabs(float& masterLevel) : TabbedComponent(TabbedButtonBar::TabsAtTop),
                                level(masterLevel)
    {
        addAndMakeVisible(masterSlider);
        masterSlider.setRange(-100, 0);
        masterSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
        masterSlider.setValue(Decibels::gainToDecibels(level));
        masterSlider.onValueChange = [&] { level = Decibels::decibelsToGain((float)masterSlider.getValue()); };
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

    float& level;

private:
    DecibelSlider masterSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tabs)
};
