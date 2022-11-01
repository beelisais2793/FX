#pragma once

#include <cstdint>
#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::VCS3Filter
{
    // Classes definition
    class VCS3Filter : public FilterProcessor
    {
        DspFloatType s0, s1, s2, s3;
        DspFloatType zi;
        DspFloatType xx, y0, y1, y2, y3;
        DspFloatType sr;
        DspFloatType fc,q,volume;

        VCS3Filter(DspFloatType Fs=44100.0f)
        {
            sr = Fs;
            s0=s1=s2=s3=0;
            zi = 0;
            xx=y0=y1=y2=y3=0;

            volume = pow(10,0.5/20);
            fc     = 440.0f;
            q      = 0.5;
        }
        void setCutoff(DspFloatType f) {
            fc = f;
        }
        void setResonance(DspFloatType res) {
            q  = res;
        }
        void setVolume(DspFloatType db) {
            volume = pow(10,db/20.0f);
        }
        enum {
            PORT_CUTOFF,
            PORT_RESONANCE,
            PORT_VOLUME,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_RESONANCE: setResonance(v); break;
                case PORT_VOLUME: setVolume(v); break;
            }
        }
        DspFloatType Tick(DspFloatType sample, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            Undenormal denormal;
            
            DspFloatType f = tan(M_PI * fc/sampleRate);
            DspFloatType r = 7 * q + 0.5;
            

            const DspFloatType input=sample;
            
            // input with half delay, for non-linearities
            const DspFloatType ih = 0.5f * (input + zi);

            // gains
            DspFloatType g0 = 1.f;
            DspFloatType g1 = 1.836f;
            DspFloatType g2 = 3*1.836f;

            DspFloatType g0inv = 1.f/g0;
            DspFloatType g1inv = 1.f/g1;
            DspFloatType g2inv = 1.f/g2;

            // optimization of function calls 
            const DspFloatType in0 = (ih - r * s3)*g0inv;
            const DspFloatType a0 = in0 * in0;
            const DspFloatType t0 = g0inv * f * (((a0 + 105)*a0 + 945) / ((15*a0 + 420)*a0 + 945));

            const DspFloatType in1 = (s1-s0)*g1inv;
            const DspFloatType a1 = in1 * in1;
            const DspFloatType t1 = g1inv * f * (((a1 + 105)*a1 + 945) / ((15*a1 + 420)*a1 + 945));

            const DspFloatType in2 = (s2-s1)*g1inv;
            const DspFloatType a2 = in2 * in2;
            const DspFloatType t2 = g1inv * f * (((a2 + 105)*a2 + 945) / ((15*a2 + 420)*a2 + 945));

            const DspFloatType in3 = (s3-s2)*g1inv;
            const DspFloatType a3 = in3 * in3;
            const DspFloatType t3 = g1inv * f * (((a3 + 105)*a3 + 945) / ((15*a3 + 420)*a3 + 945));

            const DspFloatType in4 = (s3)*g2inv;
            const DspFloatType a4 = in4 * in4;
            const DspFloatType t4 = g2inv * f * (((a4 + 105)*a4 + 945) / ((15*a4 + 420)*a4 + 945));

            // This formula gives the result for y3 thanks to MATLAB
            DspFloatType y3 = (s2 + s3 + t2*(s1 + s2 + s3 + t1*(s0 + s1 + s2 + s3 + t0*zi)) + t1*(2*s2 + 2*s3))*t3 + s3 + 2*s3*t1 + t2*(2*s3 + 3*s3*t1);
            y3 /= (t4 + t1*(2*t4 + 4) + t2*(t4 + t1*(t4 + r*t0 + 4) + 3) + 2)*t3 + t4 + t1*(2*t4 + 2) + t2*(2*t4 + t1*(3*t4 + 3) + 2) + 1;

            // Other outputs
            DspFloatType y2 = (s3 - (1+t4+t3)*y3) / (-t3);
            DspFloatType y1 = (s2 - (1+t3+t2)*y2 + t3*y3) / (-t2);
            DspFloatType y0 = (s1 - (1+t2+t1)*y1 + t2*y2) / (-t1);
            DspFloatType xx = (zi - r*y3);

            // update state
            s0 += 2 * (t0*xx + t1*(y1-y0));
            s1 += 2 * (t2*(y2-y1) - t1*(y1-y0));
            s2 += 2 * (t3*(y3-y2) - t2*(y2-y1));
            s3 += 2 * (-t4*(y3) - t3*(y3-y2));

            zi = input;

            return y3*r*volume;        
        }
        void reset()
        {  	zi = 0;
            s0 = s1 = s2 = s3 = 0;
        }
        
    };
}