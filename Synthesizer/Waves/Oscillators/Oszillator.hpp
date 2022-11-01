class Oszillator {

public:
    
    enum OscMode {
        SAW,
        SINE,
        PULSE,
        NOISE
    };
    
    Oszillator();
    Oszillator(double sampleRate);
    virtual ~Oszillator();

    virtual void setFrequency(double frequency);
    void setPhase(double phase);
    
    virtual void setVolume(float volume);
    virtual float getVolume();
    
    virtual float process() = 0;
    void setPitch(int pitch);
    int getPitch() const;
    void setPitchBend(float bend);
    float getPitchBend() const;
    
    virtual void setFine(float fine) {};
    virtual float getFine() const { return 0; };
    
    void setMode(OscMode mode);
    
    virtual void setSync(bool sync);
    bool isSync();
    
    virtual void setSlave(Oszillator* slave);
    
    virtual void reset() = 0;
    
protected:
    
    double phase;
    float volume;
    double frequency;
    double sampleRate;
    double phaseIncrement;
    int pitch; // halftones
    float pitchBend;
    OscMode mode;
    bool sync = false;
    Oszillator* slave = 0;

};

Oszillator::Oszillator() {
    this->pitch = 0;
}

Oszillator::Oszillator(double sampleRate) {
    this->sampleRate = sampleRate;
}

Oszillator::~Oszillator() {
    
}

void Oszillator::setPhase(double phase) {
    this->phase = phase;
}

void Oszillator::setVolume(float volume) {
    this->volume = volume;
}

float Oszillator::getVolume() {
    return volume;
}

void Oszillator::setFrequency(double frequency) {
    this->frequency = frequency;
}

void Oszillator::setPitch(int pitch) {
    this->pitch = pitch;
}

int Oszillator::getPitch() const {
    return this->pitch;
}

void Oszillator::setPitchBend(float bend) {
    this->pitchBend = bend;
}

float Oszillator::getPitchBend() const {
    return this->pitchBend;
}

void Oszillator::setMode(OscMode mode) {
    this->mode = mode;
}

void Oszillator::setSync(bool sync) {
    this->sync = sync;
}

bool Oszillator::isSync() {
    return sync;
}

void Oszillator::setSlave(Oszillator *slave) {
    this->slave = slave;
}


#include <stdio.h>
#include "Oszillator.h"
#include "Modulator.h"

class Sine : public Oszillator , public Modulator {
    
    
public:
    Sine(float sampleRate, int buffersize);
    virtual float process() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    virtual void setFrequency(double frequency) override;
    virtual float getOutput() override;
    virtual void reset() override;
    
private:
    float fine;
    double step;
    double phase;
    float value;
};


#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

Sine::Sine(float sampleRate, int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->step = (frequency * 2 * M_PI) / sampleRate;
    this->phase = 0;
	this->value = 0;
}

float Sine::getOutput() {
    return this->value;
}

void Sine::reset() {
    this->phase = 0;
}

float Sine::process() {
    
    phase += step;

    this->value = (float)(sin(phase) * this->volume);
    return this->value;
}

void Sine::setFrequency(double frequency) {
    this->frequency = frequency;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

void Sine::setFine(float fine) {
    this->fine = fine;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

float Sine::getFine() const {
    return this->fine;
}


#include <stdio.h>
#include "Oszillator.h"
#include "Modulator.h"

class Pulse : public Oszillator , public Modulator {
    
    
public:
    Pulse(float sampleRate, int buffersize);
    virtual float process() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    virtual void setFrequency(double frequency) override;
    virtual float getOutput() override;
    virtual void reset() override;
private:
    float fine;
    double step;
    double phase;
    double value;
    bool on = false;
};

#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

Pulse::Pulse(float sampleRate, int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->step = (frequency * 2 * M_PI) / sampleRate;
    this->phase = 0;
}

void Pulse::reset() {
    this->phase = 0;
}

float Pulse::getOutput() {
    return this->value;
}

float Pulse::process() {
    
    phase += step;
    
    this->value = (float)(sin(phase));
    
    if (value < 0) {
        value = -1;
    }
    else {
        value = 1;
    }

    return this->value * volume;
}

void Pulse::setFrequency(double frequency) {
    this->frequency = frequency;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

void Pulse::setFine(float fine) {
    this->fine = fine;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

float Pulse::getFine() const {
    return this->fine;
}


#include "Oszillator.h"
#include "Modulator.h"

class Sawtooth : public Oszillator, public Modulator {
    
public:
    virtual float process() override;
    virtual float getOutput() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    
    Sawtooth(float sampleRate, int buffersize);
    float fine;
    virtual void setFrequency(double frequency) override;
    virtual void reset() override;
    
    
    float p;      //current position
    float dp;     //change in postion per sample
    float pmax;   //maximum position
    float x;      //position in sinc function
    float leak;   //leaky integrator
    float dc;     //dc offset
    float saw;    //output
    float lastValue;
};


#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

using namespace std;

Sawtooth::Sawtooth(float sampleRate, int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->p = 0.0f;      //current position
    this->dp = 1.0f;     //change in postion per sample
    this->leak = 0.995f; //leaky integrator
    this->pmax = 0.5f * sampleRate / (frequency + this->fine);
    this->dc = -0.498f / this->pmax;
    this->lastValue = 0;
	this->saw = 0;
}

float Sawtooth::getOutput() {
    return this->saw;
}

void Sawtooth::reset() {
    this->p = 0;
}


float Sawtooth::process() {
    
    p += dp;
    
    if(p < 0.0f)
    {
        p = -p;
        dp = -dp;
    }
    else if(p > pmax)
    {
        p = pmax + pmax - p;
        dp = -dp;
        
        if (this->slave != 0 && sync) {
            slave->reset();
        }
    }
    
    x = M_PI * p;
    
    if(x < 0.00001f)
        x=0.00001f; //don't divide by 0
    
    saw = leak * saw + dc + (float)sin(x)/(x);
    
    if (saw > 1.0f)
        saw = lastValue;
    else
        lastValue = saw;
        
    return saw * this->volume;
}

void Sawtooth::setFrequency(double frequency) {
    this->frequency = frequency;
    pmax = 0.5f * sampleRate / (frequency + this->fine);
    dc = -0.498f / pmax;
    
}

void Sawtooth::setFine(float fine) {
    this->fine = fine;
    setFrequency(frequency + fine);
}

float Sawtooth::getFine() const {
    return this->fine;
}

class WhiteNoise : public Oszillator, public Modulator {
    
public:
    
    WhiteNoise(double sampleRate, int buffersize);
    ~WhiteNoise();
    
    virtual float process() override;
    virtual float getOutput() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    
    virtual void reset() override;
    
    float fine;
    
private:
    juce::ScopedPointer<juce::Random> random;
    
};

WhiteNoise::WhiteNoise(double sampleRate,  int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    random = new Random();
    this->fine = 0.0f;
}

WhiteNoise::~WhiteNoise() {
    random = nullptr;
}

void WhiteNoise::reset() {
}

float WhiteNoise::process() {
    return (random->nextFloat() * 0.25f - 0.125f) * volume;
}

float WhiteNoise::getOutput() {
    return this->process();
}

void WhiteNoise::setFine(float fine) {
    this->fine = fine;
}

float WhiteNoise::getFine() const {
    return this->fine;
}
class MultimodeOscillator : public Oszillator, public Modulator, public ModTarget {
    
    
public:
    MultimodeOscillator(float sampleRate, int buffersize);
    ~MultimodeOscillator();
    virtual float process() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    virtual void setFrequency(double frequency) override;
    virtual void setVolume(float volume) override;
    virtual float getOutput() override;
    virtual void setModulator(Modulator* mod) override;
    virtual void setModAmount(float amount) override;
    virtual void reset() override;
    virtual void setSlave(Oszillator* slave) override;
    virtual void setSync(bool sync) override;
    
private:
    float fine;
    Sawtooth* saw;
    Sine* sine;
    Pulse* pulse;
    WhiteNoise* noise;
    float modAmount;
    Modulator* modulator;
};

MultimodeOscillator::MultimodeOscillator(float sampleRate,int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    
    this->sine = new Sine(sampleRate, buffersize);
    this->saw = new Sawtooth(sampleRate, buffersize);
    this->pulse = new Pulse(sampleRate, buffersize);
    this->noise = new WhiteNoise(sampleRate, buffersize);
    this->mode = OscMode::SAW;
    this->modulator = NULL;
}

MultimodeOscillator::~MultimodeOscillator() {
    delete sine;
    delete saw;
    delete pulse;
    delete noise;
}

void MultimodeOscillator::setFrequency(double frequency) {
    this->saw->setFrequency(frequency);
    this->sine->setFrequency(frequency);
    this->pulse->setFrequency(frequency);
}

void MultimodeOscillator::setVolume(float volume) {
    this->saw->setVolume(volume);
    this->sine->setVolume(volume);
    this->pulse->setVolume(volume);
    this->noise->setVolume(volume);
}

void MultimodeOscillator::setModulator(Modulator* mod) {
    this->modulator = mod;
}

void MultimodeOscillator::setModAmount(float amount) {
    this->modAmount = amount;
}

float MultimodeOscillator::getOutput() {
    if (this->mode == SAW) {
        return this->saw->getOutput();
    }
    else if (this->mode == SINE) {
        return this->sine->getOutput();
    }
    else if (this->mode == PULSE) {
        return this->pulse->getOutput();
    }
    else if(this->mode == NOISE) {
        return this->noise->getOutput();
    }
    else {
        return 0;
    }
}

float MultimodeOscillator::process() {
    
    if (this->modulator != NULL) {
        setFine(modulator->getOutput() * 10 * modAmount);
    }
    
    if (this->mode == SAW) {
        return this->saw->process();
    }
    else if (this->mode == SINE) {
        return this->sine->process();
    }
    else if (this->mode == PULSE) {
        return this->pulse->process();
    }
    else if(this->mode == NOISE) {
        return this->noise->process();
    }
    else {
        return 0;
    }
    
}

void MultimodeOscillator::reset() {
    this->saw->reset();
    this->pulse->reset();
    this->sine->reset();
}

void MultimodeOscillator::setFine(float fine) {
    this->fine = fine;
    saw->setFine(fine);
    sine->setFine(fine);
    pulse->setFine(fine);
}

float MultimodeOscillator::getFine() const {
    return this->fine;
}

void MultimodeOscillator::setSlave(Oszillator *slave) {
    this->saw->setSlave(slave);
}

void MultimodeOscillator::setSync(bool sync) {
    this->saw->setSync(sync);
}


class WaveTable : public Oszillator , public Modulator {
    
    
public:
    WaveTable(float sampleRate, int buffersize);
    virtual float process() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    virtual void setFrequency(double frequency) override;
    virtual float getOutput() override;
    virtual void reset() override;
private:
    float fine;
    double step;
    double phase;
    double value;
    bool on = false;
};


#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

WaveTable::WaveTable(float sampleRate, int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->step = (frequency * 2 * M_PI) / sampleRate;
    this->phase = 0;
}

void WaveTable::reset() {
    this->phase = 0;
}

float WaveTable::getOutput() {
    return this->value;
}

float WaveTable::process() {
    
    phase += step;
    

    
    return this->value * volume;
}

void WaveTable::setFrequency(double frequency) {
    this->frequency = frequency;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

void WaveTable::setFine(float fine) {
    this->fine = fine;
    this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

float WaveTable::getFine() const {
    return this->fine;
}