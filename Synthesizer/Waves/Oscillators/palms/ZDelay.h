#include <math.h>
#include <vector>

#pragma once

class ZDelay {
  public:
    ZDelay();
    ZDelay(float delay, float samplingRate);
    int setup(float frequency, float samplingRate);
    float process(float input);
    void setDelay(float delay) {
        _delay = delay;
        _K = static_cast<int>(round(_samplingRate * delay));
        _j = _i - _K;
        if (_j < 0) {
            _j = buf.size() + _j;
        }
    };
    float getDelay() { return _delay; };

  private:
    float _delay, _samplingRate;
    int _K, _i, _j;
    std::vector<float> buf;
};
