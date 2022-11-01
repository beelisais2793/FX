/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IdealBar.h"

//==============================================================================
/**
*/
class XylophoneModelAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    XylophoneModelAudioProcessor();
    ~XylophoneModelAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float limit(float input);

    inline void handleButtonPress(int keyIndex) {
        bars[keyIndex]->strike(5);
    }
    
    std::vector<std::unique_ptr<idealBar>> bars;
    
    double octave4Lengths[12] = {0.5270, 0.5120, 0.5061, 0.6810, 0.6720, 0.6520, 0.6460, 0.6270, 0.6191, 0.6030, 0.5690, 0.5356};
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XylophoneModelAudioProcessor)
};
