#include "SawVoice.h"
#include "Utils.h"
#include <libraries/Biquad/Biquad.h>
#include <math.h>
#include <stdexcept>
#include <time.h>

SawVoice::SawVoice() {}

SawVoice::SawVoice(float frequency, float samplingRate) {
    int ret;
    if ((ret = setup(frequency, samplingRate)))
        throw std::runtime_error("OnePole: cutoff is above Nyquist");
}

int SawVoice::setup(float frequency, float samplingRate) {
    if (frequency < 1) {
        return -1;
    }
    _samplingRate = samplingRate;
    _frequency = frequency;
    _detuning = randfloat(0.002, 0.003);
    _amp = 1.0;
    _amp = linlin(freq_to_midi(frequency), 0, 128, 2.0, 0);
    _amp = clamp(_amp, 0, 1);

    Biquad::Settings settings{
        .fs = _samplingRate,
        .cutoff = _frequency,
        .type = Biquad::lowpass,
        .q = randfloat(0.303, 0.808),
        .peakGainDb = 0,
    };
    lpFilter.setup(settings);

    float pulse_frequency = _frequency;
    float offset = randfloat(-0.002, 0.002);
    while (pulse_frequency > 68.0) {
        pulse_frequency = pulse_frequency / 2.0;
    }
    pulse = PulseDetuned(pulse_frequency, _samplingRate);
    pulse.setAmp(_amp);
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        osc[i] = SawDetuned(_frequency / pow(2.0, i * 1.0), _samplingRate);
        osc[i].setDetuning(_detuning);
        osc[i].setAmp(_amp);
        osc[i].setOffset(offset);
    }

    lfo[0] = LFO(randfloat(1.0 / 30.0, 1.0 / 10.0), _samplingRate);
    lfo[1] = LFO(randfloat(1.0 / 30.0, 1.0 / 10.0), _samplingRate);
    lfo[2] = LFO(randfloat(1.0 / 30.0, 1.0 / 10.0), _samplingRate);
    return 0;
}

float SawVoice::getAmp() { return _amp; }
void SawVoice::setAmp(float amp) {
    _amp = amp;
    pulse.setAmp(_amp);
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        osc[i].setAmp(_amp);
    }
}

void SawVoice::setNote(float note) { setFrequency(midi_to_freq(note) / 2.0); }

void SawVoice::setFrequency(float frequency) {
    _frequency = frequency;
    pulse.setFrequency(frequency / 2.0);
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        osc[i].setFrequency(_frequency);
    }
}

void SawVoice::setDetuning(float detuning) {
    _detuning = detuning;
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        osc[i].setDetuning(_detuning);
    }
}

void SawVoice::process_block(unsigned int n) {
    for (unsigned int i = 0; i < NUM_LFOS; i++) {
        lfo[i].process(n);
    }
    float pan = linlin(lfo[2].val(), -1, 1, -0.5, 0.5);
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        osc[i].setPan(pan);
        osc[i].process_block(n);
    }
    lpFilter.setFc(linexp(lfo[1].val(), -1, 1, 100, 18000));
    lfo[1].setFrequency(linlin(lfo[0].val(), -1, 1, 0.03, 0.1));
}

float SawVoice::process(unsigned int channel) {
    if (channel > 1) {
        return 0;
    }
    float out = 0.0;
    // out += pulse.process(channel);
    for (unsigned int i = 0; i < NUM_OSC; i++) {
        out += osc[i].process(channel);
    }
    out = lpFilter.process(out) * _amp;
    return out;
}
