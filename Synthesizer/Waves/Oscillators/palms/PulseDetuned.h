#include "LFO.h"
#include "Pulse.h"
#include <libraries/Biquad/Biquad.h>
#include <vector>

#pragma once

class PulseDetuned {
  public:
    PulseDetuned();
    PulseDetuned(float frequency, float samplingRate);
    int setup(float frequency, float samplingRate);
    void setFrequency(float frequency);
    void setDetuning(float detuning);
    void setPan(float pan);
    void setAmp(float amp);
    float process(unsigned int channel);
    void process_block(unsigned int n);

  private:
    static const int NUM_LFOS = 2;
    static const int NUM_OSC = 2;
    float _samplingRate;
    float _frequency;
    float _detuning, _detuningL, _detuningR;
    float _spread;
    float _pan, _panVal;
    float _amp;
    Biquad lpFilter;
    Pulse osc[NUM_OSC];
    LFO lfo[NUM_LFOS];
    void update();
};
