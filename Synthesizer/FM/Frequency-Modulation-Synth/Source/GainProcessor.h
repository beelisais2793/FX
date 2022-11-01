/*
  ==============================================================================

    GainProcessor.h
    Created: 11 May 2019 4:45:01pm
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GainProcessor : public AudioProcessor
{
public:
	GainProcessor()
	{
		gain.setGainDecibels(-6.0f);
	}

	~GainProcessor() {}

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
		gain.prepare(spec);
	}

	void releaseResources() override {}

	void processBlock(AudioSampleBuffer& buffer, MidiBuffer&) override
	{
		dsp::AudioBlock<float> block(buffer);
		dsp::ProcessContextReplacing<float> context(block);
		gain.process(context);
	}

	void reset() override
	{
		gain.reset();
	}

	const String getName() const override { return "Gain"; }

	//==============================================================================
	AudioProcessorEditor* createEditor() override { return nullptr; }
	bool hasEditor() const override { return false; }

	//==============================================================================
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0; }

	//==============================================================================
	int getNumPrograms() override { return 0; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const String getProgramName(int) override { return {}; }
	void changeProgramName(int, const String&) override {}

	//==============================================================================
	void getStateInformation(MemoryBlock&) override {}
	void setStateInformation(const void*, int) override {}

private:
	dsp::Gain<float> gain;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessor)
};