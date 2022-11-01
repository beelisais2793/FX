/*
  ==============================================================================

    Envelope.h
    Created: 2 May 2019 2:43:26am
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLook.h"

#define attackPID "attack"
#define decayPID "decay"
#define sustainPID "sustain"
#define releasePID "release"

//==============================================================================
/*
*/

class Envelope    : public Component
{
public:
    Envelope(FmodSynthAudioProcessor&, juce::String t);
    ~Envelope();

    void paint (Graphics&) override;
    void resized() override; 
	static int instance;

private:
	FmodSynthAudioProcessor& processor;
	juce::String tag = "";
	LookAndFeel_V4 style;
	CustomLook look;

	juce::String attackParamID		= "attack-";
	juce::String decayParamID		= "decay-";
	juce::String sustainParamID		= "sustain-";
	juce::String releaseParamID		= "release-";
	juce::String name				= "Envelope";

	Slider attackSlider;
	Slider decaySlider;
	Slider sustainSlider;
	Slider releaseSlider;

	Label attackLabel;
	Label decayLabel;
	Label sustainLabel;
	Label releaseLabel;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sustainValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseValue;

	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Envelope)
};
