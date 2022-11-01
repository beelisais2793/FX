/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Synth.h"


//==============================================================================
/**
*/
class StringSynthesisKarplusAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    StringSynthesisKarplusAudioProcessorEditor (StringSynthesisKarplusAudioProcessor&);
    ~StringSynthesisKarplusAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider*) override;

private:
    
    ScopedPointer<Slider> m_lopSlider, m_attackSlider, m_decaySlider, m_feedbackSlider;
    ScopedPointer<Label> m_lopLabel, m_attackLabel, m_decayLabel, m_feedbackLabel;
    float m_sliderSize, m_sliderY, m_sliderX, m_spacing;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StringSynthesisKarplusAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringSynthesisKarplusAudioProcessorEditor)
};
