/*
  ==============================================================================

    Filter.cpp
    Created: 3 May 2019 4:52:35pm
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Filter.h"

//==============================================================================
Filter::Filter(FmodSynthAudioProcessor& p) : processor (p)
{
	setSize(150, 256);
	filterMenu.addItem("Low Pass", 1);
	filterMenu.addItem("High Pass", 2);
	filterMenu.addItem("Band Pass", 3);
	filterMenu.addItem("Notch", 4);
	filterMenu.setJustificationType(Justification::centred);
	filterMenu.setLookAndFeel(&look);
	addAndMakeVisible(&filterMenu);

	filterCutOff.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	filterCutOff.setRange(20, 20000, 1);
	filterCutOff.setSkewFactorFromMidPoint(1000);
	filterCutOff.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	filterCutOff.setLookAndFeel(&look);
	//decaySlider.addListener(this);
	filterCutOff.setName("freq");


	addAndMakeVisible(filterCutOff);

	filterRes.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	filterRes.setRange(1.0, 5.0, 0.01);
	filterRes.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	filterRes.setLookAndFeel(&look);
	//decaySlider.addListener(this);
	filterRes.setName("Q");

	addAndMakeVisible(filterRes);

	cutoffLabel.setText("freq", NotificationType::dontSendNotification);
	cutoffLabel.setJustificationType(Justification::topLeft);
	cutoffLabel.setColour(Label::textColourId, Colours::white);
	cutoffLabel.attachToComponent(&filterCutOff, false);
	cutoffLabel.setLookAndFeel(&look);
	addAndMakeVisible(&cutoffLabel);

	resLabel.setText("Q", NotificationType::dontSendNotification);
	resLabel.setJustificationType(Justification::topLeft);
	resLabel.setColour(Label::textColourId, Colours::white);
	resLabel.attachToComponent(&filterRes, false);
	resLabel.setLookAndFeel(&look);
	addAndMakeVisible(&resLabel);


	filterTypeValue = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.tree, "filterType", filterMenu);
	filterValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "filterCutoff", filterCutOff);
	filterResValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "filterResonance", filterRes);
}

Filter::~Filter()
{
}

void Filter::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

	g.fillAll(Colours::black);

	g.setColour(Colours::white);
	auto area = getLocalBounds().toFloat().reduced(4);
	g.drawRoundedRectangle(area, 4, 1.5);
	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0,4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);
	g.setColour(Colours::black);
	g.drawText("Filter", area.reduced(6).removeFromTop(16), Justification::horizontallyJustified, true);   // Envelope Label
}

void Filter::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);

	filterMenu.setBounds(area.removeFromTop(40));
	filterCutOff.setBounds(12, 120, 64, 64);
	filterRes.setBounds(76, 120, 64, 64);

}
