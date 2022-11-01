#ifndef _JL_SEQUENCE_OSCILLATOR_H_
#define _JL_SEQUENCE_OSCILLATOR_H_

// pretty arbitrary, @todo : update from use cases
#define SEQUENCE_OSCILLATOR_MAX_SEQUENCE_LENGTH 24

#include "OscillatorBase.h"

class SequenceOscillator : public OscillatorBase {
private:
  size_t length;
  // uint32_t sequence[SEQUENCE_OSCILLATOR_MAX_SEQUENCE_LENGTH];
  size_t index;
  uint32_t sequenceAccum[SEQUENCE_OSCILLATOR_MAX_SEQUENCE_LENGTH];
  uint32_t totalDuration;
  float speedRatio;

public:
  bool value;

public:
  SequenceOscillator() :
  OscillatorBase(0),
  length(0), index(0), totalDuration(0), speedRatio(1) {
    computeValue();
  }

  ~SequenceOscillator() {}

  void setSpeedRatio(float r) {
    speedRatio = r;
    computeFrequency();
  }

  void setSequence(uint32_t *intervals, size_t n) {
    length = constrain(n, 0, SEQUENCE_OSCILLATOR_MAX_SEQUENCE_LENGTH);

    for (size_t i = 0; i < length; ++i) {
      // sequence[i] = *(intervals + i);

      sequenceAccum[i] = (i > 0) ? sequenceAccum[i - 1] : 0;
      sequenceAccum[i] += *(intervals + i);
    }

    totalDuration = (length > 0) ? sequenceAccum[length - 1] : 0;
    computeFrequency();
  }

  bool update() {
    if (frequency == 0 || length == 0 || totalDuration <= 0) { return value; }
    updatePhase();
    computeValue();
    return value;
  }


private:
  void setFrequency() { /* prevent execution of base class method */ }

  void computeFrequency() {
    float f = (totalDuration == 0) ? 0 : ((float) 1000 / totalDuration);
    OscillatorBase::setFrequency(f * speedRatio);
  }

  void computeValue() {
    float msPhase = phase * totalDuration;
    index = 0;

    while (msPhase > sequenceAccum[index]) {
      // in case length changed because sequence changed
      if (index > length - 2) {
        index = 0;
      // likeliest case
      } else {
        index++;
      }
    }

    value = index % 2 == 0;
  }
};

#endif /* _JL_SEQUENCE_OSCILLATOR_H_ */
