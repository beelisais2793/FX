/*
  ==============================================================================

    GainProcessor.h
    Created: 21 Apr 2020 8:18:03pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "GainUI.h"

//==============================================================================
/*
*/
class GainProcessor    : public ModuleProcessor
{
public:
    GainProcessor() :
    ModuleProcessor(
        1, // Inlets
        1, // Outlets
        //============= Parameters =============
        std::make_unique<AudioParameterFloat> (
            "gain",
            "Gain",
            NormalisableRange<float> (-70.0f, 12.0f),
            0.0f,
            "Gain",
            AudioParameterFloat::genericParameter,
            [](float value, int) { return String (value, 1); },
            [](const String& text) { return text.getFloatValue(); }
        )
    )
    {}
    
    ~GainProcessor() {}
    
    void prepareToPlay (double sampleRate, int maxBlockSize) override {}
    
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override
    {
       buffer.applyGain(juce::Decibels::decibelsToGain((float)*params.getRawParameterValue("gain")));
    }
    
    void releaseResources() override {}
    
    AudioProcessorEditor* createEditor() override {return new GainUI(*this);}

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainProcessor)
};
