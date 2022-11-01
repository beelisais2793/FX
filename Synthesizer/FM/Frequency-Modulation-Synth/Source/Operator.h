/*
  ==============================================================================

    Operator.h
    Created: 19 May 2019 3:36:09pm
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLook.h"

class Operator : public Component 
{
public:
	Operator(FmodSynthAudioProcessor&, juce::String t);
	~Operator();
	
	void paint(Graphics&) override;
	void resized() override;

private:

	FmodSynthAudioProcessor& processor;
	juce::String tag = "";

	juce::String indexParamID		= "modIndex";
	juce::String oscParamID			= "oscType";
	juce::String freqParamID		= "modFreq";
	juce::String fixedFreqParamID	= "fixedFreq";
	juce::String freqRatioParamID	= "freqRatio";
	juce::String name				= "Operator";

	CustomLook look;

	Slider oscSlider;
	Slider indexSlider;
	Slider fixedFreqSlider;
	Slider freqRatioSlider;
	ToggleButton fixedFreqButton{ "fixed freq" };

	Label oscLabel;
	Label indexLabel;
	Label fixedFreqLabel;
	Label freqRatioLabel;

	// Button fixedFreqButton;
	//TextButton fixedButton;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> indexValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> freqValue;
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> fixedFreqValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> freqRatioValue;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Operator)
};