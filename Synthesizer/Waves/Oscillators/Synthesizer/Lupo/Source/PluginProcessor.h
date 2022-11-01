/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioEngine/LupoSynth.h"
#include "Model.h"
#include "ModMatrix.h"
class AttachmentFactory;
//==============================================================================
/**
*/
class LupoAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    LupoAudioProcessor();
    ~LupoAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
	AudioProcessorParameter* getBypassParameter() const override;
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

	void setSelectedProgram(String name);

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	AudioProcessorValueTreeState* getValueTreeState();
	AudioProcessorValueTreeState* parameters = nullptr;

	AttachmentFactory* getFactory() {
		return factory;
	}

	AttachmentFactory* factory = nullptr;
	String selectedProgram = "";
	bool prepared = false;
	LupoSynth* getSynth();
	Model* getModel();
	ModMatrix* matrix;
private:
    //==============================================================================

	std::unique_ptr<LupoSynth> lupo = nullptr;
	std::unique_ptr<Model> model = nullptr;

	AudioProcessorParameter* bypass = nullptr;
	FileLogger* fileLogger = nullptr;

	vector<String> programNames;
	int currentProgramNumber = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LupoAudioProcessor)
};
