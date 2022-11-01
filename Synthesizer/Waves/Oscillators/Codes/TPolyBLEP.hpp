/*
TPolyBLEP Waveform generator ported from the Jesusonic code by Tale
http://www.taletn.com/reaper/mono_synth/

Permission has been granted to release this port under the WDL/IPlug license:

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once
#define _USE_MATH_DEFINES

#include <math.h>
#include <cmath>
#include <cstdint>


namespace SoundWave 
{
    template<typename T>
    class TPolyBLEP {
    public:
        enum Waveform {
            SINE,
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

        TPolyBLEP(T sampleRate, Waveform waveform = SINE, T initialFrequency = 440.0);

        virtual ~TPolyBLEP();

        void setFrequency(T freqInHz);

        void setSampleRate(T sampleRate);

        void setWaveform(Waveform waveform);

        void setPulseWidth(T pw);

        T get() const;

        void inc();

        T getAndInc();

        T getFreqInHz() const;

        void sync(T phase);

        T Tick() { return getAndInc(); }
        T Tick(T in) { return in * getAndInc(); }
        void Process(T * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = getAndInc();
        }
        void Process(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*getAndInc();
        }
    protected:
        Waveform waveform;
        T sampleRate;
        T freqInSecondsPerSample;
        T amplitude; // Frequency dependent gain [0.0..1.0]
        T pulseWidth; // [0.0..1.0]
        T t; // The current phase [0.0..1.0) of the oscillator.

        void setdt(T time);

        T sin() const;

        T cos() const;

        T half() const;

        T full() const;

        T tri() const;

        T tri2() const;

        T trip() const;

        T trap() const;

        T trap2() const;

        T sqr() const;

        T sqr2() const;

        T rect() const;

        T saw() const;

        T ramp() const;
    };


    const double TWO_PI = 2 * M_PI;

    template<typename T>
    inline T square_number(const T &x) {
        return x * x;
    }

    // Adapted from "Phaseshaping Oscillator Algorithms for Musical Sound
    // Synthesis" by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa
    // Valimaki.
    // http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
    template<typename T>
    inline T blep(T t, T dt) {
        if (t < dt) {
            return -square_number(t / dt - 1);
        } else if (t > 1 - dt) {
            return square_number((t - 1) / dt + 1);
        } else {
            return 0;
        }
    }

    // Derived from blep().
    template<typename T>
    inline T blamp(T t, T dt) {
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

    template<typename T>
    TPolyBLEP<T>::TPolyBLEP(T sampleRate, Waveform waveform, T initialFrequency)
            : waveform(waveform), sampleRate(sampleRate), amplitude(1.0), t(0.0) {
        setSampleRate(sampleRate);
        setFrequency(initialFrequency);
        setWaveform(waveform);
        setPulseWidth(0.5);
    }

    template<typename T>
    TPolyBLEP<T>::~TPolyBLEP() {

    }

    template<typename T>
    void TPolyBLEP<T>::setdt(T time) {
        freqInSecondsPerSample = time;
    }

    template<typename T>
    void TPolyBLEP<T>::setFrequency(T freqInHz) {
        setdt(freqInHz / sampleRate);
    }

    template<typename T>
    void TPolyBLEP<T>::setSampleRate(T sampleRate) {
        const T freqInHz = getFreqInHz();
        this->sampleRate = sampleRate;
        setFrequency(freqInHz);
    }

    template<typename T>
    T TPolyBLEP<T>::getFreqInHz() const {
        return freqInSecondsPerSample * sampleRate;
    }

    template<typename T>
    void TPolyBLEP<T>::setPulseWidth(T pulseWidth) {
        this->pulseWidth = pulseWidth;
    }

    template<typename T>
    void TPolyBLEP<T>::sync(T phase) {
        t = phase;
        if (t >= 0) {
            t -= bitwiseOrZero(t);
        } else {
            t += 1 - bitwiseOrZero(t);
        }
    }

    template<typename T>
    void TPolyBLEP<T>::setWaveform(Waveform waveform) {
        this->waveform = waveform;
    }

    template<typename T>
    T TPolyBLEP<T>::get() const {
        if(getFreqInHz() >= sampleRate / 4) {
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

    template<typename T>
    void TPolyBLEP<T>::inc() {
        t += freqInSecondsPerSample;
        t -= bitwiseOrZero(t);
    }

    template<typename T>
    T TPolyBLEP<T>::getAndInc() {
        const T sample = get();
        inc();
        return sample;
    }

    template<typename T>
    T TPolyBLEP<T>::sin() const {
        return amplitude * std::sin(TWO_PI * t);
    }

    template<typename T>
    T TPolyBLEP<T>::cos() const {
        return amplitude * std::cos(TWO_PI * t);
    }

    template<typename T>
    T TPolyBLEP<T>::half() const {
        T t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        T y = (t < 0.5 ? 2 * std::sin(TWO_PI * t) - 2 / M_PI : -2 / M_PI);
        y += TWO_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::full() const {
        T _t = this->t + 0.25;
        _t -= bitwiseOrZero(_t);

        T y = 2 * std::sin(M_PI * _t) - 4 / M_PI;
        y += TWO_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::tri() const {
        T t1 = t + 0.25;
        t1 -= bitwiseOrZero(t1);

        T t2 = t + 0.75;
        t2 -= bitwiseOrZero(t2);

        T y = t * 4;

        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }

        y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::tri2() const {
        T pulseWidth = std::fmax(0.0001, std::fmin(0.9999, this->pulseWidth));

        T t1 = t + 0.5 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        T t2 = t + 1 - 0.5 * pulseWidth;
        t2 -= bitwiseOrZero(t2);

        T y = t * 2;

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

    template<typename T>
    T TPolyBLEP<T>::trip() const {
        T t1 = t + 0.75 + 0.5 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        T y;
        if (t1 >= pulseWidth) {
            y = -pulseWidth;
        } else {
            y = 4 * t1;
            y = (y >= 2 * pulseWidth ? 4 - y / pulseWidth - pulseWidth : y / pulseWidth - pulseWidth);
        }

        if (pulseWidth > 0) {
            T t2 = t1 + 1 - 0.5 * pulseWidth;
            t2 -= bitwiseOrZero(t2);

            T t3 = t1 + 1 - pulseWidth;
            t3 -= bitwiseOrZero(t3);
            y += 2 * freqInSecondsPerSample / pulseWidth * (blamp(t1, freqInSecondsPerSample) - 2 * blamp(t2, freqInSecondsPerSample) + blamp(t3, freqInSecondsPerSample));
        }
        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::trap() const {
        T y = 4 * t;
        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }
        y = std::fmax(-1, std::fmin(1, 2 * y));

        T t1 = t + 0.125;
        t1 -= bitwiseOrZero(t1);

        T t2 = t1 + 0.5;
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

    template<typename T>
    T TPolyBLEP<T>::trap2() const {
        T pulseWidth = std::fmin(0.9999, this->pulseWidth);
        T scale = 1 / (1 - pulseWidth);

        T y = 4 * t;
        if (y >= 3) {
            y -= 4;
        } else if (y > 1) {
            y = 2 - y;
        }
        y = std::fmax(-1, std::fmin(1, scale * y));

        T t1 = t + 0.25 - 0.25 * pulseWidth;
        t1 -= bitwiseOrZero(t1);

        T t2 = t1 + 0.5;
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

    template<typename T>
    T TPolyBLEP<T>::sqr() const {
        T t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        T y = t < 0.5 ? 1 : -1;
        y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::sqr2() const {
        T t1 = t + 0.875 + 0.25 * (pulseWidth - 0.5);
        t1 -= bitwiseOrZero(t1);

        T t2 = t + 0.375 + 0.25 * (pulseWidth - 0.5);
        t2 -= bitwiseOrZero(t2);

        // Square #1
        T y = t1 < 0.5 ? 1 : -1;

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

    template<typename T>
    T TPolyBLEP<T>::rect() const {
        T t2 = t + 1 - pulseWidth;
        t2 -= bitwiseOrZero(t2);

        T y = -2 * pulseWidth;
        if (t < pulseWidth) {
            y += 2;
        }

        y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::saw() const {
        T _t = t + 0.5;
        _t -= bitwiseOrZero(_t);

        T y = 2 * _t - 1;
        y -= blep(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T TPolyBLEP<T>::ramp() const {
        T _t = t;
        _t -= bitwiseOrZero(_t);

        T y = 1 - 2 * _t;
        y += blep(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

}