#include "PulseDetuned.h"
#include "Utils.h"
#include <Bela.h>
#include <libraries/Biquad/Biquad.h>
#include <math.h>
#include <stdexcept>
#include <time.h>

float sprandfloat(float a, float b) {
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

PulseDetuned::PulseDetuned() {}

PulseDetuned::PulseDetuned(float frequency, float samplingRate) {
    int ret;
    if ((ret = setup(frequency, samplingRate)))
        throw std::runtime_error("OnePole: cutoff is above Nyquist");
}

int PulseDetuned::setup(float frequency, float samplingRate) {
    if (frequency < 1) {
        return -1;
    }
    _samplingRate = samplingRate;
    _frequency = frequency;
    _detuning = sprandfloat(0.002, 0.006);
    _detuningL = sprandfloat(0, _detuning);
    _detuningR = sprandfloat(0, _detuning);
    _pan = 0.0;    // [-1,1]
    _spread = 0.3; // [0,1]
    _amp = 0.1;

    Biquad::Settings settings{
        .fs = _samplingRate,
        .cutoff = _frequency,
        .type = Biquad::lowpass,
        .q = 0.707,
        .peakGainDb = 0,
    };
    lpFilter.setup(settings);

    for (unsigned int i = 0; i < 2; i++) {
        osc[i].setup(_samplingRate);
        osc[i].setAmp(_amp);
    }
    setFrequency(frequency);

    lfo[0] = LFO(sprandfloat(1.0 / 30.0, 1.0 / 10.0), _samplingRate);
    lfo[1] = LFO(sprandfloat(1.0 / 60.0, 1.0 / 10.0), _samplingRate);
    return 0;
}

void PulseDetuned::update() {}

void PulseDetuned::setAmp(float amp) { _amp = amp; }

void PulseDetuned::setFrequency(float frequency) {
    _frequency = frequency;
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        if (i == 0) {
            osc[i].setFrequency(_frequency * (1 + _detuning + _detuningL));
        } else {
            osc[i].setFrequency(_frequency * (1 - _detuning - _detuningR));
        }
    }
}

void PulseDetuned::setDetuning(float detuning) {
    _detuning = detuning;
    _detuningL = sprandfloat(0, _detuning);
    _detuningR = sprandfloat(0, _detuning);
}

void PulseDetuned::setPan(float pan) { _pan = pan; }

void PulseDetuned::process_block(unsigned int n) {
    for (unsigned int i = 0; i < NUM_LFOS; i++) {
        lfo[i].process(n);
    }
    lpFilter.setFc(
        clamp(linexp(lfo[0].val(), -1, 1, _frequency / 3, _frequency * 16), 10,
              20000));
    _panVal = clamp(linlin(lfo[1].val(), -1.0, 1.0, 0.5 - _spread / 2.0,
                           0.5 + _spread / 2.0) +
                        _pan,
                    0, 1);
}

float PulseDetuned::process(unsigned int channel) {
    if (channel > 1) {
        return 0;
    }
    float out = 0;
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        out += osc[i].process();
    }
    out = lpFilter.process(out) * _amp;
    if (channel == 0) {
        return out * _panVal;
    }
    return out * (1 - _panVal);
}
