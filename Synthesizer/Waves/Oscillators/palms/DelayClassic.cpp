#include "DelayClassic.h"
#include <cmath>
#include <libraries/math_neon/math_neon.h>
#include <math.h>
#include <stdlib.h>

void DelayClassic::setup(float max_delay_time, float delay_time,
                         float decay_time, float max_offset, float offset,
                         float fs) {
    _max_delay_time = delay_time;
    _delay_time = delay_time;
    _decay_time = decay_time;
    _offset = offset;
    _interp_inc = fs * 0.15; // 150 milliseconds
    for (unsigned int i = 0; i < 2; i++) {
        comb[i] = FeedbackComb(max_delay_time, decay_time, 1.0, fs);
        delay[i] = ZDelay(max_offset, fs);
        comb[i].setDelay(delay_time);
        delay[i].setDelay(offset);
    }
}

void DelayClassic::process_block(unsigned int n) {
    if (_interp > 0) {
        return;
    }
    if (delay[_i].getDelay() != _delay_time ||
        comb[_i].getDecay() != _decay_time ||
        comb[_i].getDelay() != _delay_time) {
        if (_delay_time < _max_delay_time) {
            // TODO: update the other comb+delay and interpolate to it
            _interp += 0.0000001;
            comb[1 - _i].setDelay(_delay_time);
            comb[1 - _i].setDecay(_decay_time);
            delay[1 - _i].setDelay(_offset);
        }
    }
}

float DelayClassic::process(float input) {
    if (_interp > 0) {
        _interp += _interp_inc;
        if (_interp < 1) {
            // interpolate between the two
            return cos(half_pi * _interp) *
                       delay[_i].process(comb[_i].process(input)) +
                   cos(half_pi * (1 - _interp)) *
                       delay[1 - _i].process(comb[1 - _i].process(input));
        } else {
            _interp = 0;
            _i = 1 - _i;
        }
    }
    return delay[_i].process(comb[_i].process(input));
}
