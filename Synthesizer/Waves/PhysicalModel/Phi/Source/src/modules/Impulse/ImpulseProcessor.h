/*
  ==============================================================================

    ImpusleProcessor.h
    Created: 25 Apr 2020 12:10:22am
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once


///@cond
#include <JuceHeader.h>
///@endcond
#include "ImpulseUI.h"

class ImpulseProcessor : public ModuleProcessor
{
public:
    static const float constexpr invTwoPi = 1.0f/MathConstants<float>::twoPi;
    
    ImpulseProcessor() :
    ModuleProcessor(
        3, // Inlets
        2, // Outlets
        //============= Parameters =============
        std::make_unique<AudioParameterFloat> (
           "freq",
           "Frequency",
           NormalisableRange<float> (20.0f, 20000.0f, 0, 0.2f),
           20.0f,
           "Frequency",
           AudioParameterFloat::genericParameter,
           [](float value, int) { return String (value, 1); },
           [](const String& text) { return text.getFloatValue(); }
        ),
        std::make_unique<AudioParameterFloat> (
           "shape",
           "Shape",
           NormalisableRange<float> (0.0f, 1.0f),
           0.0f,
           "Shape",
           AudioParameterFloat::genericParameter,
           [](float value, int) { return String (floorf(value * 100.0f), 0); },
           [](const String& text) { return text.getFloatValue() * 0.01f; }
        ),
        std::make_unique<AudioParameterBool> (
           "trigger",
           "Trigger",
           false
        )
    )
    {}
    
    ~ImpulseProcessor() {}
    
    void prepareToPlay (double newSampleRate, int maxBlockSize) override
    {
        sampleRate = float(newSampleRate);
    }
    
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        const float* readBufferTrigger = buffer.getReadPointer(0);
        
        // @todo CV Implementations
        //const float* readBufferFreq = buffer.getReadPointer(1);
        //const float* readBufferShape = buffer.getReadPointer(2);
        
        const float shape = powf(*params.getRawParameterValue("shape"), 0.1f);
        
        const float phaseIncrement = (1.0f/(sampleRate/(*params.getRawParameterValue("freq")))) * MathConstants<float>::twoPi;
        
        float* writeBufferOut = buffer.getWritePointer(0);
        float* writeBufferRamp = buffer.getWritePointer(1);
        
        // for now, we constantly process audio, but ideally, the module should know if it is connected
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            const float trigger = *readBufferTrigger++;
            const float triggerDelta = previousTrigger - trigger;
            previousTrigger = trigger;
            
            if (triggerDelta > 0.5f || wasExternallyTriggered()) currentPhase = 0.0f;
            
            *writeBufferOut++ = processImpulse(currentPhase, shape);
            *writeBufferRamp++ = currentPhase * invTwoPi;

            currentPhase += phaseIncrement;
        }
        
    }
    
    void releaseResources() override {}

    AudioProcessorEditor* createEditor() override { return new ImpulseUI(*this); }
    
private:

    float currentPhase = 0.0f;
    float previousTrigger = 0.0f;
    float sampleRate = 44100.0f;
    
    bool wasExternallyTriggered()
    {
        auto triggered = params.getRawParameterValue("trigger");
        bool shouldTrigger = *triggered > 0.0f;
        triggered->exchange(0.0f);
        return shouldTrigger;
    }
};
