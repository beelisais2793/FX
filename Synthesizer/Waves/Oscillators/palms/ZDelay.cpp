#include "ZDelay.h"
#include <math.h>
#include <stdexcept>

ZDelay::ZDelay() {}

ZDelay::ZDelay(float delay, float samplingRate) {
    int ret;
    if ((ret = setup(delay, samplingRate)))
        throw std::runtime_error("delay: error");
}

int ZDelay::setup(float delay, float samplingRate) {
    if (delay < 0) {
        return -1;
    }
    _samplingRate = samplingRate;
    _delay = delay;
    _K = static_cast<int>(round(_samplingRate * delay));
    buf.resize(static_cast<int>(round(_samplingRate * delay + 2)));
    // initialize to 0
    for (unsigned i = 0; i < buf.size(); i++)
        buf.at(i) = 0;

    _i = buf.size();
    _j = buf.size() - _K;
    return 0;
}

float ZDelay::process(float input) {
    // store the current vlaue
    _i += 1;
    if (_i >= buf.size()) {
        _i = 0;
    }
    buf.at(_i) = input;

    // get the previous value
    _j += 1;
    if (_j >= buf.size()) {
        _j = 0;
    }
    return buf.at(_j);
}
