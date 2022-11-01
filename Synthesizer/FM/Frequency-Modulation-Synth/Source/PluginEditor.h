/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ProcessorGraph.h"
#include "Envelope.h"
#include "Gain.h"
#include "Filter.h"
#include "Operator.h"
#include "LowFrequencyOscillator.h"
#include "Router.h"

//==============================================================================
/**
*/
class FmodSynthAudioProcessorEditor  : public AudioProcessorEditor,
									   public Slider::Listener 
{
public:
    FmodSynthAudioProcessorEditor (FmodSynthAudioProcessor&);
    ~FmodSynthAudioProcessorEditor();

    //==============================================================================
    void					paint (Graphics&) override;
    void					resized() override;
	void					sliderValueChanged(Slider*) override;

private:

	AudioProcessorParameter* getParameter(const String& paramId);
	float                    getParameterValue(const String& paramId);
	void                     setParameterValue(const String& paramId, float value);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FmodSynthAudioProcessor& processor;

	Envelope envelopeGui;
	Envelope envelopeB;
	Envelope envelopeC;
	Envelope envelopeD;
	Gain gainGui;
	LowFrequencyOscillator lfoGui;
	Filter filterGui;
	Operator operatorA;
	Operator operatorB;
	Operator operatorC;
	Operator operatorD;
	Router algorithmGui;
//	ProcessorGraph graphGui;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FmodSynthAudioProcessorEditor)
};
