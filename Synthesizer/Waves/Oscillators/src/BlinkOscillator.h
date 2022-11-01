#ifndef _JL_BLINK_OSCILLATOR_H_
#define _JL_BLINK_OSCILLATOR_H_

#include "OscillatorBase.h"

class BlinkOscillator : public OscillatorBase {
private:
  float onOffRatio;

public:
  bool value;

public:
  BlinkOscillator(float f = 1, float r = 0.5) :
  OscillatorBase(f), onOffRatio(r) {
    computeValue();
  }

  ~BlinkOscillator() {}

  void setOnOffRatio(float r) {
    onOffRatio = r;
  }

  bool update() {
    if (frequency == 0) { return value; }
    updatePhase();
    computeValue();
    return value;
  }

private:
  void computeValue() {
    value = phase < onOffRatio;
  }
};

#endif /* _JL_BLINK_OSCILLATOR_H_ */
