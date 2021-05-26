/*
  ==============================================================================

    TuningSelector.h
    Created: 11 Apr 2021 11:48:51am
    Author:  Filip

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Tunings.h"

using namespace Tunings;

class TuningSelector : public GroupComponent
{
public:
    TuningSelector(std::shared_ptr<Tuning> t) : tuning(t)
    {
        setText("Tuning");
        setTextLabelPosition(Justification::centred);
        scl = t->scale;
        kbm = t->keyboardMapping;
        sclLabel.setText("Scale: " + scl.description, NotificationType::dontSendNotification);
        kbmLabel.setText("Mapping: " + kbm.name, NotificationType::dontSendNotification);
        byLabel.setText("by", NotificationType::dontSendNotification);
        byLabel.setJustificationType(Justification::centred);
        spanSlider.setRange(2, 8, 1);
        divisionSlider.setRange(2, 1000, 1);
        divisionSlider.setValue(12, NotificationType::dontSendNotification);
        sclButton.onClick = [&] { loadScale(); };
        kbmButton.onClick = [&] { loadMapping(); };
        equalButton.onClick = [&] { equalToggle(); };
        spanSlider.onValueChange = [&] { loadEqual(spanSlider.getValue(), divisionSlider.getValue()); };
        divisionSlider.onValueChange = [&] { loadEqual(spanSlider.getValue(), divisionSlider.getValue()); };

        addAndMakeVisible(sclButton);
        addAndMakeVisible(kbmButton);
        addAndMakeVisible(equalButton);
        addAndMakeVisible(sclLabel);
        addAndMakeVisible(kbmLabel);
        addAndMakeVisible(byLabel);
        addAndMakeVisible(spanSlider);
        addAndMakeVisible(divisionSlider);
        equalButton.setToggleState(false, NotificationType::sendNotification);
        equalToggle();
    }

    void resized() override
    {
        FlexBox fb, fbEqual;
        const auto margin = FlexItem::Margin(5);
        fb.flexDirection = FlexBox::Direction::column;
        fbEqual.items.add(FlexItem(spanSlider).withFlex(1).withMargin(margin));
        fbEqual.items.add(FlexItem(byLabel).withFlex(1).withMargin(margin));
        fbEqual.items.add(FlexItem(divisionSlider).withFlex(1).withMargin(margin));

        fb.items.add(FlexItem(sclLabel).withFlex(0.5).withMargin(margin));
        fb.items.add(FlexItem(sclButton).withFlex(1).withMargin(margin));
        fb.items.add(FlexItem(kbmLabel).withFlex(0.5).withMargin(margin));
        fb.items.add(FlexItem(kbmButton).withFlex(1).withMargin(margin));
        fb.items.add(FlexItem(equalButton).withFlex(1).withMargin(margin));
        fb.items.add(FlexItem(fbEqual).withFlex(1).withMargin(margin));
        fb.performLayout(getLocalBounds().reduced(15));
    }

private:
    Scale scl;
    KeyboardMapping kbm;
    std::shared_ptr<Tuning> tuning;
    FileChooser sclChooser{ "Load .scl", File::getSpecialLocation(File::userDesktopDirectory), "*.scl" };
    FileChooser kbmChooser{ "Load .kbm", File::getSpecialLocation(File::userDesktopDirectory), "*.kbm" };

    TextButton sclButton{ "Load scale" };
    TextButton kbmButton{ "Load mapping" };
    ToggleButton equalButton{"Equal division"};
    Label sclLabel;
    Label kbmLabel;
    Label byLabel;
    Slider spanSlider{ Slider::IncDecButtons, Slider::TextBoxAbove };
    Slider divisionSlider{ Slider::IncDecButtons, Slider::TextBoxAbove };

    void loadScale()
    {
        try 
        {
            if (sclChooser.browseForFileToOpen())
            {
                scl = readSCLFile(sclChooser.getResult().getFullPathName().toStdString());
                *tuning = Tuning(scl, kbm);
                sclLabel.setText("Scale: " + scl.description, NotificationType::dontSendNotification);
            }
        }
        catch (...)
        {
            sclLabel.setText("Scale: could not open file", NotificationType::dontSendNotification);
        }
    }

    void loadMapping()
    {
        try
        {
            if (kbmChooser.browseForFileToOpen())
            {
                kbm = readKBMFile(kbmChooser.getResult().getFullPathName().toStdString());
                *tuning = Tuning(scl, kbm);
                kbmLabel.setText("Mapping: " + kbm.name, NotificationType::dontSendNotification);
            }
        }
        catch (...)
        {
            kbmLabel.setText("Mapping: could not open file", NotificationType::dontSendNotification);
        }
    }

    void loadEqual(int m, int n)
    {
        *tuning = Tuning(evenDivisionOfSpanByM(m, n));
        sclLabel.setText("Scale: " + (*tuning).scale.description, NotificationType::dontSendNotification);
    }

    void equalToggle()
    {
        if (equalButton.getToggleState())
        {
            spanSlider.setEnabled(true);
            divisionSlider.setEnabled(true);
            sclButton.setEnabled(false);
            kbmButton.setEnabled(false);
            loadEqual(spanSlider.getValue(), divisionSlider.getValue());
        }
        else
        {
            spanSlider.setEnabled(false);
            divisionSlider.setEnabled(false);
            sclButton.setEnabled(true);
            kbmButton.setEnabled(true);
            *tuning = Tuning(scl, kbm);
            sclLabel.setText("Scale: " + (*tuning).scale.description, NotificationType::dontSendNotification);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningSelector)
};
