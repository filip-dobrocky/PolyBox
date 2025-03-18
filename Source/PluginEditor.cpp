/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
PolyBoxAudioProcessorEditor::PolyBoxAudioProcessorEditor (PolyBoxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), tabs(p.parameters)
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

SamplerComponent& PolyBoxAudioProcessorEditor::getSamplerComponent()
{
	return static_cast<MainPage*>(tabs.getTabContentComponent(0))->samplerComponent;
}


PolyBoxAudioProcessorEditor::MainPage::MainPage(PolyBoxAudioProcessor& p) : audioProcessor(p),
                                                                            sequencer(p.sequencer),
                                                                            samplerComponent(p)
{
    sequencerGrid = new SequencerGrid(audioProcessor);
    addAndMakeVisible(sequencerGrid);


    playButton.onClick = [&] { 
		if (playButton.getToggleState())
			sequencer.play();
        else
			sequencer.stop();
    };
    resetButton.onClick = [&] { sequencerGrid->reset(); };
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(recordButton);

    bpmSlider.setSliderStyle(Slider::LinearBar);
    bpmSlider.setTextValueSuffix(" BPM");
    addAndMakeVisible(bpmSlider);

    if (p.canSync())
    {
        syncButton.setButtonText("Host Sync");
        syncButton.changeWidthToFitText();
        syncButton.setClickingTogglesState(true);
        syncButton.onClick = [&] { toggleSync(); };
        addAndMakeVisible(syncButton);
    }

	syncAttachment.reset(new ButtonAttachment(p.parameters, "syncOn", syncButton));
	recordAttachment.reset(new ButtonAttachment(p.parameters, "transposeOn", recordButton));
	durationAttachment.reset(new SliderAttachment(p.parameters, "duration", durationSlider));
	bpmAttachment.reset(new SliderAttachment(p.parameters, "tempo", bpmSlider));

    addAndMakeVisible(durationSlider);
    addAndMakeVisible(samplerComponent);

	toggleSync();
 
    startTimer(30);
}

PolyBoxAudioProcessorEditor::MainPage::~MainPage()
{
    delete sequencerGrid;
}

void PolyBoxAudioProcessorEditor::MainPage::resized()
{
    FlexBox topBarFb;

    auto margin = FlexItem::Margin(10, 10, 10, 10);

    topBarFb.items.add(FlexItem(playButton).withFlex(0.15).withMargin(margin));
    topBarFb.items.add(FlexItem(resetButton).withFlex(0.15).withMargin(margin));
    topBarFb.items.add(FlexItem(recordButton).withFlex(0.15).withMargin(margin));
    topBarFb.items.add(FlexItem(bpmSlider).withFlex(1).withMargin(margin));
    topBarFb.items.add(FlexItem(syncButton).withFlex(1).withMargin(margin));
    topBarFb.items.add(FlexItem(durationSlider).withFlex(1).withMargin(margin));

    auto bounds = getLocalBounds();
    topBarFb.performLayout(bounds.removeFromTop(50));
    
    samplerComponent.setBounds(bounds.removeFromRight(getWidth()/4));
    sequencerGrid->setBounds(bounds.reduced(8));
}

void PolyBoxAudioProcessorEditor::MainPage::timerCallback()
{
	sequencerGrid->refreshRows();
	playButton.setToggleState(sequencer.isPlaying(), NotificationType::dontSendNotification);
}  

void PolyBoxAudioProcessorEditor::MainPage::toggleSync()
{
    const auto state = syncButton.getToggleState();

    bpmSlider.setEnabled(!state);
	playButton.setEnabled(!state);
	resetButton.setEnabled(!state);
}

void PolyBoxAudioProcessorEditor::MainPage::durationChanged()
{
    auto& seq = audioProcessor.sequencer;
    seq.setDuration(durationSlider.getValue());
}

PolyBoxAudioProcessorEditor::ConfigPage::ConfigPage(PolyBoxAudioProcessor& p) : audioProcessor(p),
                                                                                tuningSelector(p),
                                                                                sequencer(p.sequencer)
{
    matrix.addListener(this);
    matrix.initialise(sequencer);
    addAndMakeVisible(matrix);
    addAndMakeVisible(tuningSelector);
}

void PolyBoxAudioProcessorEditor::ConfigPage::resized()
{
    FlexBox fb;
    fb.items.add(FlexItem(matrix).withMaxHeight(250).withMaxWidth(250).withFlex(1));
    fb.items.add(FlexItem(tuningSelector).withFlex(0.8));
    fb.performLayout(getLocalBounds());
}

void PolyBoxAudioProcessorEditor::ConfigPage::connectionChanged(int voice, int channel, bool state)
{
    auto voicePtr = sequencer.voices[voice];
    if (state)
        voicePtr->assignChannel(channel);
    else
        voicePtr->deassignChannel(channel);
}
