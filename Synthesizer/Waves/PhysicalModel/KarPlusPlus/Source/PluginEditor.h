/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class KarPlusPlus2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    KarPlusPlus2AudioProcessorEditor (KarPlusPlus2AudioProcessor&);
    ~KarPlusPlus2AudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    KarPlusPlus2AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KarPlusPlus2AudioProcessorEditor)
};
