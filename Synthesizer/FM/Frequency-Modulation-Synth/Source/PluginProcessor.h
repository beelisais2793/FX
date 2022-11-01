/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "SynthVoice.h"
#include "maximilian.h"
#include "ModMatrix.h"

//==============================================================================
/**
*/
class FmodSynthAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FmodSynthAudioProcessor();
    ~FmodSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	float attackTime;
	AudioProcessorValueTreeState tree;

	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

	// dsp
	void process(dsp::ProcessContextReplacing<float> context);
	void updateParameters();
	void updateFilter();

private:

	SynthVoice* voice;
	Synthesiser synth;
	ModMatrix* mod;

	AudioParameterFloat* synthGainParameter;

	dsp::ProcessorDuplicator<dsp::StateVariableFilter::Filter<float>, dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;

	// used in preparetoplay()
	double lastSampleRate;
	double lfoValue;



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FmodSynthAudioProcessor)
};