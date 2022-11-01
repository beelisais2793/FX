/*
  ==============================================================================

    Modulator.h
    Created: 28 Apr 2019 5:13:54pm
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
class Modulator    : public Component
{
public:
    Modulator(FmodSynthAudioProcessor&);
    ~Modulator();

    void paint (Graphics&) override;
    void resized() override;

private:

	FmodSynthAudioProcessor& processor;

	CustomLook look;

	ComboBox oscMenu;
	Slider modIndexSlider;
	Slider oscSlider;
	Slider modFreqSlider;
	Slider freqRatio;
	ToggleButton fixedFreqButton { "fixed freq" };
	Label modFreqLabel;
	Label oscLabel;
	Label modIndexLabel;
	Label freqRatioLabel;


	// Button fixedFreqButton;
	TextButton fixedButton;

	//std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> oscType;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modIndexValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modFreqValue;
	std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> fixedFreqValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> freqRatioValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Modulator)
};
