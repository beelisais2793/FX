#include "Pulse.h"
#include "math.h"

void Pulse::setup(float fs) {
    mul_ = 1.f / (float)M_PI;
    invSampleRate_ = 1.0 / fs;
    phase_ = M_PI; //* ((float)rand()/RAND_MAX);
}

float Pulse::process(float frequency) {
    frequency_ = frequency;
    return process();
}

float Pulse::process() {
    phase_ += phaseinc_;
    if (phase_ >= M_PI)
        phase_ -= 2.0f * (float)M_PI;
    if (phase_ < 0) {
        return 1.0;
    }
    return -1.0;
}
