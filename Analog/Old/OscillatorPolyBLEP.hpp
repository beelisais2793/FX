#pragma once
// modified by macho charlie 1993 

#include "SoundObject.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

const double TWO_PI = 2 * M_PI;

template<typename T>
inline T square_number(const T &x) {
    return x * x;
}

// Adapted from "Phaseshaping Oscillator Algorithms for Musical Sound
// Synthesis" by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa
// Valimaki.
// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
inline double blep(double t, double dt) {
    if (t < dt) {
        return -square_number(t / dt - 1);
    } else if (t > 1 - dt) {
        return square_number((t - 1) / dt + 1);
    } else {
        return 0;
    }
}

// Derived from blep().
inline double blamp(double t, double dt) {
    if (t < dt) {
        t = t / dt - 1;
        return -1 / 3.0 * square_number(t) * t;
    } else if (t > 1 - dt) {
        t = (t - 1) / dt + 1;
        return 1 / 3.0 * square_number(t) * t;
    } else {
        return 0;
    }
}

template<typename T>
inline int64_t bitwiseOrZero(const T &t) {
    return static_cast<int64_t>(t) | 0;
}

struct SW_PolyBLEP : public OscillatorObject
{
    enum Waveform {
        SINE=0,
        COSINE,
        TRIANGLE,
        SQUARE,
        RECTANGLE,
        SAWTOOTH,
        RAMP,
        MODIFIED_TRIANGLE,
        MODIFIED_SQUARE,
        HALF_WAVE_RECTIFIED_SINE,
        FULL_WAVE_RECTIFIED_SINE,
        TRIANGULAR_PULSE,
        TRAPEZOID_FIXED,
        TRAPEZOID_VARIABLE
    };

    Waveform waveform;
    double sampleRate;
    double freqInSecondsPerSample;
    double amplitude; // Frequency dependent gain [0.0..1.0]
    double pulseWidth; // [0.0..1.0]
    double t; // The current phase [0.0..1.0) of the oscillator.


    SW_PolyBLEP(double sampleRate, Waveform wave = SINE)
    : sampleRate(sampleRate), amplitude(1.0), t(0.0) 
    {     
        setSampleRate(sampleRate);
        setFrequency(440.0);        
        setWaveform(wave);
        setPulseWidth(0.5);
    }
    ~SW_PolyBLEP() = default;
    
    void setFrequency(double freqInHz) {
        setdt(freqInHz / sampleRate);
        m_freq = freqInHz;
    }
    void setdt(double time) {
        freqInSecondsPerSample = time;
        m_phase = t;
    }
    void setSampleRate(double sampleRate)  {
        const double freqInHz = getFreqInHz();
        this->sampleRate = sampleRate;
        setFrequency(freqInHz);
    }
    void setWaveform(Waveform waveform)
    {
        this->waveform = waveform;
    }
    void setPhase(double p) {
        t = p;
        m_phase = p;
    }
    double getPhase() {
        return t;
    }
    void setPulseWidth(double pw)  {
        this->pulseWidth = pulseWidth;
        m_pwm = pw;
    }

    double get()  {
        double r = 0;
        double tfreq  = m_freq;
        double tphase = t;
        setPulseWidth(m_pwm);        
        setFrequency(m_index * m_fenv*(m_freq + m_freq*m_fm));
        t += m_pm;
        t *= m_penv;
        if(getFreqInHz() >= sampleRate / 2) {
            return sin();
        } else switch (waveform) {
            case SINE:                      
                r = sin();
                break;
            case COSINE:
                r = cos();
                break;
            case TRIANGLE:
                r = tri();
                break;
            case SQUARE:
                r = sqr();
                break;
            case RECTANGLE:
                r = rect();
                break;
            case SAWTOOTH:                                
                r = saw();
                break;
            case RAMP:
                r = ramp();
                break;
            case MODIFIED_TRIANGLE:
                r = tri2();
                break;
            case MODIFIED_SQUARE:
                r = sqr2();
                break;
            case HALF_WAVE_RECTIFIED_SINE:
                r = half();
                break;
            case FULL_WAVE_RECTIFIED_SINE:
                r = full();
                break;
            case TRIANGULAR_PULSE:
                r = trip();
                break;
            case TRAPEZOID_FIXED:
                r = trap();
                break;
            case TRAPEZOID_VARIABLE:
                r = trap2();
                break;
            default:
                r = 0.0;
        }
        setFrequency(tfreq);
        t = tphase;
        m_phase = t;
        r *= m_env;
        r *= m_lfo;
        r *= m_mod;
        r  = fmod(r,m_cmod);
        t += m_drift;
        return m_gain*r;
    }
    

    double Tick()
    {
        return getAndInc();
    }
    void inc() {
        t += freqInSecondsPerSample;
        t -= bitwiseOrZero(t);
    }

    double getAndInc() {
        const double sample = get();
        inc();
        return sample;
    }  

    double getFreqInHz() 
    {
        return freqInSecondsPerSample * sampleRate;
    }

    void sync(double phase)
    {
        t = phase;
        if (t >= 0) {
            t -= bitwiseOrZero(t);
        } else {
            t += 1 - bitwiseOrZero(t);
        }
    }

    
    
    double sin() {
        return amplitude * std::sin(TWO_PI * t);
    }

    double cos() {
        return amplitude * std::cos(TWO_PI * t);
    }

    double half() {
        double t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        double y = (t < 0.5 ? 2 * std::sin(TWO_PI * t) - 2 / M_PI : -2 / M_PI);
        y += TWO_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    double full() {
        double _t = this->t + 0.25;
        _t -= bitwiseOrZero(_t);

        double y = 2 * std::sin(M_PI * _t) - 4 / M_PI;
        y += TWO_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    double tri() {
        double t1 = t + 0.25;
        t1 -= bitwiseOrZero(t1);

        double t2 = t + 0.75;
        t2 -= bitwiseOrZero(t2);

        double y = t * 4;

        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }

        y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    double tri2() {
        double pulseWidth = std::fmax(0.0001, std::fmin(0.9999, this->pulseWidth));

        double t1 = t + 0.5 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        double t2 = t + 1 - 0.5 * pulseWidth;
        t2 -= bitwiseOrZero(t2);

        double y = t * 2;

        if (y >= 2 - pulseWidth) {
            y = (y - 2) / pulseWidth;
        } else if (y >= pulseWidth) {
            y = 1 - (y - pulseWidth) / (1 - pulseWidth);
        } else {
            y /= pulseWidth;
        }

        y += freqInSecondsPerSample / (pulseWidth - pulseWidth * pulseWidth) * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    double trip() {
        double t1 = t + 0.75 + 0.5 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        double y;
        if (t1 >= pulseWidth) {
            y = -pulseWidth;
        } else {
            y = 4 * t1;
            y = (y >= 2 * pulseWidth ? 4 - y / pulseWidth - pulseWidth : y / pulseWidth - pulseWidth);
        }

        if (pulseWidth > 0) {
            double t2 = t1 + 1 - 0.5 * pulseWidth;
            t2 -= bitwiseOrZero(t2);

            double t3 = t1 + 1 - pulseWidth;
            t3 -= bitwiseOrZero(t3);
            y += 2 * freqInSecondsPerSample / pulseWidth * (blamp(t1, freqInSecondsPerSample) - 2 * blamp(t2, freqInSecondsPerSample) + blamp(t3, freqInSecondsPerSample));
        }
        return amplitude * y;
    }

    double trap() {
        double y = 4 * t;
        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }
        y = std::fmax(-1, std::fmin(1, 2 * y));

        double t1 = t + 0.125;
        t1 -= bitwiseOrZero(t1);

        double t2 = t1 + 0.5;
        t2 -= bitwiseOrZero(t2);

        // Triangle #1
        y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        t1 = t + 0.375;
        t1 -= bitwiseOrZero(t1);

        t2 = t1 + 0.5;
        t2 -= bitwiseOrZero(t2);

        // Triangle #2
        y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    double trap2() {
        double pulseWidth = std::fmin(0.9999, this->pulseWidth);
        double scale = 1 / (1 - pulseWidth);

        double y = 4 * t;
        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }
        y = std::fmax(-1, std::fmin(1, scale * y));

        double t1 = t + 0.25 - 0.25 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        double t2 = t1 + 0.5;
        t2 -= bitwiseOrZero(t2);

        // Triangle #1
        y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        t1 = t + 0.25 + 0.25 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        t2 = t1 + 0.5;
        t2 -= bitwiseOrZero(t2);

        // Triangle #2
        y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    double sqr() const {
        double t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        double y = t < 0.5 ? 1 : -1;
        y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * y;
    }

    double sqr2() {
        double t1 = t + 0.875 + 0.25 * (pulseWidth - 0.5);
        t1 -= bitwiseOrZero(t1);

        double t2 = t + 0.375 + 0.25 * (pulseWidth - 0.5);
        t2 -= bitwiseOrZero(t2);

        // Square #1
        double y = t1 < 0.5 ? 1 : -1;

        y += blep(t1, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        t1 += 0.5 * (1 - pulseWidth);
        t1 -= bitwiseOrZero(t1);

        t2 += 0.5 * (1 - pulseWidth);
        t2 -= bitwiseOrZero(t2);

        // Square #2
        y += t1 < 0.5 ? 1 : -1;

        y += blep(t1, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * 0.5 * y;
    }

    double rect() {
        double t2 = t + 1 - pulseWidth;
        t2 -= bitwiseOrZero(t2);

        double y = -2 * pulseWidth;
        if (t < pulseWidth) {
            y += 2;
        }

        y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * y;
    }

    double saw() {
        double _t = t + 0.5;
        _t -= bitwiseOrZero(_t);

        double y = 2 * _t - 1;
        y -= blep(_t, freqInSecondsPerSample);

        return (amplitude * y);
    }

    double ramp() {
        double _t = t;
        _t -= bitwiseOrZero(_t);

        double y = 1 - 2 * _t;
        y += blep(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

};




