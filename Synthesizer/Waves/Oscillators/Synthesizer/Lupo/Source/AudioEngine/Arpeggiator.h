/*
  ==============================================================================

    Arpeggiator.h
    Created: 28 Mar 2020 2:41:26pm
    Author:  mpue

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"

#pragma once


class Arpeggiator {

public:

	enum Mode {
		UP,
		DOWN
	};

	Arpeggiator();
	~Arpeggiator();

	void prepareToPlay(double sampleRate, int bufferSize);
	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi);
	int currentNote = 0;                        
	int lastNoteValue = -1;                    
	int time = 0;                             
	float sampleRate;

	float speed = 0.1f;
	int octaves = 1;
	int currentOctave = 0;
	Mode mode = Mode::UP;
	bool enabled = false;
	SortedSet<int> notes;
private:

};