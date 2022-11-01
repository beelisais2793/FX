/*
  ==============================================================================

    Oszillator.h
    Created: 3 Jun 2016 8:50:44pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef OSZILLATOR_H_INCLUDED
#define OSZILLATOR_H_INCLUDED



class Oszillator {

public:
    
	enum OscMode {
		SAW,
		SINE,
		PULSE,
		NOISE,
		SAMPLE
    };
    
    Oszillator();
    Oszillator(double sampleRate);
    virtual ~Oszillator();

    virtual void setFrequency(double frequency);
	float getFrequency();
    void setPhase(double phase);
    
    virtual void setVolume(float volume);
    virtual float getVolume();
    
	virtual void setPan(float pan);
	virtual float getPan();

    virtual float process() = 0;
    virtual void setPitch(int pitch);
    virtual int getPitch();
    void setPitchBend(float bend);
    float getPitchBend() const;
    
    virtual void setFine(float fine) {};
    virtual float getFine() const { return 0; };
    
    void setMode(OscMode mode);
	void setMode(float mode);

    virtual void setSync(bool sync);
    bool isSync();
    
    virtual void setSlave(Oszillator* slave);
    
    virtual void reset() = 0;

	void setPitchMod(float mod);
    bool enabled = false;
    
protected:
    
    double phase;
    float volume;
	float pan;
	float pitchMod = 0;
    double frequency;
    double sampleRate;
    double phaseIncrement;
    int pitch = 0; // halftones
    float pitchBend;
    OscMode mode;
    bool sync = false;
    Oszillator* slave = 0;

};



#endif  // OSZILLATOR_H_INCLUDED
