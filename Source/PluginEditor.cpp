/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SamplerTestAudioProcessorEditor::SamplerTestAudioProcessorEditor (SamplerTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    mLoadSampleButton.onClick = [&] { audioProcessor.loadSample(); };
    mLoadTuningButton.onClick = [&] { audioProcessor.loadTuning(); };
    addAndMakeVisible(mLoadSampleButton);
    addAndMakeVisible(mLoadTuningButton);

    setSize (200, 200);
}

SamplerTestAudioProcessorEditor::~SamplerTestAudioProcessorEditor()
{
}

//==============================================================================
void SamplerTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void SamplerTestAudioProcessorEditor::resized()
{
    mLoadSampleButton.setBounds(getWidth() / 2 - 50, getHeight() / 4 - 50, 100, 50);
    mLoadTuningButton.setBounds(getWidth() / 2 - 50, 3 * getHeight() / 4 - 25, 100, 50);
}
