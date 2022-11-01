/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PolyBlepOSC.h"

//==============================================================================
/**
*/
class WillPolyBlepOSCAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WillPolyBlepOSCAudioProcessor();
    ~WillPolyBlepOSCAudioProcessor() override;

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
    
    //==============================================================================
    // =============== toggle buttons =============
    void toggleOSC(bool t)
    {
        osc1.toggleOSC(t);
    };
    void toggleBlep(bool t)
    {
        osc1.toggleBlep(t);
    };
    //=============================================
    // =============== setters ====================
    void setFreq(float f)
    {
        osc1.setFreq(f);
    };
    //=============================================
    // =============== getters ====================
    float getFreq()
    {
        return osc1.getFreq();
    };
    bool getRunOSC()
    {
        return osc1.getRunOSC();
    };
    bool getBlepOSC()
    {
        return osc1.getBlepOSC();
    };
    //=============================================
private:
    
    float currentSampleRate;
    PolyBlepOSC osc1;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WillPolyBlepOSCAudioProcessor)
};

