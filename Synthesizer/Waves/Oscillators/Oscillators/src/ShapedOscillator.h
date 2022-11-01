#ifndef _JL_SHAPED_OSCILLATOR_H_
#define _JL_SHAPED_OSCILLATOR_H_

// reshape output value using pow exponent from 3 to 1/3 (MUST be > 1)
#define SHAPED_OSCILLATOR_MAX_Y_SHAPE_FACTOR 3

#include "OscillatorBase.h"

class ShapedOscillator : public OscillatorBase {
private:
  float xShape;
  float yShape;
  bool yReshape;

public:
  float value;

public:
  ShapedOscillator(float f = 1, float xs = 0, float ys = 0) :
  OscillatorBase(f), value(0) {
    setXShape(xs);
    setYShape(ys);
    computeValue();
  }

  ~ShapedOscillator() {}

  void setXShape(float s) {
    xShape = (s < -1) ? -1 : ((s > 1) ? 1 : s);
    xShape = 1 - ((xShape + 1) * 0.5);
  }

  void setYShape(float s) {
    yShape = (s < -1) ? -1 : ((s > 1) ? 1 : s);
    yReshape = abs(yShape) > 1e-6;

    float factor = abs(yShape) * (SHAPED_OSCILLATOR_MAX_Y_SHAPE_FACTOR - 1) + 1;
    yShape = (yShape < 0) ? factor : 1 / factor; // factor is always >= 1
  }

  float update() {
    if (frequency == 0) { return value; }

    updatePhase();
    computeValue();

    return value;
  }

private:
  void computeValue() {
    if (phase < xShape) {
      value = phase / xShape;
    } else {
      value = 1 - (phase - xShape) / (1 - xShape);
    }

    if (yReshape) { value = pow(value, yShape); }
  }
};

#endif /* _JL_SHAPED_OSCILLATOR_H_ */
