/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "NoiseSource.h"
// defines for consistent IDs and names
// BUTTONS
#define WHITE_ID    "white"
#define WHITE_NAME  "White Noise"
#define PINK_ID     "pink"
#define PINK_NAME   "Pink Noise"
#define BROWN_ID    "brown"
#define BROWN_NAME  "Brown Noise"
#define DC_ID       "dc"
#define DC_NAME     "DC Blocking Filter"
#define AVG_ID      "avg"
#define AVG_NAME    "Moving Average Filter"
#define STATE_ID    "state"
#define STATE_NAME  "On/Off"
// SLIDERS
#define LEVEL_ID        "level"
#define LEVEL_NAME      "Level"
#define DC_SLIDER_ID    "dc_slider"
#define DC_SLIDER_NAME  "DC Filter Constant"
#define AVG_SLIDER_ID   "avg_slider"
#define AVG_SLIDER_NAME "Smooth Length"

//==============================================================================
/**
*/
class NoiseGeneratorPluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    NoiseGeneratorPluginAudioProcessor();
    ~NoiseGeneratorPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //juce::AudioProcessorValueTreeState treeState;
    juce::AudioProcessorValueTreeState treeState;
    
private:
    // user-adjustable filter parameters
    float dcFilterRatio = 0.99;
    int   smoothLength = 4;
    
    // noise classses
    juce::Random random;
    PinkNoise nP;
    BrownNoise nB;
    NoiseFilter filterWhite, filterPink, filterBrown; // one for each noise source
    //NoiseFilter filterWhite(dcFilterRatio, smoothLength); //, filterPink, filterBrown; // one for each noise source
    //NoiseFilter(dcFilterRatio, smoothLength) filterWhite;
    //NoiseFilter filterWhite(0.99, 4);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGeneratorPluginAudioProcessor)
};
