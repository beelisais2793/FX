// a simple inline one-pole (I1P) filter
// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/
// DC blocking:
// I1P *dcBlock = new I1P(10.0 / sampleRate);
// sample -= dcBlock->process(sample);

#ifndef I1P_h
#define I1P_h

#include <math.h>

class I1P {
  public:
    I1P() {
        a0 = 1.0;
        b1 = 0.0;
        z1 = 0.0;
    };
    I1P(double Fc) {
        z1 = 0.0;
        setFc(Fc);
    };
    ~I1P();
    void setFc(double Fc);
    float process(float in);

  protected:
    double a0, b1, z1;
};

inline void I1P::setFc(double Fc) {
    b1 = exp(-2.0 * M_PI * Fc);
    a0 = 1.0 - b1;
}

inline float I1P::process(float in) { return z1 = in * a0 + z1 * b1; }

#endif
