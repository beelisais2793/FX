#pragma once
#include <cmath>


// Arduino Calculus Library
// Arduino Filters - https://github.com/JonHub/Filters


namespace Calculus
{
    // a filter IS an integrator
    struct Integrator
    {
        double prev;
        double dt = 1/sampleRate;

        Integrator() : prev(0) {}

        double Tick(double I, double A=1,double X=-1, double Y=1)
        {
            double r = I*dt + prev;
            prev = I;        
            return r;
        }
    };

    struct TrapezoidalIntegrator
    {
        double output,prev;
        double dt = 1/sampleRate;

        TrapezoidalIntegrator() : output(0),prev(0) {}

        double Tick(double I, double A=1,double X=-1, double Y=1)
        {
            output += (I*dt + prev)*0.5;
            prev    = I;        
            return A*output ;
        }    
    };

    
    struct Differentiator
    {
        double prev;

        Differentiator() : prev(0) {}

        double Tick(double I, double A=1,double X=-1, double Y=1)
        {
            double r = (I - prev);
            prev = I;        
            return r;
        }
    };

    struct Modulator
    {
        double prev;

        Modulator() : prev(0) {}

        double Tick(double I, double A=1,double X=-1, double Y=1)
        {
            double r = (I * prev);
            prev = I;        
            return r;
        }
    };

    struct Circulator
    {
        double prev;

        Circulator() : prev(0) {}

        double Tick(double I, double A=1,double X=-1, double Y=1)
        {
            double r = (std::fmod(I,prev));
            prev = I;
            return r;
        }
    };

    struct Opamp
    {
        double Tick(double A, double B)
        {
            return A - B;
        }
    };

    struct Comparator
    {
        double Tick(double A, double B) {
            if(A > B) return 1;
            return -1;
        }
    };

    struct SlewLimiter {
        float _delta;
        float _last = 0.0f;

        SlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f) {
            setParams(sampleRate, milliseconds, range);
        }

        void setParams(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f);
        inline void setLast(float last) { _last = last; }
        inline float next(float sample) {
            return _last = next(sample, _last);
        }
        float next(float sample, float last);
    };

    void SlewLimiter::setParams(float sampleRate, float milliseconds, float range) {
        assert(sampleRate > 0.0f);
        assert(milliseconds >= 0.0f);
        assert(range > 0.0f);
        _delta = range / ((milliseconds / 1000.0f) * sampleRate);
    }

    float SlewLimiter::next(float sample, float last) {
        if (sample > last) {
            return std::min(last + _delta, sample);
        }
        return std::max(last - _delta, sample);
    }


    struct Slew
    {
        double rise,fall;
        double target,out;
        double sr;
        Slew(double hz,double fs)
        {
            sr = fs;
            rise = hz/sr;
            fall = hz/sr;
            target = 1.0;
            out = 0;
        }
        void setRise(float hz) {
            rise = hz/sr;
        }
        void setFall(float hz) {
            fall = hz/sr;
        }
        void setTarget(double t) {
            target = t;
        }
        double Tick() {
            //out = clamp(target, out - fall, out + rise);
            if(fabs(out - target) < 1e-3) return target;
            if(out < target) out += rise;
            if(out > target) out -= fall;
            return out;
        }
        double Tick(double _target) {
            //out = clamp(_target, out - fall, out + rise);
            if(fabs(out - _target) < 1e-3) return _target;
            if(out < _target) out += rise;
            if(out > _target) out -= fall;
            return out;
        }
    };

    // 1 pole filter = opamp + capacitor (smoother)
    struct OnePoleSmoother 
    {    
        float inputs, outputs, lastOutput;
        float cutoff;
        OnePoleLP() {
            cutoff = 0.1;
            lastOutput = inputs = outputs = 0.0f;
        }

        ~OnePoleLP() {}

        // f = time = ms/sampleRate
        void setCutoff(float f) {
            cutoff = f;
        }
        double Tick(float target)
        {
            float p = (cutoff * 0.98f) * (cutoff * 0.98f) * (cutoff * 0.98f) * (cutoff * 0.98f);
            outputs = (1.0f - p) * (target) + p * outputs;            
            return outputs;
        }
    }

    struct MultiPoleSmoother
    {
        std::vector<OnePoleSmoother> smoothers;

        MultiPoleSmoother(size_t n)
        {
            
        }
        void setCutoff(float f) {
            for(auto i = smoothers.begin(); i != smoothers.end(); i++)
                (*i)->setCutoff(f);
        }
        double Tick(float target)
        {
            double out = target;
            for(auto i = smoothers.begin(); i != smoothers.end(); i++)
                out = (*i)->Tick(out);
            return out;
        }
    };
};
}