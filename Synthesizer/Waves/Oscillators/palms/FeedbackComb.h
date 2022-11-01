#include <vector>

#pragma once

class FeedbackComb {
  public:
    FeedbackComb();
    FeedbackComb(float delay_time, float decay_time, float mul,
                 float samplingRate);
    int setup(float delay_time, float decay_time, float mul,
              float samplingRate);
    float process(float input);
    void setDelay(float delay_time);
    void setDecay(float decay_time);
    float getDelay();
    float getDecay();

  private:
    float _delay_time, _decay_time, _mul, _samplingRate;
    float _alpha;
    int _K, _i, _j;
    std::vector<float> buf;
};
