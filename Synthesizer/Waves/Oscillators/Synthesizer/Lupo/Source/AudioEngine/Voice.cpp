/*
  ==============================================================================

    Voice.cpp
    Created: 4 Jun 2016 6:53:08pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "Voice.h"
#include "MultimodeOscillator.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846 
#endif

Voice::Voice(float sampleRate) {

    this->sampleRate = sampleRate;
    this->calculateFrequencyTable();
    this->playing = false;
    this->ampEnvelope = new SynthLab::ADSR();
    this->modulator = 0;
    this->pitchBend = 1;
    
    ampEnvelope->setAttackRate(0 * sampleRate);  // 1 second
    ampEnvelope->setDecayRate(1 * sampleRate);
    ampEnvelope->setReleaseRate(1 * sampleRate);
    ampEnvelope->setSustainLevel(.8);


}

Voice::~Voice() {
	if (ampEnvelope != NULL)
		delete ampEnvelope;
    
	for (int i = 0; i < 4; i++) {
        delete oscillators[i];
    }
    
}

void Voice::setNoteAndVelocity(int note, int velocity) {

	this->noteNumber = note;
	this->velocity = velocity;

	oscillators[0]->setFrequency((midiNote[noteNumber + oscillators[0]->getPitch()]) * pitchBend);
	oscillators[1]->setFrequency((midiNote[noteNumber + oscillators[1]->getPitch()]) * pitchBend);
	oscillators[2]->setFrequency((midiNote[noteNumber + oscillators[2]->getPitch()]) * pitchBend);
	oscillators[3]->setFrequency((midiNote[noteNumber + oscillators[3]->getPitch()]) * pitchBend);

	/*
    for(std::vector<Oszillator*>::iterator it = oscillators.begin(); it != oscillators.end(); ++it) {
        Oszillator* o = *it;
        
        if (note != NULL)
            o->setFrequency((midiNote[noteNumber + o->getPitch()]) * pitchBend);
    }
	*/
}



void Voice::setPitchBend(float bend) {
    this->pitchBend = bend;
	for (int i = 0; i < 4;i++) {        
        oscillators[i]->setFrequency((midiNote[noteNumber + oscillators[i]->getPitch()]) * pitchBend);
    }
}

void Voice::addOszillator(Oszillator* o, int index) {
    this->oscillators[index] = o;
}

Oszillator* Voice::getOscillator(int num) {
	return oscillators[num];
}

float Voice::process(int channel) {
    
    value = 0;
    
	if(ampEnvelope->getState() != SynthLab::ADSR::env_idle) {
        
        float amplitude = (1.0f / (float) 127) * this->velocity;

        for (int i = 0; i < 4; i++) {
            
            if (oscillators[i]->enabled) {

                if (i == 1 && oscillators[i - 1]->enabled && oscillators[i - 1]->isSync()) {
                    oscillators[i - 1]->reset();
                }

			    if (channel == 0) {
				    value += oscillators[i]->process() * cos((M_PI*(oscillators[i]->getPan() + 1) / 4)) * amplitude * ampEnvelope->process();;
			    }
			    else {
				    value += oscillators[i]->process() * sin((M_PI*(oscillators[i]->getPan() + 1) / 4)) * amplitude * ampEnvelope->process();;
			    }
			    if (modulator != nullptr) {	
				    oscillators[i]->setPitchMod(modulator->getOutput() * this->modAmount );
			    }
            }

		}
    }
    else {
        ampEnvelope->reset();
    }

    return value;    
}

void Voice::setNoteNumber(int number) {
    this->noteNumber = number;
}

int Voice::getNoteNumber() const {
    return this->noteNumber;
}

void Voice::setPitch(int number) {
    this->pitch = number;
}

int Voice::getPitch() const {
    return this->pitch;
}

void Voice::setOscVolume(int osc, float volume) {
	oscillators[osc]->setVolume(volume);
}

void Voice::setOscPan(int osc,float pan) {
	oscillators[osc]->setPan(pan);
}

void Voice::setOscSpread(int osc, float spread)
{
	MultimodeOscillator* mmo = dynamic_cast<MultimodeOscillator*>(oscillators[osc]);
	mmo->setSpread(spread);
}

void Voice::setOctave(int number) {
    this->octave = number;
	for (int i = 0; i < 4; i++) {      
        oscillators[i]->setFrequency((midiNote[noteNumber + oscillators[i]->getPitch() + this->offset + this->octave * 12]) * pitchBend);
    }
}

void Voice::setOffset(int number) {
    this->offset = number;
	for (int i = 0; i < 4; i++) {
     
		oscillators[i]->setFrequency((midiNote[noteNumber + oscillators[i]->getPitch() + this->offset + this->octave * 12]) * pitchBend);
    }
}

int Voice::getOctave() const {
    return this->octave;
}

void Voice::updateOscillator(int index) {
	if (this->noteNumber >= 0) {
	    oscillators[index]->setFrequency(midiNote[this->noteNumber + oscillators[index]->getPitch()]);
	}
}

void Voice::calculateFrequencyTable() {
    int a = 440; // a is 440 hz...
    for (int x = 0; x < 127; ++x)
    {
        midiNote[x] = a * pow(2.0,(x-69.0)/12.0);
    }
}

void Voice::setPlaying(bool playing) {
    this->playing = playing;
}

bool Voice::isPlaying() const {
    return this->playing;
}


float Voice::getSampleRate() {
    return this->sampleRate;
}

SynthLab::ADSR* Voice::getAmpEnvelope() {
    return ampEnvelope;
}

void Voice::setModulator(Modulator* modulator) {
	
	if (modulator) {
		this->modulator = modulator;
		for (int i = 0; i < 4; i++) {
			MultimodeOscillator* mmo = dynamic_cast<MultimodeOscillator*>(oscillators[i]);
			if (mmo != nullptr) {
				mmo->setModulator(modulator);
			}
		}
	}
    this->modulator = modulator;
	
}

float Voice::getTime() {
    return this->time;
}

void Voice::setTime(float time) {
    this->time = time;
}

float Voice::getDuration() {
    return this->duration;
}

void Voice::setDuration(float duration) {
    this->duration = duration;
}

void Voice::setVelocity(int velocity) {
	this->velocity = velocity;
}

int Voice::getVelocity() const {
	return this->velocity;
}
