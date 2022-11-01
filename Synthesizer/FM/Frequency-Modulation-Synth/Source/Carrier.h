/*
  ==============================================================================

    Carrier.h
    Created: 28 Apr 2019 5:13:22pm
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
class Carrier    : public Component
{
public:
    Carrier(FmodSynthAudioProcessor&);
    ~Carrier();

    void paint (Graphics&) override;
	void resized() override;	
	
	/** Called when a ComboBox has its selected item changed. */
	//void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FmodSynthAudioProcessor& processor;

	CustomLook look;

	// ScopedPointer<AudioProcessorValueTreeState::ComboBoxAttachment> waveSelection;
	ComboBox oscMenu;

	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> carrierType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Carrier)
};
