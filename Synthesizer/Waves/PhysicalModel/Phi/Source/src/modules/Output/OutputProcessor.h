/*
  ==============================================================================

    OutputProcessor.h
    Created: 25 Apr 2020 12:10:44am
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "OutputUI.h"

//==============================================================================
/*
*/
class OutputProcessor    : public ModuleProcessor
{
public:
    OutputProcessor() :
    // Output modules must define the number of output channels but the UI shouldn't create outlets for them
    ModuleProcessor(2/* Inlets */, 2/* Outlets */)
    {}
    
    ~OutputProcessor() {};
    
    void prepareToPlay (double sampleRate, int maxBlockSize) override {}
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override {}
    void releaseResources() override {}
    
    AudioProcessorEditor* createEditor() override {return new OutputUI(*this);}

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputProcessor)
};
