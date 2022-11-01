/*
  ==============================================================================

    LowFrequencyOscillator.cpp
    Created: 4 May 2019 7:19:12pm
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LowFrequencyOscillator.h"

//==============================================================================
LowFrequencyOscillator::LowFrequencyOscillator(FmodSynthAudioProcessor& p) : processor (p)
{
	setSize(216, 128);

	oscSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	oscSlider.setRange(0, 4, 1);
	oscSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 36, 16);
	oscSlider.setLookAndFeel(&look);
	oscSlider.setValue(0);
	oscSlider.setName("LFO");

	addAndMakeVisible(&oscSlider);

	lfoRateSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	lfoRateSlider.setRange(0, 12000, 1);
	lfoRateSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 36, 16);
	lfoRateSlider.setLookAndFeel(&look);
	lfoRateSlider.setValue(0);
	lfoRateSlider.setName("Rate");

	addAndMakeVisible(&lfoRateSlider);

	lfoAmountSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	lfoAmountSlider.setRange(0, 1, 0.01);
	lfoAmountSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 36, 16);
	lfoAmountSlider.setLookAndFeel(&look);
	lfoAmountSlider.setValue(0);
	lfoAmountSlider.setName("Amount");
	
	addAndMakeVisible(&lfoAmountSlider);

	oscLabel.setText("osc", NotificationType::dontSendNotification);
	oscLabel.setJustificationType(Justification::centred);
	oscLabel.setColour(Label::textColourId, Colours::white);
	oscLabel.attachToComponent(&oscSlider, false);
	oscLabel.setLookAndFeel(&look);
	addAndMakeVisible(&oscLabel);

	rateLabel.setText("rate", NotificationType::dontSendNotification);
	rateLabel.setJustificationType(Justification::centred);
	rateLabel.setColour(Label::textColourId, Colours::white);
	rateLabel.attachToComponent(&lfoRateSlider, false);
	rateLabel.setLookAndFeel(&look);
	addAndMakeVisible(&rateLabel);

	amountLabel.setText("amount", NotificationType::dontSendNotification);
	amountLabel.setJustificationType(Justification::centred);
	amountLabel.setColour(Label::textColourId, Colours::white);
	amountLabel.attachToComponent(&lfoAmountSlider, false);
	amountLabel.setLookAndFeel(&look);
	addAndMakeVisible(&amountLabel);

	oscValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "lfoType", oscSlider);
	rateValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "lfoRate", lfoRateSlider);
	amountValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "lfoAmount", lfoAmountSlider);

}

LowFrequencyOscillator::~LowFrequencyOscillator()
{
}

void LowFrequencyOscillator::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

	g.fillAll(Colours::black);

	auto area = getLocalBounds().toFloat().reduced(4);
    g.setColour (Colours::white);
    // g.drawRect (getLocalBounds(), 2);   // draw an outline around the component
	g.drawRoundedRectangle(area, 4, 1.5);
	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0, 4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);

    g.setFont (14.0f);
	g.setColour(Colours::black);
	g.drawText("LFO", area.reduced(8).removeFromTop(16),
		Justification::horizontallyJustified, true);   // draw some placeholder text
}

void LowFrequencyOscillator::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);

	//oscMenu.setBounds(area.removeFromTop(20));
	oscSlider.setBounds(12, 52, 64, 64);
	lfoRateSlider.setBounds(76, 52, 64, 64);
	lfoAmountSlider.setBounds(140, 52, 64, 64);
}
