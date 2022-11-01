/*
  ==============================================================================

    Carrier.cpp
    Created: 28 Apr 2019 5:13:22pm
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Carrier.h"

//==============================================================================
Carrier::Carrier(FmodSynthAudioProcessor& p) : processor (p)
{
	setSize(200, 128);

	oscMenu.addItem("Sine", 1);
	oscMenu.addItem("Saw", 2);
	oscMenu.addItem("Square", 3);
	oscMenu.addItem("Triangle", 4);
	oscMenu.setLookAndFeel(&look);
	addAndMakeVisible(&oscMenu);

	oscMenu.setJustificationType(Justification::centred);
	
	// this changes waveType param
	carrierType = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.tree, "waveType", oscMenu);
}

Carrier::~Carrier()
{
}

void Carrier::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
	g.fillAll(Colours::black);

    g.setColour (Colours::white);
	auto area = getLocalBounds().toFloat().reduced(4);
	g.drawRoundedRectangle(area, 4, 1.5);
	g.fillRoundedRectangle(area.removeFromTop(12), 4);
	g.fillRect(area.expanded(0, 4).removeFromTop(16));

	area = getLocalBounds().toFloat().reduced(4);
    //g.setFont (14.0f);
	g.setColour(Colours::black);
    g.drawText ("Carrier", area.reduced(6).removeFromTop(16), Justification::topLeft, true);   // draw some placeholder text
}

void Carrier::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);

	oscMenu.setBounds(area.removeFromTop(20));


}