/*
  ==============================================================================

    LowFrequencyOscillator.h
    Created: 4 May 2019 7:19:12pm
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
class LowFrequencyOscillator    : public Component
{
public:
    LowFrequencyOscillator(FmodSynthAudioProcessor&);
    ~LowFrequencyOscillator();

    void paint (Graphics&) override;
    void resized() override;

private:

	FmodSynthAudioProcessor& processor;

	CustomLook look;

	Slider oscSlider;
	Slider lfoRateSlider;
	Slider lfoAmountSlider;

	Label oscLabel;
	Label rateLabel;
	Label amountLabel;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rateValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> amountValue;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LowFrequencyOscillator)
};
