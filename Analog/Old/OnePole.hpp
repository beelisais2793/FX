#pragma once

#include <cmath>

class OnePole {
public:
    OnePole() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePole(double Fc) {z1 = 0.0; setFc(Fc);};
    ~OnePole() = default;

    void setFc(double Fc);
    void setHighPass(double Fc);
    float process(float in);

protected:
    double a0, b1, z1;
};

// low pass
inline void OnePole::setFc(double Fc) {
    b1 = exp(-2.0 * M_PI * Fc);
    a0 = 1.0 - b1;
}

inline void OnePole::setHighPass(double Fc) {
    b1 = -exp(-2.0 * M_PI * (0.5 - Fc));
    a0 = 1.0 + b1;
}

inline float OnePole::process(float in) {
    return z1 = in * a0 + z1 * b1;
}

