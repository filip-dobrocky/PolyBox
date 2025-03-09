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
#include "PluginProcessor.h"

using namespace Tunings;

class TuningSelector : public GroupComponent
{
public:
    TuningSelector(PolyBoxAudioProcessor& p) : processor(p), 
                                               tuning(p.tuning),
                                               scl(p.scl),
                                               kbm(p.kbm)
    {
        setText("Tuning");
        setTextLabelPosition(Justification::centred);
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
        spanSlider.onValueChange = [&] { 
            loadEqual(spanSlider.getValue(), divisionSlider.getValue());
            processor.equalTuningSpan = spanSlider.getValue(); 
        };
        divisionSlider.onValueChange = [&] {
            loadEqual(spanSlider.getValue(), divisionSlider.getValue()); 
            processor.equalTuningDivision = divisionSlider.getValue();
        };

        addAndMakeVisible(sclButton);
        addAndMakeVisible(kbmButton);
        addAndMakeVisible(equalButton);
        addAndMakeVisible(sclLabel);
        addAndMakeVisible(kbmLabel);
        addAndMakeVisible(byLabel);
        addAndMakeVisible(spanSlider);
        addAndMakeVisible(divisionSlider);
        
        if (processor.equalTuningDivision > 0)
            divisionSlider.setValue(processor.equalTuningDivision, NotificationType::dontSendNotification);
        if (processor.equalTuningSpan > 0)
            spanSlider.setValue(processor.equalTuningSpan, NotificationType::dontSendNotification);
        
        equalButton.setToggleState(processor.equalTuning, NotificationType::sendNotification);
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
    PolyBoxAudioProcessor& processor;
    Scale& scl;
    KeyboardMapping& kbm;
    std::shared_ptr<Tuning> tuning;

    std::unique_ptr<FileChooser> sclChooser;
    std::unique_ptr<FileChooser> kbmChooser;

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
        sclChooser = std::make_unique<FileChooser>("Load .scl", File::getSpecialLocation(File::userDesktopDirectory), "*.scl");
        auto chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

        sclChooser->launchAsync(chooserFlags, [this](const FileChooser& fc)
        {
            if (processor.loadScl(fc.getResult().getFullPathName()))
                sclLabel.setText("Scale: " + scl.description, NotificationType::dontSendNotification);
            else
                sclLabel.setText("Scale: could not open file", NotificationType::dontSendNotification);
        });
    }

    void loadMapping()
    {
        kbmChooser = std::make_unique<FileChooser>("Load .kbm", File::getSpecialLocation(File::userDesktopDirectory), "*.kbm");
        auto chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

        kbmChooser->launchAsync(chooserFlags, [this](const FileChooser& fc)
        {
            if (processor.loadKbm(fc.getResult().getFullPathName()))
                kbmLabel.setText("Mapping: " + kbm.name, NotificationType::dontSendNotification);
            else
                kbmLabel.setText("Mapping: could not open file", NotificationType::dontSendNotification);
        });
    }

    void loadEqual(int m, int n)
    {
        *tuning = Tuning(evenDivisionOfSpanByM(m, n));
        sclLabel.setText("Scale: " + (*tuning).scale.description, NotificationType::dontSendNotification);
    }

    void equalToggle()
    {
        processor.equalTuning = equalButton.getToggleState();
        processor.equalTuningSpan = spanSlider.getValue();
        processor.equalTuningDivision = divisionSlider.getValue();

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
