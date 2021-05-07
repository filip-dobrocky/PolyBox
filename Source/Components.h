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

        double getValueFromText(const String& text) override
        {
            return text.isNotEmpty() && text.containsOnly("0123456789.,-")
                 ? Slider::getValueFromText(text)
                 : -1;
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
        l.setBounds(bounds.removeFromTop(l.getFont().getHeight() + 1));
        s.setBounds(bounds);
    }

    CustomSlider s;

private:
    Label l;
};

class FloatSlider : public Component
{
public:
    FloatSlider(String parameter, Slider::SliderStyle style, bool centered, double min, double max)
    {
        addAndMakeVisible(s);
        addAndMakeVisible(l);
        s.setSliderStyle(style);
        s.setRange(min, max);
        s.setNumDecimalPlacesToDisplay(2);
        l.setText(parameter, juce::dontSendNotification);
        l.setJustificationType(centered ? Justification::centred
                                        : Justification::centredLeft);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        l.setBounds(bounds.removeFromTop(l.getFont().getHeight() + 1));
        s.setBounds(bounds);
    }

    Slider s;
    Label l;
};

class FrequencySlider : public Component
{
    class CustomSlider : public Slider
    {
    public:
        CustomSlider()
        {
            setSliderStyle(Slider::LinearBar);
            setRange(24.5f, 1046.5f);
            setNumDecimalPlacesToDisplay(2);
            setSkewFactorFromMidPoint(262);
        }

        double snapValue(double attemptedValue, DragMode dragMode) override
        {
            if (dragMode == notDragging || fine)
                return attemptedValue;

            auto f = attemptedValue / cFreq;
            int note = 0;

            while (f / halfStep >= 1)
            {
                f /= halfStep;
                note++;
            }

            return 440 * std::pow(2, (note - 57) / 12.0f);
        }

        String 	getTextFromValue(double value) override
        {
            int note = 0;
            auto f = value / cFreq;
            
            while (f / halfStep >= 1)
            {
                f /= halfStep;
                note++;
            }

            return notes[note % 12] + String(note / 12) + " (" + String(value) + " Hz - note " + String(note + 12) + ")";
        }

        double getValueFromText(const String& text) override
        {
            for (int i = 11; i >= 0; i--)
            {
                auto octave = text.fromFirstOccurrenceOf(notes[i], false, true);
                if (octave.isNotEmpty())
                {
                    int note = i + 12 * octave.getIntValue();
                    return 440 * std::pow(2, (note - 57) / 12.0f);
                }
            }

            return Slider::getValueFromText(text);
        }

        void mouseDown(const MouseEvent& event) override
        {
            Slider::mouseDown(event);
            fine = event.mods.isAltDown() || event.mods.isCtrlDown();
        }

    private:
        const double cFreq = 16.35f;
        const double halfStep = 1.05946f;
        const String notes[12]{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        bool fine = false;
    };

public:
    FrequencySlider()
    {
        addAndMakeVisible(s);
        addAndMakeVisible(l);
        l.setText("Root", juce::dontSendNotification);
        l.setJustificationType(Justification::centred);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        l.setBounds(bounds.removeFromTop(l.getFont().getHeight() + 1));
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

class ConnectionMatrix : public GroupComponent,
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
        setText("Sequencer routing");
        setTextLabelPosition(Justification::centred);

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
        voicesLabel.setJustificationType(Justification::centredBottom);
        channelsLabel.setText("CHANNELS", NotificationType::dontSendNotification);
        channelsLabel.setJustificationType(Justification::centredBottom);
        addAndMakeVisible(voicesLabel);
        addAndMakeVisible(channelsLabel);

    }

    void resized() override
    {
        Grid grid;
        using Track = Grid::TrackInfo;
        using Fr = Grid::Fr;
        grid.alignItems = Grid::AlignItems::center;
        grid.justifyItems = Grid::JustifyItems::center;
        grid.alignContent = Grid::AlignContent::center;
        grid.justifyContent = Grid::JustifyContent::center;

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

        auto bounds = getLocalBounds().reduced(15, 30);
        auto chBounds = bounds.removeFromTop(15);
        chBounds.removeFromLeft(30);
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

    void initialise(PolySequencer& s)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            for (int j = 0; j < NUM_VOICES; j++)
            {
                toggles[i][j]->setToggleState(s.voices[i]->hasChannel(j),
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
