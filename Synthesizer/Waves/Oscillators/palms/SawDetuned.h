#include "LFO.h"
#include "Saw.h"
#include <libraries/Biquad/Biquad.h>
#include <vector>

#pragma once

class SawDetuned {
  public:
    SawDetuned();
    SawDetuned(float frequency, float samplingRate);
    int setup(float frequency, float samplingRate);
    void setFrequency(float frequency);
    void setDetuning(float detuning);
    void setOffset(float offset) { _offset = offset; };
    void setPan(float pan);
    void setAmp(float amp);
    float process(unsigned int channel);
    void process_block(unsigned int n);

  private:
    static const int NUM_LFOS = 2;
    static const int NUM_OSC = 2;
    float _samplingRate;
    float _frequency;
    float _detuning, _detuningL, _detuningR, _offset;
    float _spread;
    float _pan, _panVal;
    float _amp;
    Biquad lpFilter;
    Saw osc[NUM_OSC];
    LFO lfo[NUM_LFOS];
    void update();
};
