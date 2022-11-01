#ifndef _JL_NOISE_OSCILLATOR_H_
#define _JL_NOISE_OSCILLATOR_H_

#include "OscillatorBase.h"

class NoiseOscillator : public OscillatorBase {
private:
  float valueA;
  float valueB;
  float deltaRampPoints;

public:
  float value;

public:
  NoiseOscillator(float f = 1) :
  OscillatorBase(f), valueA(0), valueB(0) {
    updateRampPoints();
    computeValue();
  }

  ~NoiseOscillator() {}

  float update() {
    if (frequency == 0) { return value; }
    
    float prevPhase = phase;
    updatePhase();

    if (phase < prevPhase) {
      updateRampPoints();
    }
    
    computeValue();
    
    return value;
  }

private:
  void updateRampPoints() {
    valueA = valueB;
    valueB = (float) random(1000) / 999;
    deltaRampPoints = valueB - valueA;
  }

  void computeValue() {
    value = valueA + phase * deltaRampPoints;
  }
};

#endif /* _JL_NOISE_OSCILLATOR_H_ */
