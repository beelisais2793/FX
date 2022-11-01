/*
  ==============================================================================

    Router.cpp
    Created: 21 May 2019 2:07:19am
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Router.h"

//==============================================================================
Router::Router(FmodSynthAudioProcessor& p) : processor(p)
{
	setSize(100, 128);

	algorithmSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	algorithmSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 14);
	algorithmSlider.setLookAndFeel(&look);
	algorithmSlider.setRange(0, 5, 1);
	algorithmSlider.setName("algorithm");
	addAndMakeVisible(&algorithmSlider);

	algorithmLabel.setText("algorithm", NotificationType::dontSendNotification);
	algorithmLabel.setJustificationType(Justification::centred);
	algorithmLabel.setColour(Label::textColourId, Colours::white);
	algorithmLabel.attachToComponent(&algorithmSlider, false);
	algorithmLabel.setLookAndFeel(&look);
	addAndMakeVisible(algorithmLabel);

	algorithmValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "algorithmSelection", algorithmSlider);
}

Router::~Router()
{
}

void Router::paint (Graphics& g)
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
	g.drawText("Algorithm", area.reduced(6).removeFromTop(16),
		Justification::topLeft, true);   // draw some placeholder text
}

void Router::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(10);

	algorithmSlider.setBounds(18, 52, 64, 64);

}
