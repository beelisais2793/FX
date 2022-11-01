#include "LFO.h"
#include <cmath>
#include <libraries/math_neon/math_neon.h>
#include <stdlib.h>

void LFO::setup(float frequency, float fs, LFO::Type type) {
    setType(type);
    invSampleRate_ = 1.0 / fs;
    setFrequency(frequency);
    phase_ = M_PI * (rand() / (float)RAND_MAX);
}

void LFO::process(unsigned int n) {
    phase_ += phaseinc_ * n;
    if (phase_ >= M_PI)
        phase_ -= 2.0f * (float)M_PI;
    switch (type_) {
    default:
    case sine:
        out_ = sinf_neon(phase_);
        break;
    case triangle:
        if (phase_ > 0) {
            out_ = -1 + (2 * phase_ / (float)M_PI);
        } else {
            out_ = -1 - (2 * phase_ / (float)M_PI);
        }
        break;
    case square:
        if (phase_ > 0) {
            out_ = 1;
        } else {
            out_ = -1;
        }
        break;
    case sawtooth:
        out_ = 1.f / (float)M_PI * phase_;
        break;
    }
}
