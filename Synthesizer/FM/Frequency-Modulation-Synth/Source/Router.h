/*
  ==============================================================================

    Router.h
    Created: 21 May 2019 2:07:19am
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
class Router    : public Component
{
public:
    Router(FmodSynthAudioProcessor&);
    ~Router();

    void paint (Graphics&) override;
    void resized() override;

private:

	FmodSynthAudioProcessor& processor;

	CustomLook look;

	Slider algorithmSlider;
	Label algorithmLabel;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> algorithmValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Router)
};
