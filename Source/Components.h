#pragma once
#include <JuceHeader.h>
#include "Constants.h"

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
    FloatSlider(String parameter, Slider::SliderStyle style, bool centered)
    {
        addAndMakeVisible(s);
        addAndMakeVisible(l);
		s.setRange(0.0f, 1.0f);
        s.setSliderStyle(style);
        s.setNumDecimalPlacesToDisplay(3);
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
            setNumDecimalPlacesToDisplay(2);
            setRange(MIN_ROOT_F, MAX_ROOT_F);
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
    }

    String 	getTextFromValue(double value) override
    {
        return String(value) + (value == 1 ? " bar" : " bars");
    }
};

class DecibelSlider : public Slider
{
public:
    DecibelSlider() {}

    double getValueFromText(const juce::String& text) override
    {
        auto minusInfinitydB = -96.0;

        auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();

        return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
            : decibelText.getDoubleValue();
    }

    juce::String getTextFromValue(double value) override
    {
        return juce::Decibels::toString(value);
    }
};

class PlayButton : public ImageButton
{
public:
    PlayButton()
    {
        setClickingTogglesState(true);
        auto hashCode = String("play_button").hashCode64();
        auto img = ImageCache::getFromHashCode (hashCode);

        if (img.isNull())
        {
            img = ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
            ImageCache::addImageToCache(img, hashCode);
        }

        const auto colour = Colour(0xff123b7b);

        setImages(false, true, true,
            img, 1.0f, colour,
            img, 0.4f, colour.brighter(0.1f),
            img, 1.0f, Colour(0xff5BD0C4));
    }

};

class RecordButton : public ImageButton
{
public:
    RecordButton()
    {
        setClickingTogglesState(true);
        auto hashCode = String("record_button").hashCode64();
        auto img = ImageCache::getFromHashCode(hashCode);

        if (img.isNull())
        {
            img = ImageFileFormat::loadFrom(BinaryData::record_png, BinaryData::record_pngSize);
            ImageCache::addImageToCache(img, hashCode);
        }

        const auto colour = Colour(0xff123b7b);

        setImages(false, true, true,
            img, 1.0f, colour,
            img, 1.0f, colour.brighter(0.1f),
            img, 1.0f, Colour(0xe0f40600));
    }

};

class ResetButton : public ImageButton
{
public:
    ResetButton()
    {
        auto hashCode = String("reset_button").hashCode64();
        auto img = ImageCache::getFromHashCode(hashCode);

        if (img.isNull())
        {
            img = ImageFileFormat::loadFrom(BinaryData::reset_png, BinaryData::reset_pngSize);
            ImageCache::addImageToCache(img, hashCode);
        }

        const auto colour = Colour(0xff123b7b);

        setImages(false, true, true,
            img, 1.0f, colour,
            img, 0.4f, colour.brighter(0.1f),
            img, 1.0f, findColour(Slider::textBoxHighlightColourId));
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

        voicesLabel.setText("SEQUENCER VOICES", NotificationType::dontSendNotification);
        voicesLabel.setJustificationType(Justification::centredBottom);
        channelsLabel.setText("MIDI CHANNELS", NotificationType::dontSendNotification);
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

class TwoValueSliderAttachment
{
public:
    TwoValueSliderAttachment(juce::AudioProcessorValueTreeState& apvts,
        const juce::String& paramIDMin,
        const juce::String& paramIDMax,
        juce::Slider& slider)
        : paramMinAttachment(*apvts.getParameter(paramIDMin), [this, &slider](float newValue)
            {
                ignoreCallbacks = true;
                slider.setMinValue(newValue, juce::dontSendNotification);
                ignoreCallbacks = false;
            }),
        paramMaxAttachment(*apvts.getParameter(paramIDMax), [this, &slider](float newValue)
            {
                ignoreCallbacks = true;
                slider.setMaxValue(newValue, juce::dontSendNotification);
                ignoreCallbacks = false;
            }),
        minParam(apvts.getParameter(paramIDMin)),
        maxParam(apvts.getParameter(paramIDMax))
    {
        jassert(dynamic_cast<juce::AudioParameterFloat*>(minParam) != nullptr);
        jassert(dynamic_cast<juce::AudioParameterFloat*>(maxParam) != nullptr);

        slider.onValueChange = [this, &slider]()
            {
                if (!ignoreCallbacks)
                {
                    minParam->beginChangeGesture();
                    maxParam->beginChangeGesture();

                    minParam->setValueNotifyingHost((float)slider.getMinValue());
                    maxParam->setValueNotifyingHost((float)slider.getMaxValue());

                    minParam->endChangeGesture();
                    maxParam->endChangeGesture();
                }
                onValueChange();
            };

        // Initialize UI with parameter values
        slider.setMinValue(minParam->getValue(), juce::dontSendNotification);
        slider.setMaxValue(maxParam->getValue(), juce::dontSendNotification);
    }

    std::function<void()> onValueChange;

private:
    juce::ParameterAttachment paramMinAttachment;
    juce::ParameterAttachment paramMaxAttachment;

    juce::RangedAudioParameter* minParam;
    juce::RangedAudioParameter* maxParam;

    bool ignoreCallbacks = false;
};
