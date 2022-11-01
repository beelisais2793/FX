/*
  ==============================================================================

    ProcessorGraph.h
    Created: 11 May 2019 3:27:08am
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GainProcessor.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
/*
*/
class ProcessorGraph    : public AudioProcessor, public Component
{
public:

	using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
	using Node = AudioProcessorGraph::Node;

    ProcessorGraph();
    ~ProcessorGraph();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif


	//==============================================================================
	AudioProcessorEditor* createEditor() override { return new GenericAudioProcessorEditor(this);
}
	bool hasEditor() const override { return true; }

	//==============================================================================
	const String getName() const override { return "Graph Tutorial"; }
	bool acceptsMidi() const override { return true; }
	bool producesMidi() const override { return true; }
	double getTailLengthSeconds() const override { return 0; }

	//==============================================================================
	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const String getProgramName(int) override { return {}; }
	void changeProgramName(int, const String&) override {}

	//==============================================================================
	void getStateInformation(MemoryBlock&) override {}
	void setStateInformation(const void*, int) override {}



	//==============================================================================

private:

	void initialiseGraph();
	void updateGraph();
	void connectAudioNodes();
	void connectMidiNodes();

	StringArray processorChoices{ "Empty", "Synthesizer", "Gain" };

	std::unique_ptr<AudioProcessorGraph> mainProcessor;

	AudioParameterChoice* processorSlot1;
	AudioParameterChoice* processorSlot2;

	AudioParameterBool* bypassSlot1;
	AudioParameterBool* bypassSlot2;

	Node::Ptr audioInputNode;
	Node::Ptr audioOutputNode;
	Node::Ptr midiInputNode;
	Node::Ptr midiOutputNode;

	Node::Ptr slot1Node;
	Node::Ptr slot2Node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorGraph)
};
