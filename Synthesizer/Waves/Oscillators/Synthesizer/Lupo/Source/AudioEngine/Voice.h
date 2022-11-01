/*
  ==============================================================================

    Voice.h
    Created: 4 Jun 2016 6:53:13pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#include <vector>

#include "Oszillator.h"
#include "Sawtooth.h"
#include "Modulator.h"
#include "Note.h"
#include "ADSR.h"
#include "ModTarget.h"
#include "Sampler.h"
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class Voice : public ModTarget {
    
public:
        
    Voice(float sampleRate);
    ~Voice();
    
    void addOszillator(Oszillator* o, int index);
    Oszillator* getOscillator(int num);
    
    float process(int channel);
    
    void setNoteNumber(int number);
    int getNoteNumber() const;
    void setOctave(int number);
    int getOctave() const;
    
    void setOffset(int number);
    
    void setPitch(int number);
    int getPitch() const;
    void updateOscillator(int index);
    
    void setPlaying(bool playing);
    bool isPlaying() const;

    virtual void setModulator(Modulator* mod) override;
    
	SynthLab::ADSR* getAmpEnvelope();

    float getSampleRate();
    
    void setPitchBend(float bend);
    
    void setDuration(float duration);
    float getDuration();
    void setTime(float time);
    float getTime();
    
	void setVelocity(int velocity);
	int getVelocity() const;
	void setNoteAndVelocity(int note, int velocity);

	void setOscVolume(int osc, float volume);
	void setOscPan(int osc, float pan);
	void setOscSpread(int osc, float spread);

private:
    Oszillator* oscillators[4];
    float sampleRate;
    int noteNumber = 0;
    int pitch = 0;
    int velocity;
    double midiNote[128];
    void calculateFrequencyTable();
    bool playing;
	SynthLab::ADSR* ampEnvelope;
	Modulator* modulator = nullptr;
    float pitchBend;
    float duration;
    float time;
    int octave = 0;
    int offset = 0;
	float value = 0;
    float modAmount = 0;
	
};



#endif  // VOICE_H_INCLUDED
