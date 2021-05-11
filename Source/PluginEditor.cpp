/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyBoxAudioProcessorEditor::PolyBoxAudioProcessorEditor (PolyBoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), tabs(p.level)
{
    setLookAndFeel(&appLookAndFeel);
    auto colour = findColour(Slider::textBoxHighlightColourId);
    tabs.addTab("Play", colour, new MainPage(p), true);
    tabs.addTab("Config", colour, new ConfigPage(p), true);
    addAndMakeVisible(tabs);

    setResizable(true, true);
    setResizeLimits(900, 450, 2000, 1200);
    setSize (1000, 600);
}

PolyBoxAudioProcessorEditor::~PolyBoxAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
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


PolyBoxAudioProcessorEditor::MainPage::MainPage(PolyBoxAudioProcessor& p) : audioProcessor(p),
                                                                            sequencer(p.sequencer),
                                                                            samplerComponent(p.sampler)
{
    sequencerGrid = new SequencerGrid(audioProcessor.sequencer);
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
    bpmSlider.setValue(p.sequencer.getTempo());
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

    addAndMakeVisible(samplerComponent);
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
    
    samplerComponent.setBounds(bounds.removeFromRight(getWidth()/4));
    sequencerGrid->setBounds(bounds.reduced(8));
}
   

void PolyBoxAudioProcessorEditor::MainPage::toggleSync()
{
    const auto state = syncButton.getToggleState();
    audioProcessor.syncOn = state;
    
    if (!state)
        bpmChanged();

    bpmSlider.setEnabled(!state);
}

void PolyBoxAudioProcessorEditor::MainPage::durationChanged()
{
    auto& seq = audioProcessor.sequencer;
    seq.setDuration(durationSlider.getValue());
}

void PolyBoxAudioProcessorEditor::MainPage::bpmChanged()
{
    sequencer.setTempo(bpmSlider.getValue());
}

PolyBoxAudioProcessorEditor::ConfigPage::ConfigPage(PolyBoxAudioProcessor& p) : audioProcessor(p),
                                                                                tuningSelector(p.tuning),
                                                                                sequencer(p.sequencer)
{
    matrix.addListener(this);
    matrix.initialise(sequencer);
    addAndMakeVisible(matrix);
    addAndMakeVisible(tuningSelector);
}

void PolyBoxAudioProcessorEditor::ConfigPage::resized()
{
    matrix.setBounds(30, 30, 250, 250);
    tuningSelector.setBounds(400, 30, 500, 250);
}

void PolyBoxAudioProcessorEditor::ConfigPage::connectionChanged(int voice, int channel, bool state)
{
    auto voicePtr = sequencer.voices[voice];
    if (state)
        voicePtr->assignChannel(channel);
    else
        voicePtr->deassignChannel(channel);
}
