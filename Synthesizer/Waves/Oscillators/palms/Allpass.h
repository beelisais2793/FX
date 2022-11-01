#include <vector>

#pragma once

class Allpass {
  public:
    Allpass();
    Allpass(float delay, float decay, float mul, float samplingRate);
    int setup(float frequency, float decay, float mul, float samplingRate);
    float process(float input);

  private:
    float _delay, _mul, _samplingRate, _k;
    int _D, _t, _tD;
    std::vector<float> _y;
    std::vector<float> _x;
};
