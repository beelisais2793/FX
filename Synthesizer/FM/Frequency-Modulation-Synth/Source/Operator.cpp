/*
  ==============================================================================

    Operator.cpp
    Created: 19 May 2019 3:36:09pm
    Author:  quasa

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "Operator.h"

Operator::Operator(FmodSynthAudioProcessor& p, juce::String t) : processor(p), tag(t)
{
	setSize(350, 128);

	//==============================================================================

	oscSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	oscSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 14);
	oscSlider.setLookAndFeel(&look);
	oscSlider.setRange(0, 4, 1);
	oscSlider.setName("osc");
	addAndMakeVisible(&oscSlider);

	indexSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	indexSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	indexSlider.setLookAndFeel(&look);
	indexSlider.setRange(0.0, 1.0, 0.01);
	indexSlider.setName("index");
	addAndMakeVisible(indexSlider);

	fixedFreqSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	fixedFreqSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	fixedFreqSlider.setLookAndFeel(&look);
	fixedFreqSlider.setRange(20, 20000, 1);
	fixedFreqSlider.setName("frequency");
	addAndMakeVisible(fixedFreqSlider);

	freqRatioSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	freqRatioSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 14);
	freqRatioSlider.setLookAndFeel(&look);
	//freqRatioSlider.setRange(0, 3, 1);
	freqRatioSlider.setRange(-2400, 2400, 100);
	freqRatioSlider.setName("Ratio");
	addAndMakeVisible(freqRatioSlider);

	fixedFreqButton.setName("Fixed Frequency");
	fixedFreqButton.setClickingTogglesState(true);
	fixedFreqButton.setToggleState(0, dontSendNotification);
	addAndMakeVisible(fixedFreqButton);

	//==============================================================================
	// Labels //
	//==============================================================================

	fixedFreqLabel.setText("freq", NotificationType::dontSendNotification);
	fixedFreqLabel.setJustificationType(Justification::centred);
	fixedFreqLabel.setColour(Label::textColourId, Colours::white);
	fixedFreqLabel.attachToComponent(&fixedFreqSlider, false);
	fixedFreqLabel.setLookAndFeel(&look);
	addAndMakeVisible(fixedFreqLabel);

	oscLabel.setText("osc", NotificationType::dontSendNotification);
	oscLabel.setJustificationType(Justification::centred);
	oscLabel.setColour(Label::textColourId, Colours::white);
	oscLabel.attachToComponent(&oscSlider, false);
	oscLabel.setLookAndFeel(&look);
	addAndMakeVisible(oscLabel);

	indexLabel.setText("mod index", NotificationType::dontSendNotification);
	indexLabel.setJustificationType(Justification::centred);
	indexLabel.setColour(Label::textColourId, Colours::white);
	indexLabel.attachToComponent(&indexSlider, false);
	indexLabel.setLookAndFeel(&look);
	addAndMakeVisible(indexLabel);

	freqRatioLabel.setText("ratio", NotificationType::dontSendNotification);
	freqRatioLabel.setJustificationType(Justification::centred);
	freqRatioLabel.setColour(Label::textColourId, Colours::white);
	freqRatioLabel.attachToComponent(&freqRatioSlider, false);
	freqRatioLabel.setLookAndFeel(&look);
	addAndMakeVisible(freqRatioLabel);

	//==============================================================================
	// Params //
	//==============================================================================

	indexParamID += tag;
	oscParamID += tag;
	freqParamID += tag;
	freqRatioParamID += tag;	
	fixedFreqParamID += tag;
	name += " " + tag;

	indexValue		= std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, indexParamID, indexSlider);
	oscValue		= std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, oscParamID, oscSlider);
	freqValue		= std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, freqParamID, fixedFreqSlider);
	freqRatioValue	= std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, freqRatioParamID, freqRatioSlider);
	fixedFreqValue	= std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.tree, fixedFreqParamID, fixedFreqButton);

}

Operator::~Operator() {}

void Operator::paint(Graphics& g)
{

	g.fillAll(Colours::black);

	auto area = getLocalBounds().toFloat().reduced(4);
	g.setColour(Colours::white);
	g.drawRoundedRectangle(area, 4, 1.5);
	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0, 4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);

	g.setFont(14.0f);
	g.setColour(Colours::black);
	g.drawText(name, area.reduced(6).removeFromTop(16),
		Justification::topLeft, true);   // draw some placeholder text
}

void Operator::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);

	oscSlider.setBounds(12, 52, 64, 64);
	indexSlider.setBounds(76, 52, 64, 64);
	fixedFreqSlider.setBounds(140, 52, 64, 64);
	fixedFreqButton.setBounds(204, 52, 50, 50);
	freqRatioSlider.setBounds(268, 52, 64, 64);
}
