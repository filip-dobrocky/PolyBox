/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyBoxAudioProcessorEditor::PolyBoxAudioProcessorEditor (PolyBoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    mSequencerGrid = new SequencerGrid(audioProcessor.getSequencerPtr());
    addAndMakeVisible(mSequencerGrid);

    playButton.setClickingTogglesState(true);
    playButton.onClick = [&] { 
        bool playing = playButton.getToggleState();
        playButton.setButtonText(playing ? "Stop" : "Play");
        mSequencerGrid->togglePlay();
    };
    resetButton.onClick = [&] { mSequencerGrid->reset(); };
    addAndMakeVisible(playButton);
    addAndMakeVisible(resetButton);

    bpmSlider.setSliderStyle(Slider::LinearBar);
    bpmSlider.setRange(30, 300, 1);
    bpmSlider.setTextValueSuffix(" BPM");
    bpmSlider.onValueChange = [&] { bpmChanged(); };
    addAndMakeVisible(bpmSlider);

    if (p.canSync())
    {
        syncButton.setButtonText("Sync");
        syncButton.changeWidthToFitText();
        syncButton.setClickingTogglesState(true);
        syncButton.onClick = [&] { toggleSync(); };
        addAndMakeVisible(syncButton);
    }

    durationSlider.onValueChange = [&] { durationChanged(); };
    addAndMakeVisible(durationSlider);

    setSize (1000, 500);
}

PolyBoxAudioProcessorEditor::~PolyBoxAudioProcessorEditor()
{
    delete mSequencerGrid;
}

//==============================================================================
void PolyBoxAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void PolyBoxAudioProcessorEditor::resized()
{
    FlexBox topBarFb;

    auto margin = FlexItem::Margin(10, 10, 10, 10);

    topBarFb.items.add(FlexItem(playButton).withFlex(0.5).withMargin(margin));
    topBarFb.items.add(FlexItem(resetButton).withFlex(0.5).withMargin(margin));
    topBarFb.items.add(FlexItem(bpmSlider).withFlex(1).withMargin(margin));
    topBarFb.items.add(FlexItem(syncButton).withFlex(1).withMargin(margin));
    topBarFb.items.add(FlexItem(durationSlider).withFlex(1).withMargin(margin));

    auto bounds = getLocalBounds();
    topBarFb.performLayout(bounds.removeFromTop(50));
    mSequencerGrid->setBounds(bounds);
}

void PolyBoxAudioProcessorEditor::toggleSync()
{
    const auto state = syncButton.getToggleState();
    const auto seq = audioProcessor.getSequencerPtr();
    audioProcessor.syncOn = state;
    
    if (!state)
        bpmChanged();

    bpmSlider.setEnabled(!state);
}

void PolyBoxAudioProcessorEditor::durationChanged()
{
    const auto seq = audioProcessor.getSequencerPtr();
    seq->setDuration(durationSlider.getValue());
}

void PolyBoxAudioProcessorEditor::bpmChanged()
{
    const auto seq = audioProcessor.getSequencerPtr();
    seq->setTempo(bpmSlider.getValue());
}
