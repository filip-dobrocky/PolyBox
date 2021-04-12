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
        sclLabel.setText("Scale: " + scl.name, NotificationType::dontSendNotification);
        kbmLabel.setText("Mapping: " + kbm.name, NotificationType::dontSendNotification);
        sclButton.onClick = [&] { loadScale(); };
        kbmButton.onClick = [&] { loadMapping(); };
        addAndMakeVisible(sclButton);
        addAndMakeVisible(kbmButton);
        addAndMakeVisible(sclLabel);
        addAndMakeVisible(kbmLabel);
    }

    void resized() override
    {
        FlexBox fb;
        const auto margin = FlexItem::Margin(5);
        fb.flexDirection = FlexBox::Direction::column;
        fb.items.add(FlexItem(sclLabel).withFlex(0.5).withMargin(margin));
        fb.items.add(FlexItem(sclButton).withFlex(1).withMargin(margin));
        fb.items.add(FlexItem(kbmLabel).withFlex(0.5).withMargin(margin));
        fb.items.add(FlexItem(kbmButton).withFlex(1).withMargin(margin));
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
    Label sclLabel;
    Label kbmLabel;

    void loadScale()
    {
        if (sclChooser.browseForFileToOpen())
        {
            scl = readSCLFile(sclChooser.getResult().getFullPathName().toStdString());
            tuning = std::make_shared<Tuning>(scl, kbm);
            sclLabel.setText("Scale: " + scl.name, NotificationType::dontSendNotification);
        }
    }

    void loadMapping()
    {
         if (kbmChooser.browseForFileToOpen())
         {
             kbm = readKBMFile(kbmChooser.getResult().getFullPathName().toStdString());
             tuning = std::make_shared<Tuning>(scl, kbm);
             kbmLabel.setText("Mapping: " + kbm.name, NotificationType::dontSendNotification);
         }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningSelector)
};
