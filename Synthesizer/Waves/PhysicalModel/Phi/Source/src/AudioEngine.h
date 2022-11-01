/*
  ==============================================================================

    AudioEngine.h
    Created: 25 Apr 2020 10:58:41pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

///@cond
#include <JuceHeader.h>
///@endcond
#include "Connections.h"
#include "ModuleProcessor.h"

/// The class where each module's DSP routine gets implemented as nodes and patched together
class AudioEngine : public AudioProcessorGraph
{
public:
    AudioEngine();

    ~AudioEngine();
   
   /// Takes an array of Connections::PlugID pairs (output, input) and iterates through it to apply the connections in the AudioProcessorGraph
    void applyAudioConnections(const OwnedArray<PhiConnection>&);
   
   /** Connects all the outlets of a node to the output node.
    This function should only be called on modules that are meant as an audio output to the patcher.
    Its use however, still allows for the outlets to be connected to other modules in the patcher, if they are made available */
    void connectToOuput(Node::Ptr);

private:
    
    /// Interfaces with output devices
    AudioDeviceManager deviceManager;
    /// Allows to playback our Processor Graph
    AudioProcessorPlayer player;
    /// A constant output node to plug output modules to
    Node::Ptr outputNode;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioEngine)
};
