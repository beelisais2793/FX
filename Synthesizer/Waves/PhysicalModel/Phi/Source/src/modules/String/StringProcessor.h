/*
  ==============================================================================

    StringProcessor.h
    Created: 25 Mar 2020 3:14:32pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "StringUI.h"
#include "../../DSPUtils.h"

//==============================================================================
/*
*/
class StringProcessor    : public ModuleProcessor
{
public:
    StringProcessor() :
    ModuleProcessor(2, 2,
        std::make_unique<AudioParameterFloat> (
            "freq",
            "Frequency",
            NormalisableRange<float>(20.0f, 10000.0f, 0.0f, 0.3f),
            440.0f,
            "Frequency",
            AudioParameterFloat::genericParameter,
            [](float value, int) { return String (value, 1); },
            [](const String& text) { return text.getFloatValue(); }
        ),
        std::make_unique<AudioParameterFloat> (
            "damp",
            "Damping",
            NormalisableRange<float>(0.0f, 1.0f),
            0.0f,
            "Damping",
            AudioParameterFloat::genericParameter,
            [](float value, int) { return String (floorf(value * 100.0f), 0); },
            [](const String& text) { return text.getFloatValue() * 0.01f; }
        ),
        std::make_unique<AudioParameterFloat> (
            "pos",
            "Position",
            NormalisableRange<float>(0.0f, 1.0f),
            0.0f,
            "Position",
            AudioParameterFloat::genericParameter,
            [](float value, int) { return String (floorf(value * 100.0f), 0); },
            [](const String& text) { return text.getFloatValue() * 0.01f; }
        ),
        std::make_unique<AudioParameterFloat> (
            "decay",
            "Decay",
            NormalisableRange<float>(0.0f, 1.0f),
            0.0f,
            "Decay",
            AudioParameterFloat::genericParameter,
            [](float value, int) { return String (floorf(value * 100.0f), 0); },
            [](const String& text) { return text.getFloatValue() * 0.01f; }
        ),
        std::make_unique<AudioParameterBool>  (
            "mode" ,
            "Mode"     ,
            false
        )
    )
    {}
    
    ~StringProcessor() {}
    
    void prepareToPlay (double newSampleRate, int maxBlockSize) override
    {
        sampleRate = newSampleRate;
        line1.resize(newSampleRate);
        line2.resize(newSampleRate);
        
        line1.clear();
        line2.clear();
        
        onePole1.reset();
        onePole2.reset();
        
        dcBlock1.reset();
        dcBlock2.reset();
        
        accum1.reset();
        accum2.reset();
       
    }
    
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override
    {
        const float* input1 = buffer.getReadPointer(0);
        const float* input2 = buffer.getReadPointer(1);
        
        float* out1 = buffer.getWritePointer(0);
        float* out2 = buffer.getWritePointer(1);
        
        const bool mode = *params.getRawParameterValue("mode");
        const float damp = *params.getRawParameterValue("damp");
        
        const float frequency = *params.getRawParameterValue("freq");
        const float periodInSamples = sampleRate / frequency;
        
        const float decay = powf( scaleDecay( *params.getRawParameterValue("decay"), mode), periodInSamples * (0.01f + mode * 0.52f) ) * 0.997f;
         
        // The interval to apply to each delay line
        // Mode B doubles the (perceived) interval so we must divide it accordingly
        const float interval = periodInSamples / static_cast<float>(static_cast<int>(mode) + 1 );
        
        // Pickup position is always a fraction of the interval
        const float pickupPosition = interval * *params.getRawParameterValue("pos");
        
        for (int n = 0; n < buffer.getNumSamples(); n++)
        {
            line1.push(   (*input1++)*0.05f + processLine1Node(damp, decay, interval, mode) );
            line2.push( - (*input2++)*0.05f + processLine2Node(damp, decay, interval) );
            
            *out1++ = readOutput1(interval - pickupPosition);
            *out2++ = readOutput2(pickupPosition);
        }
    }
    
    void releaseResources() override {}
    
    AudioProcessorEditor* createEditor() override {return new StringUI(*this);}

private:
    
    float sampleRate = 44100.0f;
    
    DelayLine<float> line1, line2;
    OnePole<float> onePole1, onePole2;
    DCBlock<float> dcBlock1, dcBlock2;
    Accum<float> accum1, accum2;
    
    const float processLine1Node(float damp, float decay, float interval, bool mode)
    {
        // Line 1 gets a "special" function to liven the sound up a bit...
        float line1Node = sinf(
                               onePole1.process(
                                                line1.get( interval ),
                                                damp
                                                )
                               * MathConstants<float>::twoPi * 1.5f
                               )
        * decay * (0.1063f + mode * 0.00005f); // mode needs a small adjustment
        
        // For our B mode, inverting a node changes the sound
        if ( mode )
            line1Node *= -1.0f;
        return line1Node;
    }
    
    const float processLine2Node(float damp, float decay, float interval)
    {
        // Normal case for Line 2
        // read from delay line, apply low pass and feedback (decay)
        float line2Node = onePole2.process( line2.get( interval ), damp ) * decay;
        return line2Node;
    }
    
    const float readOutput1(float pickupPosition)
    {
        // Read from the delay line with the current position, intergrate and highpass (dcblock)
        return dcBlock1.process(
                                accum1.process(
                                               line1.get( pickupPosition )
                                               )
                                );
    }
    
    const float readOutput2(float pickupPosition)
    {
        // Read from the delay line with the current position, intergrate and highpass (dcblock)
        return dcBlock2.process(
                                accum2.process(
                                               - line2.get( pickupPosition )
                                               )
                                );
    }
    const float scaleDecay(float decay, bool mode)
    {
        return powf(decay, mode ? 0.0005f : 0.05f);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringProcessor)
};

