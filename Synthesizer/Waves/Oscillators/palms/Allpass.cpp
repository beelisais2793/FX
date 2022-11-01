// A Schroeder allpass filter is given by the difference equations
// https://doc.sccode.org/Classes/AllpassN.html
// s(t) = x(t) + k * s(t - D)
// y(t) = -k * s(t) + s(t - D)

#include "Allpass.h"
#include <math.h>
#include <stdexcept>

Allpass::Allpass() {}

Allpass::Allpass(float delay, float decay, float mul, float samplingRate) {
    int ret;
    if ((ret = setup(delay, decay, mul, samplingRate)))
        throw std::runtime_error("allpass: decay is less then delay");
}

int Allpass::setup(float delay, float decay, float mul, float samplingRate) {
    if (decay < delay) {
        return -1;
    }
    _mul = mul;
    _samplingRate = samplingRate;
    _delay = delay;
    _k = pow(0.001, delay / decay);
    _D = static_cast<int>(round(_samplingRate * delay));
    _x.resize(static_cast<int>(round(_samplingRate * delay + 2)));
    _y.resize(static_cast<int>(round(_samplingRate * delay + 2)));
    // initialize to 0
    for (unsigned i = 0; i < _x.size(); i++) {
        _x.at(i) = 0;
        _y.at(i) = 0;
    }

    _t = _x.size();
    _tD = _x.size() - _D;
    return 0;
}

float Allpass::process(float input) {
    _t += 1;
    if (_t >= _x.size()) {
        _t = 0;
    }
    _x.at(_t) = input;

    _tD += 1;
    if (_tD >= _x.size()) {
        _tD = 0;
    }
    // https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html
    // y(t) = k * x(t) + x(t - D) - k * y(t - D)
    _y.at(_t) = _k * input + _x.at(_tD) - _k * _y.at(_tD);
    return _y.at(_t);
}
