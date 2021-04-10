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

class ConnectionMatrix : public Component,
                         public Button::Listener
{
    struct Toggle : ToggleButton
    {
        Toggle(int i, int j) : voice(i), channel(j)
        {
            setClickingTogglesState(true);
        }

        int voice;
        int channel;
    };

public:
    ConnectionMatrix()
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            for (int j = 0; j < NUM_VOICES; j++)
            {
                auto toggle = new Toggle(i, j);
                toggle->addListener(this);
                addAndMakeVisible(toggles[i].add(toggle));
            }

            auto cL = new Label("columnLabel" + i, String(i + 1));
            auto rL = new Label("rowLabel" + i, String(i + 1));
            cL->setJustificationType(Justification::centred);
            rL->setJustificationType(Justification::centred);
            addAndMakeVisible(columnLabels.add(cL));
            addAndMakeVisible(rowLabels.add(rL));
        }

        voicesLabel.setText("VOICES", NotificationType::dontSendNotification);
        voicesLabel.setJustificationType(Justification::centred);
        channelsLabel.setText("CHANNELS", NotificationType::dontSendNotification);
        channelsLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(voicesLabel);
        addAndMakeVisible(channelsLabel);

    }

    void resized() override
    {
        Grid grid;
        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;

        for (int i = 0; i < NUM_VOICES + 1; i++)
        {
            grid.templateRows.add(Track(Fr(1)));
            grid.templateColumns.add(Track(Fr(1)));
        }

        grid.items.add(nullptr);
        for (auto l : columnLabels)
            grid.items.add(l);
        for (int i = 0; i < NUM_VOICES; i++)
        {
            for (int j = 0; j < NUM_VOICES + 1; j++)
            {
                grid.items.add((j == 0) ? GridItem(rowLabels[i]) : GridItem(toggles[i][j-1]));
            }
        }

        auto bounds = getLocalBounds();
        auto chBounds = bounds.removeFromTop(15);
        chBounds.removeFromLeft(20);
        channelsLabel.setBounds(chBounds);
        auto vBounds = bounds.removeFromLeft(15);
        auto transform = AffineTransform().rotated(-MathConstants<float>::halfPi,
            vBounds.getCentreX(),
            vBounds.getCentreY());
        voicesLabel.setTransform(transform);
        voicesLabel.setBounds(vBounds.transformed(transform));

        grid.performLayout(bounds);
    }

    void buttonClicked(Button* b) override
    {
        if (Toggle* t = dynamic_cast<Toggle*>(b)) {
            clickedToggle = t;
            callListeners();
        }
    }

    void initialise(PolySequencer* s)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            for (int j = 0; j < NUM_VOICES; j++)
            {
                toggles[i][j]->setToggleState(s->voices[i]->hasChannel(j),
                                              NotificationType::dontSendNotification);
            }
        }
    }

    class JUCE_API  Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void connectionChanged(int voice, int channel, bool state) = 0;
    };

    ListenerList<Listener> listeners;
    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }
    void callListeners()
    {
        Component::BailOutChecker checker(this);
        listeners.callChecked(checker, [this](Listener& l) { l.connectionChanged(clickedToggle->voice, clickedToggle->channel, clickedToggle->getToggleState()); });
    }

private:
    OwnedArray<Toggle> toggles[NUM_VOICES];
    OwnedArray<Label> rowLabels, columnLabels;
    Toggle* clickedToggle{ nullptr };
    Label voicesLabel, channelsLabel;
};
