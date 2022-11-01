/*
  ==============================================================================

    Envelope.cpp
    Created: 2 May 2019 2:43:26am
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Envelope.h"

//==============================================================================
Envelope::Envelope(FmodSynthAudioProcessor& p, juce::String t) : processor (p), tag(t)
{

	setSize(300, 128);
	//getLookAndFeel().setColour(Slider::thumbColourId, Colours::red);
	//getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Colours::aliceblue);
	//style.setColour(Slider::thumbColourId, Colours::slateblue);

	// otherLookAndFeel.setColour(Slider::thumbColourId, Colours::red);

	attackSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	attackSlider.setRange(0.01, 8.0, 0.01);
	attackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	attackSlider.setLookAndFeel(&look);
	attackSlider.setName("Attack");

	addAndMakeVisible(attackSlider);

	decaySlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	decaySlider.setRange(0.01, 1.0, 0.01);
	decaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	decaySlider.setLookAndFeel(&look);
	decaySlider.setName("Decay");

	addAndMakeVisible(decaySlider);

	sustainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	sustainSlider.setRange(0.0, 1.0, 0.01);
	sustainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 36, 16);
	sustainSlider.setLookAndFeel(&look);
	sustainSlider.setName("Sustain");

	addAndMakeVisible(sustainSlider);

	releaseSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	releaseSlider.setRange(0.01, 8.0, 0.01);
	releaseSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 36, 16);
	releaseSlider.setLookAndFeel(&look);
	releaseSlider.setName("Release");

	addAndMakeVisible(releaseSlider);

	attackLabel.setText("A", NotificationType::dontSendNotification);
	attackLabel.setJustificationType(Justification::topLeft);
	attackLabel.setColour(Label::textColourId, Colours::white);
	attackLabel.attachToComponent(&attackSlider, false);
	attackLabel.setLookAndFeel(&look);
	addAndMakeVisible(&attackLabel);

	decayLabel.setText("D", NotificationType::dontSendNotification);
	decayLabel.setJustificationType(Justification::bottomRight);
	decayLabel.setColour(Label::textColourId, Colours::white);
	decayLabel.attachToComponent(&decaySlider, false);
	decayLabel.setLookAndFeel(&look);
	addAndMakeVisible(&decayLabel);

	sustainLabel.setText("S", NotificationType::dontSendNotification);
	sustainLabel.setJustificationType(Justification::centredBottom);
	sustainLabel.setColour(Label::textColourId, Colours::white);
	sustainLabel.attachToComponent(&sustainSlider, false);
	sustainLabel.setLookAndFeel(&look);
	addAndMakeVisible(&sustainLabel);

	releaseLabel.setText("R", NotificationType::dontSendNotification);
	releaseLabel.setJustificationType(Justification::centredTop);
	releaseLabel.setColour(Label::textColourId, Colours::white);
	releaseLabel.attachToComponent(&releaseSlider, false);
	releaseLabel.setLookAndFeel(&look);
	addAndMakeVisible(&releaseLabel);

	attackParamID += tag;
	decayParamID += tag;
	sustainParamID += tag;
	releaseParamID += tag;
	name += " " + tag;

	// attackValue = new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "attack", attackSlider);

	attackValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, attackParamID, attackSlider);
	decayValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, decayParamID, decaySlider);
	sustainValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, sustainParamID, sustainSlider);
	releaseValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, releaseParamID, releaseSlider);

}

Envelope::~Envelope()
{
}



void Envelope::paint (Graphics& g)
{
	g.fillAll(Colours::black);

	g.setColour(Colours::white);
	auto area = getLocalBounds().toFloat().reduced(4);
	g.drawRoundedRectangle(area, 4, 1.5);

	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0, 4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);
	g.setColour(Colours::black);
	g.drawText(name, area.reduced(6).removeFromTop(16), Justification::horizontallyJustified, true);   // Envelope Label

}

void Envelope::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);
	attackSlider.setBounds(12, 52, 64, 64);
	decaySlider.setBounds(76, 52, 64, 64);
	sustainSlider.setBounds(140, 52, 64, 64);
	releaseSlider.setBounds(204, 52, 64, 64);
}
//void Envelope::sliderValueChanged(Slider* slider)
//{
//	// these sets the parameters each slider represents to the slider's value
//	//if (slider->getName() == "ModIndex")
//	//{
//	//	setParameterValue("ModIndex", modulationIndex.getValue());
//	//}
//	//else if (slider->getName() == "ModFreq")
//	//{
//	//	setParameterValue("ModFreq", modulationFrequency.getValue());
//	//}
//	//else if (slider->getName() == "Attack")
//	//{
//	//	setParameterValue("Attack", attackCTRL.getValue());
//	//}
//	//else if (slider->getName() == "Release")
//	//{
//	//	setParameterValue("Release", releaseCTRL.getValue());
//	//}
//}