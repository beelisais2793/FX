/*
  ==============================================================================

    SynthSound.h
    Created: 18 Apr 2019 3:22:40am
    Author:  quasa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

/*
inherits from SynthesiserSound
Describes one of the sounds that a Synthesiser can play

A synthesiser can contain one or more sounds, and a sound can choose which
midi notes and channels can trigger it.

The SynthesiserSound is a passive class that just describes what the sound is -
the actual audio rendering for a sound is done by a SynthesiserVoice.This allows
more than one SynthesiserVoice to play the same sound at the same time.
*/

struct SynthSound : public SynthesiserSound
{
public:

	bool appliesToNote(int midiNoteNumber) override { return true; }
	bool appliesToChannel(int midiChannel) override { return true; }

};