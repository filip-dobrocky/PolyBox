/*
  ==============================================================================

    AppLookAndFeel.h
    Created: 3 May 2021 5:59:07pm
    Author:  Filip

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AppLookAndFeel : public LookAndFeel_V4
{
public:
    AppLookAndFeel()
    {
        setColourScheme({ 0xff235ABE, 0xff0D42A2, 0xff67ABE3,
                          0xff123b7b, 0xc8ffffff, 0xff67ABE3,
                          0xffffffff, 0xff0D42A2, 0xff000000 });
    }
};