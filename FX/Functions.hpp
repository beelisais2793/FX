#pragma once
#include <cmath>

inline double dsp_osc_step(double freq, unsigned int rate)
{
    return rate ? freq / rate : 0.0;
}

/**
 * Increment the time using the step.
 *   @t: The time.
 *   @step: The step.
 */

inline double dsp_osc_inc(double t, double step)
{
    return fmod(t + step, 1.0);
}

/**
 * Decrement the time using the step.
 *   @t: The time.
 *   @step: The step.
 */

inline double dsp_osc_dec(double t, double step)
{
    return fmod(fmod(t - step, 1.0) + 1.0, 1.0);
}

/**
 * Generate a sine wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

inline double dsp_osc_sine(double t)
{
    return sin(2.0 * M_PI * t);
}

/**
 * Generate a square wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

inline double dsp_osc_square(double t)
{
    return (t < 0.5) ? 1.0 : -1.0;
}

/**
 * Generate a sawtooth wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

inline double dsp_osc_saw(double t)
{
    return fmod(1.0 + 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a reverse sawtooth wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

inline double dsp_osc_rsaw(double t)
{
    return fmod(3.0 - 2.0 * t, 2.0) - 1.0;
}

/**
 * Generate a triangle wave data point.
 *   @t: The time.
 *   &returns: The data point.
 */

inline double dsp_osc_tri(double t)
{
    return 2.0 * fabs(2.0 * t - 1.0) - 1.0;
}



struct NonBandlimitedOsc
{
    double phase;
    double freq;
    double step;        
    double sample_rate;

    double (*function)(double);
    
    enum Type {
            SIN,
            SAW,
            REVERSE_SAW,
            SQUARE,
            TRIANGLE,
        } _type;

    void SetFunction(Type t) {
        _type = t;
        switch(_type)
        {
            case SIN: function = dsp_osc_sin; break;
            case SAW: function = dsp_osc_saw; break;
            case SQUARE: function = dsp_osc_square; break;
            case TRIANGLE: function = dsp_osc_triangle; break;
            case REVERSE_SAW: function = dsp_osc_rsaw; break;
        }
    }

    NonBandlimitedOsc(Type t, double f, double sr = 44100) {
        freq = f;
        sample_rate = sr;
        step = dsp_osc_step(f,sr);
        _type = t;
        SetFunction(t);
    }

    void Increment() {
        phase = dsp_osc_inc(phase,step);        
    }

    void Decrement() {
        phase = dsp_osc_dec(phase,step);        
    }

    double Tick() {            
        Increment();        
        return function(phase);
    }

    double Tick(double I, double A=1, double X=0,double Y=0) {                
        double tp = phase;                
        double tf = freq;
        phase = phase + Y;
        if(phase > 1.0) phase -=1.0;
        if(phase < 0) phase += 1.0;
        double ts = step;
        step = dsp_osc_step(freq + X,sample_rate);
        double r = I*A*Tick();                
        phase = tp;
        freq  = tf;
        step = dsp_osc_step(freq,sampleRate);
        return r;
    }
};


struct LFO : public NonBandlimitedOscillator
{
        
    LFO( Type type, double freq, double sampleRate=44100) : NonBandlimitedOscillator(type,freq,sampleRate)
    {
        
    }
    ~LFO() = default;
};
}