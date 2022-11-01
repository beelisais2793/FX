#pragma once
// modified by macho charlie 1993 

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

namespace Analog::Oscillators::PolyBLEPOsc
{
    

    template<typename T>
    inline T square_number(const T &x) {
        return x * x;
    }

    // Adapted from "Phaseshaping Oscillator Algorithms for Musical Sound
    // Synthesis" by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa
    // Valimaki.
    // http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
    inline DspFloatType blep(DspFloatType t, DspFloatType dt) {
        if (t < dt) {
            return -square_number(t / dt - 1);
        } else if (t > 1 - dt) {
            return square_number((t - 1) / dt + 1);
        } else {
            return 0;
        }
    }

    // Derived from blep().
    inline DspFloatType blamp(DspFloatType t, DspFloatType dt) {
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

    struct PolyBLEP 
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
        DspFloatType sampleRate;
        DspFloatType freqInSecondsPerSample;
        DspFloatType amplitude; // Frequency dependent gain [0.0..1.0]
        DspFloatType pulseWidth; // [0.0..1.0]
        DspFloatType t; // The current phase [0.0..1.0) of the oscillator.


        PolyBLEP(DspFloatType sampleRate, Waveform wave = SINE)
        : sampleRate(sampleRate), amplitude(1.0), t(0.0) 
        {     
            setSampleRate(sampleRate);
            setFrequency(440.0);        
            setWaveform(wave);
            setPulseWidth(0.5);
        }
        ~PolyBLEP() = default;
        
        void setFrequency(DspFloatType freqInHz) {
            setdt(freqInHz / sampleRate);
        }
        void setSampleRate(DspFloatType sampleRate)  {
            const DspFloatType freqInHz = getFreqInHz();
            this->sampleRate = sampleRate;
            setFrequency(freqInHz);
        }
        void setWaveform(Waveform waveform)
        {
            this->waveform = waveform;
        }
        void setPhase(DspFloatType p) {
            t = p;
        }
        DspFloatType getPhase() {
            return t;
        }
        void setPulseWidth(DspFloatType pw)  {
            this->pulseWidth = pulseWidth;
        }
        enum {
            PORT_FREQ,
            PORT_WAVEFORM,
            PORT_PHASE,
            PORT_PULSEWIDTH
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_FREQ: setFrequency(v); break;
                case PORT_WAVEFORM: setWaveform((Waveform)v); break;
                case PORT_PHASE: setPhase(v); break;
                case PORT_PULSEWIDTH: setPulseWidth(v); break;
            }
        }
        DspFloatType get()  {
            
            if(getFreqInHz() >= sampleRate / 2) {
                return sin();
            } else switch (waveform) {
                case SINE:                      
                    return sin();
                case COSINE:
                    return cos();
                case TRIANGLE:
                    return tri();
                case SQUARE:
                    return sqr();
                case RECTANGLE:
                    return rect();
                case SAWTOOTH:                                
                    return saw();
                case RAMP:
                    return ramp();
                case MODIFIED_TRIANGLE:
                    return tri2();
                case MODIFIED_SQUARE:
                    return sqr2();
                case HALF_WAVE_RECTIFIED_SINE:
                    return half();
                case FULL_WAVE_RECTIFIED_SINE:
                    return full();
                case TRIANGULAR_PULSE:
                    return trip();
                case TRAPEZOID_FIXED:
                    return trap();
                case TRAPEZOID_VARIABLE:
                    return trap2();
                default:
                    return 0.0;
            }
        }

        DspFloatType Tick(DspFloatType I = 1, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            DspFloatType p = t;
            DspFloatType f = I*(freqInSecondsPerSample + X);
            t = fmod((t+f)+Y,1);
            DspFloatType out = get();
            t = p;
            inc();
            return out;
        }
        void inc() {
            t += freqInSecondsPerSample;
            t -= bitwiseOrZero(t);
        }

        DspFloatType getAndInc() {
            const DspFloatType sample = get();
            inc();
            return sample;
        }  

        DspFloatType getFreqInHz() 
        {
            return freqInSecondsPerSample * sampleRate;
        }

        void sync(DspFloatType phase)
        {
            t = phase;
            if (t >= 0) {
                t -= bitwiseOrZero(t);
            } else {
                t += 1 - bitwiseOrZero(t);
            }
        }

        
        void setdt(DspFloatType time) {
            freqInSecondsPerSample = time;
        }

        DspFloatType sin() {
            return amplitude * std::sin(2.0*M_PI * t);
        }

        DspFloatType cos() {
            return amplitude * std::cos(2.0*M_PI * t);
        }

        DspFloatType half() {
            DspFloatType t2 = t + 0.5;
            t2 -= bitwiseOrZero(t2);

            DspFloatType y = (t < 0.5 ? 2 * std::sin(2.0*M_PI * t) - 2 / M_PI : -2 / M_PI);
            y += 2.0*M_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

            return amplitude * y;
        }

        DspFloatType full() {
            DspFloatType _t = this->t + 0.25;
            _t -= bitwiseOrZero(_t);

            DspFloatType y = 2 * std::sin(M_PI * _t) - 4 / M_PI;
            y += 2.0*M_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

            return amplitude * y;
        }

        DspFloatType tri() {
            DspFloatType t1 = t + 0.25;
            t1 -= bitwiseOrZero(t1);

            DspFloatType t2 = t + 0.75;
            t2 -= bitwiseOrZero(t2);

            DspFloatType y = t * 4;

            if (y >= 3) {
                y -= 4;
            } else if (y > 1) {
                y = 2 - y;
            }

            y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

            return amplitude * y;
        }

        DspFloatType tri2() {
            DspFloatType pulseWidth = std::fmax(0.0001, std::fmin(0.9999, this->pulseWidth));

            DspFloatType t1 = t + 0.5 * pulseWidth;
            t1 -= bitwiseOrZero(t1);

            DspFloatType t2 = t + 1 - 0.5 * pulseWidth;
            t2 -= bitwiseOrZero(t2);

            DspFloatType y = t * 2;

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

        DspFloatType trip() {
            DspFloatType t1 = t + 0.75 + 0.5 * pulseWidth;
            t1 -= bitwiseOrZero(t1);

            DspFloatType y;
            if (t1 >= pulseWidth) {
                y = -pulseWidth;
            } else {
                y = 4 * t1;
                y = (y >= 2 * pulseWidth ? 4 - y / pulseWidth - pulseWidth : y / pulseWidth - pulseWidth);
            }

            if (pulseWidth > 0) {
                DspFloatType t2 = t1 + 1 - 0.5 * pulseWidth;
                t2 -= bitwiseOrZero(t2);

                DspFloatType t3 = t1 + 1 - pulseWidth;
                t3 -= bitwiseOrZero(t3);
                y += 2 * freqInSecondsPerSample / pulseWidth * (blamp(t1, freqInSecondsPerSample) - 2 * blamp(t2, freqInSecondsPerSample) + blamp(t3, freqInSecondsPerSample));
            }
            return amplitude * y;
        }

        DspFloatType trap() {
            DspFloatType y = 4 * t;
            if (y >= 3) {
                y -= 4;
            } else if (y > 1) {
                y = 2 - y;
            }
            y = std::fmax(-1, std::fmin(1, 2 * y));

            DspFloatType t1 = t + 0.125;
            t1 -= bitwiseOrZero(t1);

            DspFloatType t2 = t1 + 0.5;
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

        DspFloatType trap2() {
            DspFloatType pulseWidth = std::fmin(0.9999, this->pulseWidth);
            DspFloatType scale = 1 / (1 - pulseWidth);

            DspFloatType y = 4 * t;
            if (y >= 3) {
                y -= 4;
            } else if (y > 1) {
                y = 2 - y;
            }
            y = std::fmax(-1, std::fmin(1, scale * y));

            DspFloatType t1 = t + 0.25 - 0.25 * pulseWidth;
            t1 -= bitwiseOrZero(t1);

            DspFloatType t2 = t1 + 0.5;
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

        DspFloatType sqr() const {
            DspFloatType t2 = t + 0.5;
            t2 -= bitwiseOrZero(t2);

            DspFloatType y = t < 0.5 ? 1 : -1;
            y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

            return amplitude * y;
        }

        DspFloatType sqr2() {
            DspFloatType t1 = t + 0.875 + 0.25 * (pulseWidth - 0.5);
            t1 -= bitwiseOrZero(t1);

            DspFloatType t2 = t + 0.375 + 0.25 * (pulseWidth - 0.5);
            t2 -= bitwiseOrZero(t2);

            // Square #1
            DspFloatType y = t1 < 0.5 ? 1 : -1;

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

        DspFloatType rect() {
            DspFloatType t2 = t + 1 - pulseWidth;
            t2 -= bitwiseOrZero(t2);

            DspFloatType y = -2 * pulseWidth;
            if (t < pulseWidth) {
                y += 2;
            }

            y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

            return amplitude * y;
        }

        DspFloatType saw() {
            DspFloatType _t = t + 0.5;
            _t -= bitwiseOrZero(_t);

            DspFloatType y = 2 * _t - 1;
            y -= blep(_t, freqInSecondsPerSample);

            return (amplitude * y);
        }

        DspFloatType ramp() {
            DspFloatType _t = t;
            _t -= bitwiseOrZero(_t);

            DspFloatType y = 1 - 2 * _t;
            y += blep(_t, freqInSecondsPerSample);

            return amplitude * y;
        }

    };
}



