#include "math.h"
#pragma once

class LFO {
  public:
    typedef enum {
        sine,
        triangle,
        square,
        sawtooth,
        numOscTypes,
    } Type;

    LFO(){};
    LFO(float frequency, float fs, LFO::Type type = sine) {
        setup(frequency, fs, type);
    }
    ~LFO(){};

    void setup(float frequency, float fs, LFO::Type type = sine);

    void process(unsigned int n);
    void setType(LFO::Type type) { type_ = type; }
    void setFrequency(float frequency) {
        frequency_ = frequency;
        phaseinc_ = 2.0f * (float)M_PI * frequency_ * invSampleRate_;
    }
    void setPhase(float phase) { phase_ = phase; }
    float val() { return out_; }
    float linlin(float slo, float shi, float dlo, float dhi);

    float getPhase() { return phase_; }
    float getFrequency() { return frequency_; }
    int getType() { return type_; }

  private:
    float out_;
    float phase_, phaseinc_;
    float frequency_;
    float invSampleRate_;
    unsigned int type_ = sine;
};
