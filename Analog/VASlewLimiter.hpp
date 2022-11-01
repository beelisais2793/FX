#pragma once

namespace Analog::Calculus::RateLimiters
{
    struct SlewLimiter : public FunctionProcessor {
        DspFloatType _delta;
        DspFloatType _last = 0.0f;

        SlewLimiter(DspFloatType sampleRate = 1000.0f, DspFloatType milliseconds = 1.0f, DspFloatType range = 10.0f) : FunctionProcessor() {
            setParams(sampleRate, milliseconds, range);
        }

        void setParams(DspFloatType sampleRate = 1000.0f, DspFloatType milliseconds = 1.0f, DspFloatType range = 10.0f);
        inline void setLast(DspFloatType last) { _last = last; }
        inline DspFloatType next(DspFloatType sample) {
            return _last = next(sample, _last);
        }
        DspFloatType next(DspFloatType sample, DspFloatType last);

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return next(I,_last);
        }
    };

    void SlewLimiter::setParams(DspFloatType sampleRate, DspFloatType milliseconds, DspFloatType range) {
        assert(sampleRate > 0.0f);
        assert(milliseconds >= 0.0f);
        assert(range > 0.0f);
        _delta = range / ((milliseconds / 1000.0f) * sampleRate);
    }

    DspFloatType SlewLimiter::next(DspFloatType sample, DspFloatType last) {
        if (sample > last) {
            return std::min(last + _delta, sample);
        }
        return std::max(last - _delta, sample);
    }


    struct Slew : public FunctionProcessor
    {
        DspFloatType rise,fall;
        DspFloatType target,out;
        DspFloatType sr;
        Slew(DspFloatType hz,DspFloatType fs) : FunctionProcessor()
        {
            sr = fs;
            rise = hz/sr;
            fall = hz/sr;
            target = 1.0;
            out = 0;
        }
        void setRise(DspFloatType hz) {
            rise = hz/sr;
        }
        void setFall(DspFloatType hz) {
            fall = hz/sr;
        }
        void setTarget(DspFloatType t) {
            target = t;
        }
        enum {
            PORT_RISE,
            PORT_FALL,
            PORT_TARGET,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_RISE: setRise(v); break;
                case PORT_FALL: setFall(v); break;
                case PORT_TARGET: setTarget(v); break;
            }
        }
        DspFloatType Tick() {
            //out = clamp(target, out - fall, out + rise);
            if(fabs(out - target) < 1e-3) return target;
            if(out < target) out += rise;
            if(out > target) out -= fall;
            return out;
        }
        DspFloatType Tick(DspFloatType _target, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            //out = clamp(_target, out - fall, out + rise);
            if(fabs(out - _target) < 1e-3) return _target;
            if(out < _target) out += rise;
            if(out > _target) out -= fall;
            return out;
        }
    };
}
