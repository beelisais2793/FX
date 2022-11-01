/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WillPolyBlepOSCAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Slider::Listener
{
public:
    WillPolyBlepOSCAudioProcessorEditor (WillPolyBlepOSCAudioProcessor&);
    ~WillPolyBlepOSCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    //==============================================================================
    void sliderValueChanged (juce::Slider *slider) override;
    void updateToggleState (juce::Button* button, juce::String name);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WillPolyBlepOSCAudioProcessor& audioProcessor;
    
    juce::Slider freqSlider;
    juce::Label freqLabel;
    
    juce::ToggleButton toggleButton;
    juce::ToggleButton blepButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WillPolyBlepOSCAudioProcessorEditor)
};

