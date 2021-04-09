/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyBoxAudioProcessorEditor::PolyBoxAudioProcessorEditor (PolyBoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), tabs(TabbedButtonBar::TabsAtTop)
{
    auto colour = findColour(ResizableWindow::backgroundColourId);
    tabs.addTab("Play", colour, new MainPage(p), true);
    addAndMakeVisible(tabs);

    setSize (1000, 500);
}

PolyBoxAudioProcessorEditor::~PolyBoxAudioProcessorEditor()
{
}

//==============================================================================
void PolyBoxAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void PolyBoxAudioProcessorEditor::resized()
{
    tabs.setBounds(getLocalBounds());
}


PolyBoxAudioProcessorEditor::MainPage::MainPage(PolyBoxAudioProcessor& p) : audioProcessor(p)
{
    sequencerGrid = new SequencerGrid(audioProcessor.getSequencerPtr());
    addAndMakeVisible(sequencerGrid);

    playButton.setClickingTogglesState(true);
    playButton.onClick = [&] {
        bool playing = playButton.getToggleState();
        playButton.setButtonText(playing ? "Stop" : "Play");
        sequencerGrid->togglePlay();
    };
    resetButton.onClick = [&] { sequencerGrid->reset(); };
    addAndMakeVisible(playButton);
    addAndMakeVisible(resetButton);

    bpmSlider.setSliderStyle(Slider::LinearBar);
    bpmSlider.setRange(30, 300, 1);
    bpmSlider.setTextValueSuffix(" BPM");
    bpmSlider.onValueChange = [&] { bpmChanged(); };
    bpmSlider.setValue(p.getSequencerPtr()->getTempo());
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
}

PolyBoxAudioProcessorEditor::MainPage::~MainPage()
{
    delete sequencerGrid;
}

void PolyBoxAudioProcessorEditor::MainPage::resized()
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
    sequencerGrid->setBounds(bounds);
}

void PolyBoxAudioProcessorEditor::MainPage::toggleSync()
{
    const auto state = syncButton.getToggleState();
    const auto seq = audioProcessor.getSequencerPtr();
    audioProcessor.syncOn = state;
    
    if (!state)
        bpmChanged();

    bpmSlider.setEnabled(!state);
}

void PolyBoxAudioProcessorEditor::MainPage::durationChanged()
{
    const auto seq = audioProcessor.getSequencerPtr();
    seq->setDuration(durationSlider.getValue());
}

void PolyBoxAudioProcessorEditor::MainPage::bpmChanged()
{
    const auto seq = audioProcessor.getSequencerPtr();
    seq->setTempo(bpmSlider.getValue());
}
