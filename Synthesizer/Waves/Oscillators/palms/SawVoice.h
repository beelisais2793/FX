#include "LFO.h"
#include "PulseDetuned.h"
#include "SawDetuned.h"
#include <libraries/Biquad/Biquad.h>
#include <vector>

#pragma once

class SawVoice {
  public:
    SawVoice();
    SawVoice(float frequency, float samplingRate);
    int setup(float frequency, float samplingRate);
    void setFrequency(float frequency);
    void setNote(float note);
    void setDetuning(float detuning);
    void setAmp(float amp);
    float getAmp();
    float process(unsigned int channel);
    void process_block(unsigned int n);

  private:
    static const int NUM_LFOS = 3;
    static const int NUM_OSC = 2;
    float _samplingRate;
    float _frequency;
    float _detuning;
    float _amp;
    Biquad lpFilter;
    SawDetuned osc[NUM_OSC];
    PulseDetuned pulse;
    LFO lfo[NUM_LFOS];
    void update();
};
