/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OldSchoolLaF.h"
// add this so I don't have to scope the juce stuff everytime
using namespace juce;

//==============================================================================
/**
*/
class NoiseGeneratorPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    // Radio button IDs for the white/pink/off radio buttons
    enum RadioButtonIds
    {
        NoiseButtons = 1001
    };

    NoiseGeneratorPluginAudioProcessorEditor(NoiseGeneratorPluginAudioProcessor&);
    ~NoiseGeneratorPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void updateToggleState(Button* button, String name);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NoiseGeneratorPluginAudioProcessor& audioProcessor;

    // look and feel class declaration
    OldScoolLaF  oldSchoolLookAndFeel;
    // GUI object declarations
    TextButton wButton;
    TextButton pButton;
    TextButton bButton;
    TextButton onButton;
    TextButton dcButton;
    TextButton avgButton;
    Slider levelSlider;
    Slider dcSlider;
    Slider avgSlider;
    Label titleLabel;
    Label levelLabel;

    // set OldSchoolLookAndFeel colours here
    void setupOldSchoolAndFeelColours(LookAndFeel& laf);

public:
    //  std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment > sliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> wAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> pAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> bAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> onAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> dcAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> avgAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> levelAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> dcSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> avgSliderAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGeneratorPluginAudioProcessorEditor)
};
