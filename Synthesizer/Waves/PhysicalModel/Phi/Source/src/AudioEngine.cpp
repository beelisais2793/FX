/*
  ==============================================================================

    AudioEngine.cpp
    Created: 27 Apr 2020 10:58:41pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    enableAllBuses();

    deviceManager.initialiseWithDefaultDevices (2, 2);
    deviceManager.addAudioCallback (&player);
    
    player.setProcessor (this);
    
    // Add an output node in the graph so that output modules may connect to it
    outputNode = addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
}


AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback (&player);
}


void AudioEngine::applyAudioConnections(const OwnedArray<PhiConnection>& connectionsToApply)
{
    for (auto c : getConnections())
        removeConnection(c);
    
    for (auto node : getNodes())
        if (static_cast<ModuleProcessor*>(node->getProcessor())->isOutput) // When we detect an output module, we hook it up to the output node
            connectToOuput(node);
    
    for (auto& connection : connectionsToApply)
    {
        NodeAndChannel source { {}, int(connection->source.plugID()) };
        NodeAndChannel destination { {}, int(connection->destination.plugID()) };
        
        source.nodeID.uid = connection->source.moduleID();
        destination.nodeID.uid = connection->destination.moduleID();
        
        addConnection ({ source, destination });
    }
    removeIllegalConnections();
}


void AudioEngine::connectToOuput(Node::Ptr nodeToConnect)
{
    int connectionNumber = nodeToConnect->getProcessor()->getTotalNumOutputChannels();
    
    for (int i = 0; i < connectionNumber; i++)
        addConnection ({ {nodeToConnect->nodeID, i}, {outputNode->nodeID, i} });
}
