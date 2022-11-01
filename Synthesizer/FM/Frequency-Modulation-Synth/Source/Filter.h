/*
  ==============================================================================

    Filter.h
    Created: 3 May 2019 4:52:34pm
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
class Filter    : public Component
{
public:
    Filter(FmodSynthAudioProcessor&);
    ~Filter();

    void paint (Graphics&) override;
    void resized() override;

private:
	FmodSynthAudioProcessor& processor;

	CustomLook look;

	Slider filterCutOff;
	Slider filterRes;
	ComboBox filterMenu;

	Label cutoffLabel;
	Label resLabel;
	// Label menuLabel;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterValue;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterResValue;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Filter)
};
