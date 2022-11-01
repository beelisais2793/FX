#pragma once

namespace Analog::Calculus::RateLimiters
{
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
}
