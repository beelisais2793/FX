/*
  ==============================================================================

    Gain.cpp
    Created: 4 May 2019 6:00:58pm
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gain.h"

//==============================================================================
Gain::Gain(FmodSynthAudioProcessor& p) : processor (p)
{
	setSize(100, 128);

	gainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	gainSlider.setRange(0.0, 1.0, 0.01);
	gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 36, 16);
	gainSlider.setLookAndFeel(&look);
	gainSlider.setValue(0.4);
	gainSlider.setName("Gain");

	addAndMakeVisible(gainSlider);

	gainLabel.setText("GAIN", NotificationType::dontSendNotification);
	gainLabel.setJustificationType(Justification::centred);
	gainLabel.setColour(Label::textColourId, Colours::white);
	gainLabel.attachToComponent(&gainSlider, false);
	gainLabel.setLookAndFeel(&look);
	addAndMakeVisible(&gainLabel);

	gainValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "masterGain", gainSlider);
}

Gain::~Gain()
{
}

void Gain::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    // g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
	g.fillAll(Colours::black);

	auto area = getLocalBounds().toFloat().reduced(4);
    g.setColour (Colours::white);
    // g.drawRect (getLocalBounds(), 2);   // draw an outline around the component
	g.drawRoundedRectangle(area, 4, 1.5);

    g.setColour (Colours::white);
    g.setFont (12);
	g.drawText("Master Gain", area.reduced(8).removeFromTop(16),
		Justification::horizontallyJustified, true);   // draw some placeholder text
}

void Gain::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(10);

	gainSlider.setBounds(18, 52, 64, 64);

}
