//
//  OnePole.h
//

#ifndef OnePole_h
#define OnePole_h

#include <math.h>

class OnePole {
public:
    
    OnePole() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePole(double Fc, float sample_rate=44100) { 
        z1 = 0.0; 
        setFc(Fc);
        sr = sample_rate;
    };
    ~OnePole();
    
    void  setLowPassFc(double Fc) {
        b1 = exp(-2.0 * M_PI * Fc/sr);
        a0 = 1.0 - b1;
    }
    void  setHighPassFc(double Fc) {
        b1 = -exp(-2.0 * M_PI * (0.5 - Fc/sr));
        a0 = 1.0 + b1;
    }
    void setDCBlocker(float f = 10.0) {
        setLowPass(f/sampleRate);
    }
    float process(float in)
    {
        x = in;
        return z1 = in * a0 + z1 * b1;
    }
    float hi() {
        return x - z1;
    }
    
protected:    
    double x;
    double a0, b1, z1;
    double sr;
};


