#ifndef _JL_OSCILLATOR_BASE_H_
#define _JL_OSCILLATOR_BASE_H_

#include <Arduino.h>

class OscillatorBase {
private:
  float msPeriod;
  uint32_t lastDate;

protected:
  float frequency;
  float phase;

public:
  OscillatorBase(float f = 1) :
  phase(0) {
    setFrequency(f);
    lastDate = millis();
  }

  virtual ~OscillatorBase() {}

  virtual void setFrequency(float f) {
    frequency = f;
    msPeriod = 1000 / frequency;
  }

  // virtual float update() = 0;

protected:
  void updatePhase() {
    uint32_t now = millis();
    uint32_t dt = now - lastDate;
    lastDate = now;

    phase += (dt / msPeriod);
    phase -= floor(phase);
  }
};

#endif /* _JL_OSCILLATOR_BASE_H_ */
