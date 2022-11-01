/*
  ==============================================================================

    ProcessorGraph.cpp
    Created: 11 May 2019 3:27:08am
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProcessorGraph.h"

//==============================================================================
ProcessorGraph::ProcessorGraph() : AudioProcessor (BusesProperties().withInput ("Input", AudioChannelSet::stereo(), true)
																	.withOutput ("Output", AudioChannelSet::stereo(), true)),
	mainProcessor (new AudioProcessorGraph()),
	processorSlot1(new AudioParameterChoice("slot1", "Slot 1", processorChoices, 0)),
	processorSlot2(new AudioParameterChoice("slot2", "Slot 2", processorChoices, 0)),
	bypassSlot1(new AudioParameterBool("bypass1", "Bypass 1", false)),
	bypassSlot2(new AudioParameterBool("bypass2", "Bypass 2", false))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	addParameter(processorSlot1);
	addParameter(processorSlot2);

	addParameter(bypassSlot1);
	addParameter(bypassSlot2);

}

ProcessorGraph::~ProcessorGraph()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProcessorGraph::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void ProcessorGraph::prepareToPlay(double sampleRate, int samplesPerBlock) 
{
	mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
										getMainBusNumOutputChannels(),
										sampleRate, samplesPerBlock);

	mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);
	initialiseGraph();
}

void ProcessorGraph::releaseResources() 
{
	mainProcessor->releaseResources();
}

void ProcessorGraph::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	updateGraph();
	mainProcessor->processBlock(buffer, midiMessages);

}

void ProcessorGraph::initialiseGraph()
{
	mainProcessor->clear();

	audioInputNode  = mainProcessor->addNode(new AudioGraphIOProcessor(AudioGraphIOProcessor::audioInputNode));
	audioOutputNode = mainProcessor->addNode(new AudioGraphIOProcessor(AudioGraphIOProcessor::audioOutputNode));
	midiInputNode   = mainProcessor->addNode(new AudioGraphIOProcessor(AudioGraphIOProcessor::midiInputNode));
	midiOutputNode  = mainProcessor->addNode(new AudioGraphIOProcessor(AudioGraphIOProcessor::midiOutputNode));

	connectAudioNodes();
	connectMidiNodes();

}void ProcessorGraph::updateGraph()
{
	bool hasChanged = false;

	Array<AudioParameterChoice*> choices { processorSlot1, processorSlot2 };
	Array<AudioParameterBool*> bypasses  { bypassSlot1, bypassSlot2 };

	ReferenceCountedArray<Node> slots;
	slots.add(slot1Node);
	slots.add(slot2Node);

	for (int i = 0; i < 2; ++i)
	{
		auto& choice = choices.getReference(i);
		auto  slot = slots.getUnchecked(i);

		if (choice->getIndex() == 0)
		{
			if (slot != nullptr)
			{
				mainProcessor->removeNode(slot->nodeID);
				slots.set(i, nullptr);
				hasChanged = true;
			}
		}
		else if (choice->getIndex() == 1)
		{
			if (slot != nullptr)
			{
				if (slot->getProcessor()->getName() == JucePlugin_Name)
					continue;

				mainProcessor->removeNode(slot->nodeID);
			}
			auto process = mainProcessor->addNode(new FmodSynthAudioProcessor()).get();
			slots.set(i, process);
			hasChanged = true;
		}
		else if (choice->getIndex() == 2)
		{
			if (slot != nullptr)
			{
				if (slot->getProcessor()->getName() == "Gain")
					continue;

				mainProcessor->removeNode(slot->nodeID);
			}
			auto process = mainProcessor->addNode(new GainProcessor()).get();
			slots.set(i, process);
			hasChanged = true;
		}
	}

	if (hasChanged)
	{
		for (auto connection : mainProcessor->getConnections())
			mainProcessor->removeConnection(connection);

		ReferenceCountedArray<Node> activeSlots;

		for (auto slot : slots)
		{
			if (slot != nullptr)
			{
				activeSlots.add(slot);

				slot->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
					getMainBusNumOutputChannels(),
					getSampleRate(), getBlockSize());
			}
		}

		if (activeSlots.isEmpty())
		{
			connectAudioNodes();
		}
		else
		{
			for (int i = 0; i < activeSlots.size() - 1; ++i)
			{
				for (int channel = 0; channel < 2; ++channel)
					mainProcessor->addConnection({ { activeSlots.getUnchecked(i)->nodeID,      channel },
													{ activeSlots.getUnchecked(i + 1)->nodeID,  channel } });
			}

			for (int channel = 0; channel < 2; ++channel)
			{
				mainProcessor->addConnection({ { audioInputNode->nodeID,         channel },
												{ activeSlots.getFirst()->nodeID, channel } });
				mainProcessor->addConnection({ { activeSlots.getLast()->nodeID,  channel },
												{ audioOutputNode->nodeID,        channel } });
			}
		}

		connectMidiNodes();

		for (auto node : mainProcessor->getNodes())
			node->getProcessor()->enableAllBuses();
	}

	for (int i = 0; i < 2; ++i)
	{
		auto  slot = slots.getUnchecked(i);
		auto& bypass = bypasses.getReference(i);

		if (slot != nullptr)
			slot->setBypassed(bypass->get());
	}

	// audioInputNode->setBypassed(muteInput->get());

	slot1Node = slots.getUnchecked(0);
	slot2Node = slots.getUnchecked(1);
}

void ProcessorGraph::connectAudioNodes()
{
	for (int channel = 0; channel < 2; channel++)
		mainProcessor->addConnection({ {audioInputNode->nodeID, channel },
									   {audioOutputNode->nodeID, channel } });
}

void ProcessorGraph::connectMidiNodes() 
{
	mainProcessor->addConnection({ {midiInputNode->nodeID, AudioProcessorGraph::midiChannelIndex },
								   {midiOutputNode->nodeID, AudioProcessorGraph::midiChannelIndex } });
}