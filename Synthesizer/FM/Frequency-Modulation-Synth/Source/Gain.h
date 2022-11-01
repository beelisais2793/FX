/*
  ==============================================================================

    Gain.h
    Created: 4 May 2019 6:00:58pm
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLook.h"

//==============================================================================
/*
*/
class Gain    : public Component
{
public:
    Gain(FmodSynthAudioProcessor&);
    ~Gain();

    void paint (Graphics&) override;
    void resized() override;

private:

	FmodSynthAudioProcessor& processor;
	CustomLook look;

	Slider gainSlider;
	Label gainLabel;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gain)
};
