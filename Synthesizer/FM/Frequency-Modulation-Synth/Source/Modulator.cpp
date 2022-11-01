/*
  ==============================================================================

    Modulator.cpp
    Created: 28 Apr 2019 5:13:54pm
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulator.h"

//==============================================================================
Modulator::Modulator(FmodSynthAudioProcessor& p) : processor (p)
{
	setSize(200, 256);

	//oscMenu.addItem("Sine", 1);
	//oscMenu.addItem("Saw", 2);
	//oscMenu.addItem("Square", 3);
	//oscMenu.addItem("Triangle", 4);
	////oscMenu.addListener(this);
	//oscMenu.setJustificationType(Justification::centred); 

	//addAndMakeVisible(&oscMenu);


	oscSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	//oscSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	oscSlider.setRange(0, 4, 1);
	oscSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 14);
	oscSlider.setLookAndFeel(&look);
	oscSlider.setName("Modulator");

	addAndMakeVisible(&oscSlider);

	modIndexSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	modIndexSlider.setRange(0.0, 1.0, 0.01);
	modIndexSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	modIndexSlider.setLookAndFeel(&look);
	modIndexSlider.setName("Index");

	addAndMakeVisible(modIndexSlider);	


	modFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	modFreqSlider.setRange(20, 20000, 1);
	modFreqSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	modFreqSlider.setLookAndFeel(&look);
	modFreqSlider.setName("Frequency");

	addAndMakeVisible(modFreqSlider);

	fixedFreqButton.setName("Fixed Frequency");
	fixedFreqButton.setClickingTogglesState(true);
	fixedFreqButton.setToggleState(0, dontSendNotification);

	addAndMakeVisible(fixedFreqButton);

	freqRatio.setName("Ratio");
	freqRatio.setLookAndFeel(&look);
	freqRatio.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	freqRatio.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 14);
	freqRatio.setRange(0, 3, 1);

	addAndMakeVisible(freqRatio);

	modFreqLabel.setText("freq", NotificationType::dontSendNotification);
	modFreqLabel.setJustificationType(Justification::centred);
	modFreqLabel.setColour(Label::textColourId, Colours::white);
	modFreqLabel.attachToComponent(&modFreqSlider, false);
	modFreqLabel.setLookAndFeel(&look);
	addAndMakeVisible(modFreqLabel);

	oscLabel.setText("osc", NotificationType::dontSendNotification);
	oscLabel.setJustificationType(Justification::centred);
	oscLabel.setColour(Label::textColourId, Colours::white);
	oscLabel.attachToComponent(&oscSlider, false);
	oscLabel.setLookAndFeel(&look);
	//oscLabel.setColour(Label::textColourId, Colours::transparentBlack);
	addAndMakeVisible(oscLabel);

	modIndexLabel.setText("mod index", NotificationType::dontSendNotification);
	modIndexLabel.setJustificationType(Justification::centred);
	modIndexLabel.setColour(Label::textColourId, Colours::white);
	modIndexLabel.attachToComponent(&modIndexSlider, false);
	modIndexLabel.setLookAndFeel(&look);
	//oscLabel.setColour(Label::textColourId, Colours::transparentBlack);
	addAndMakeVisible(modIndexLabel);

	freqRatioLabel.setText("ratio", NotificationType::dontSendNotification);
	freqRatioLabel.setJustificationType(Justification::centred);
	freqRatioLabel.setColour(Label::textColourId, Colours::white);
	freqRatioLabel.attachToComponent(&freqRatio, false);
	freqRatioLabel.setLookAndFeel(&look);
	addAndMakeVisible(freqRatioLabel);



	// this changes waveType param
	//oscType = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.tree, "modType", oscMenu);
	modIndexValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "modIndex", modIndexSlider);
	oscValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "modType", oscSlider);
	modFreqValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "modFreq", modFreqSlider);
	fixedFreqValue = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.tree, "fixedFreq", fixedFreqButton);
	freqRatioValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "freqRatio", freqRatio);
}

Modulator::~Modulator()
{
}

void Modulator::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
	g.fillAll(Colours::black);

	auto area = getLocalBounds().toFloat().reduced(4);
    g.setColour (Colours::white);
	g.drawRoundedRectangle(area, 4, 1.5);
	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0, 4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);

    g.setFont(14.0f);
	g.setColour(Colours::black);
    g.drawText ("Modulator", area.reduced(6).removeFromTop(16),
                Justification::topLeft, true);   // draw some placeholder text
}

void Modulator::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);

	//oscMenu.setBounds(area.removeFromTop(20));
	oscSlider.setBounds(25, 60, 50, 128);
	modIndexSlider.setBounds(118, 60, 64, 64);
	modFreqSlider.setBounds(118, 150, 64, 64);
	fixedFreqButton.setBounds(50, 180, 50, 50);
}
