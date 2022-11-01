/*
  ==============================================================================

    Triggering envelopes according to given intervals or sequence.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Sequencer
{
public:

	Sequencer();
	~Sequencer();

	void updateCounter(); // updates the internal counter
	bool tick(); // returns ticks according to set tempo
	void calculateTempo(const float& normCoordX);

private:

	uint32 msCounter; // millisecond counter
	int mod; // remainder from modulo
	int tempo;
	Range<int> tempoRange;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer);
};
