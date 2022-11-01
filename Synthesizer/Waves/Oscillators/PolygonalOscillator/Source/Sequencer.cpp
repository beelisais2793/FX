

#include "Sequencer.h"

Sequencer::Sequencer()
: tempoRange(Range<int>(30, 360)), tempo(0)
{

}

Sequencer::~Sequencer()
{

}

//==============================================================================

void Sequencer::updateCounter()
{
	msCounter = Time::getMillisecondCounter();
}

bool Sequencer::tick()
{
    bool trig = false;
	int interval = 60000 / tempo; // BPM to IOI

	if(fmod(msCounter, interval) < mod) // if the remainder is smaller then it was
        trig = true;
    mod = fmod(msCounter, interval); // update stored remainder
    
    return trig;
}

void Sequencer::calculateTempo(const float& normCoordX)
{
    mod = 0;
	tempo = normCoordX * (tempoRange.getEnd()-tempoRange.getStart()) + tempoRange.getStart();
}
