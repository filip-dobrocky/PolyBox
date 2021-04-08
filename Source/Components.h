#pragma once
#include <JuceHeader.h>

class NoteSlider : public Component
{
    class CustomSlider : public Slider
    {
    public:
        CustomSlider()
        {
            setSliderStyle(Slider::LinearBar);
            setRange(-1, 127, 1);
        }

        String 	getTextFromValue(double value) override
        {
            if (value == -1)
                return "OFF";
            else
                return String(value);
        }
    };

public:
    NoteSlider()
    {
        addAndMakeVisible(s);
        addAndMakeVisible(l);
        l.setText("Note", juce::dontSendNotification);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        l.setBounds(bounds.removeFromTop(20));
        s.setBounds(bounds);
    }

    CustomSlider s;

private:
    Label l;
};

class FloatSlider : public Component
{
    class CustomSlider : public Slider
    {
    public:
        CustomSlider()
        {
            setSliderStyle(Slider::LinearBar);
            setRange(0.0f, 1.0f);
            setNumDecimalPlacesToDisplay(2);
        }
    };

public:
    FloatSlider(String parameter)
    {
        addAndMakeVisible(s);
        addAndMakeVisible(l);
        l.setText(parameter, juce::dontSendNotification);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        l.setBounds(bounds.removeFromTop(20));
        s.setBounds(bounds);
    }

    CustomSlider s;

private:
    Label l;
};

class DurationSlider : public Slider
{
public:
    DurationSlider()
    {
        setSliderStyle(Slider::LinearBar);
        setRange(1, 10, 1);
    }

    String 	getTextFromValue(double value) override
    {
        return String(value) + (value == 1 ? " bar" : " bars");
    }
};
