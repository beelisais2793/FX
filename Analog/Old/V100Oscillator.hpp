#pragma

//#include "Filters.h"
//#include "Decimators.hpp"

namespace V100
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VCO
    // 2x oversampling
    // Decimators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    class VCO {
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

        VCO(T sampleRate, Waveform waveform = SINE, T initialFrequency = 440.0);

        virtual ~VCO();

        void setFrequency(T freqInHz);

        void setSampleRate(T sampleRate);

        void setWaveform(Waveform waveform);

        void setPulseWidth(T pw);

        T get();
        T get(T fm);

        void inc();

        T getAndInc();
        T getAndInc(T fm);

        T getFreqInHz();

        void sync(T phase);

        T Tick() {
            return getAndInc();
        }
        T Tick(T in) {
            return in*getAndInc();
        }
        void  Process(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*getAndInc();
        }
        void  Inplace(size_t n, T * samples) {
            for(size_t i = 0; i < n; i++) samples[i] = samples[i]*getAndInc();
        }
    protected:

        //RBJFilter   *lowpass;
        //Decimateur5 *decimator;
        size_t       overSample;
        Waveform     waveform;
        T y;        
        T sampleRate;
        T freqInSecondsPerSample;
        T amplitude; // Frequency dependent gain [0.0..1.0]
        T pulseWidth; // [0.0..1.0]
        T t; // The current phase [0.0..1.0) of the oscillator.

        void setdt(T time);

        T sin();

        T cos();

        T half();

        T full();

        T tri();

        T tri2();

        T trip();

        T trap();

        T trap2();

        T sqr();

        T sqr2();

        T rect();

        T saw();

        T ramp();
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
    VCO<T>::VCO(T sampleRate, Waveform waveform, T initialFrequency)
            : waveform(waveform), amplitude(1.0), t(0.0) {
        overSample = 1;
        setSampleRate(overSample*sampleRate);
        setFrequency(initialFrequency);
        setWaveform(waveform);
        setPulseWidth(0.5);
        y = 0;
            
        //lowpass = new RBJFilter(RBJFilter::FilterType::LOWPASS,15000.0f,overSample*sampleRate);
        //decimator = new Decimateur5;
    }

    template<typename T>
    VCO<T>::~VCO() {
        if(lowpass) delete lowpass;
        if(decimator) delete decimator;
    }

    template<typename T>
    void VCO<T>::setdt(T time) {
        freqInSecondsPerSample = time;
    }

    template<typename T>
    void VCO<T>::setFrequency(T freqInHz) {
        setdt(freqInHz / sampleRate);
    }

    template<typename T>
    void VCO<T>::setSampleRate(T sampleRate) {
        T freqInHz = getFreqInHz();
        this->sampleRate = sampleRate;
        setFrequency(freqInHz);
    }

    template<typename T>
    T VCO<T>::getFreqInHz() {
        return freqInSecondsPerSample * sampleRate;
    }

    template<typename T>
    void VCO<T>::setPulseWidth(T pulseWidth) {
        this->pulseWidth = pulseWidth;
    }

    template<typename T>
    void VCO<T>::sync(T phase) {
        t = phase;
        if (t >= 0) {
            t -= bitwiseOrZero(t);
        } else {
            t += 1 - bitwiseOrZero(t);
        }
    }

    template<typename T>
    void VCO<T>::setWaveform(Waveform waveform) {
        this->waveform = waveform;
    }

    template<typename T>
    T VCO<T>::get() {
        T r = 0;
        if(getFreqInHz() >= sampleRate / 2) {
            r =  sin();
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
                r= saw();
                break;
            case RAMP:
                r= ramp();
                break;
            case MODIFIED_TRIANGLE:
                r= tri2();
                break;
            case MODIFIED_SQUARE:
                r= sqr2();
                break;
            case HALF_WAVE_RECTIFIED_SINE:
                r= half();
                break;
            case FULL_WAVE_RECTIFIED_SINE:
                r= full();
                break;
            case TRIANGULAR_PULSE:
                r= trip();
                break;
            case TRAPEZOID_FIXED:
                r= trap();
                break;
            case TRAPEZOID_VARIABLE:
                r= trap2();
                break;
            default:
                return 0.0;
        }
        //r = lowpass->Tick(r);
        //r = decimator->Calc(y,r);
        y = r;
        return r;
    }

    template<typename T>
    T VCO<T>::get(T fm)  {
        T r;
        T temp = t;
        t += fm;
        while(t < 0) t += 1.0;
        while(t > 1.0) t -= 1.0;
        if(getFreqInHz() >= sampleRate / 2) {
            r= sin();
        } else switch (waveform) {
            case SINE:
                r= sin(); break;
            case COSINE:
                r= cos(); break;
            case TRIANGLE:
                r = tri(); break;
            case SQUARE:
                r=sqr();
            case RECTANGLE:
                r=rect();
            case SAWTOOTH:
                r=saw();
            case RAMP:
                r=ramp();
            case MODIFIED_TRIANGLE:
                r=tri2();
            case MODIFIED_SQUARE:
                r=sqr2();
            case HALF_WAVE_RECTIFIED_SINE:
                r=half();
            case FULL_WAVE_RECTIFIED_SINE:
                r=full();
            case TRIANGULAR_PULSE:
                r=trip();
            case TRAPEZOID_FIXED:
                r=trap();
            case TRAPEZOID_VARIABLE:
                r=trap2();
            default:
                r= 0.0;
        }
        t = temp;
        r = lowpass->Tick(r);
        r = decimator->Calc(y,r);
        y = r;    
        return r;
    }

    template<typename T>
    void VCO<T>::inc() {
        t += freqInSecondsPerSample;
        t -= bitwiseOrZero(t);
    }

    template<typename T>
    T VCO<T>::getAndInc() {
        T sample = get();
        inc();
        return sample;
    }

    template<typename T>
    T VCO<T>::getAndInc(T fm) {
        T sample = get(fm);
        inc();
        return sample;
    }

    template<typename T>
    T VCO<T>::sin() {
        return amplitude * std::sin(TWO_PI * t);
    }

    template<typename T>
    T VCO<T>::cos() {
        return amplitude * std::cos(TWO_PI * t);
    }

    template<typename T>
    T VCO<T>::half() {
        T t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        T y = (t < 0.5 ? 2 * std::sin(TWO_PI * t) - 2 / M_PI : -2 / M_PI);
        y += TWO_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

        return amplitude * y;
    }

    template<typename T>
    T VCO<T>::full() {
        T _t = this->t + 0.25;
        _t -= bitwiseOrZero(_t);

        T y = 2 * std::sin(M_PI * _t) - 4 / M_PI;
        y += TWO_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T VCO<T>::tri() {
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
    T VCO<T>::tri2() {
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
    T VCO<T>::trip() {
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
    T VCO<T>::trap() {
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
    T VCO<T>::trap2() {
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
    T VCO<T>::sqr() {
        T t2 = t + 0.5;
        t2 -= bitwiseOrZero(t2);

        T y = t < 0.5 ? 1 : -1;
        y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T VCO<T>::sqr2() {
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
    T VCO<T>::rect() {
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
    T VCO<T>::saw() {
        T _t = t + 0.5;
        _t -= bitwiseOrZero(_t);

        T y = 2 * _t - 1;
        y -= blep(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    T VCO<T>::ramp() {
        T _t = t;
        _t -= bitwiseOrZero(_t);

        T y = 1 - 2 * _t;
        y += blep(_t, freqInSecondsPerSample);

        return amplitude * y;
    }

    template<typename T>
    struct StereoVCO
    {
        VCO<T> * osc[2];
    };

}
