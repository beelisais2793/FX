#pragma once

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::Moog::MoogFilter
{
    // Classes definition
    struct MoogFilter : public FilterProcessor
    {
        double s0, s1, s2, s3;
        double zi;
        double xx, y0, y1, y2, y3;
        double  fc,q,sr;
        double dbGain;

        MoogFilter(double Fs) : FilterProcessor()
        {
            sr = Fs;
            fc = 440.0f/sr;
            q  = 0.5;
            s0 = s1 = s2 = s3 = zi = xx = y0 = y1 = y2 =y3 = 0.0;
            dbGain = 0;
        }
        void setCutoff(float f) {
            fc = f/sr;
        }
        void setResonance(float r) {
            q = r;
        }

        double Tick(double sample, double A=1, double X=1, double Y=1)
        {
            Undenormal denormals;
            
            double f = tan(M_PI * fc/sampleRate);
            double r = (40/9) * q;
            double volume = pow(10, (dbGain/20));

            const double input=sample;
            
            // input with half delay, for non-linearities
            const double ih = 0.5f * (input + zi);

            // evaluate the non-linear gains
            // optimization of function calls 
            const double in0 = ih - r * s3;
            const double a0 = in0 * in0;
            const double t0 = f * (((a0 + 105)*a0 + 945) / ((15*a0 + 420)*a0 + 945));

            const double a1 = s0 * s0;
            const double t1 = f * (((a1 + 105)*a1 + 945) / ((15*a1 + 420)*a1 + 945));

            const double a2 = s1 * s1;
            const double t2 = f * (((a2 + 105)*a2 + 945) / ((15*a2 + 420)*a2 + 945));

            const double a3 = s2 * s2;
            const double t3 = f * (((a3 + 105)*a3 + 945) / ((15*a3 + 420)*a3 + 945));

            const double a4 = s3 * s3;
            const double t4 = f * (((a4 + 105)*a4 + 945) / ((15*a4 + 420)*a4 + 945));

            // This formula gives the main result
            double y3 = (s3*(1+t3) + s2*t3)*(1+t2);
            y3 = (y3 + t2*t3*s1)*(1+t1);
            y3 = (y3 + t1*t2*t3*(s0+t0*zi));
            y3 = y3 / ((1+t1)*(1+t2)*(1+t3)*(1+t4) + r*t0*t1*t2*t3);

            const double xx = t0 * (zi - r*y3);
            const double y0 = t1 * (s0 + xx) / (1+t1);			
            const double y1 = t2 * (s1 + y0) / (1+t2);
            const double y2 = t3 * (s2 + y1) / (1+t3);

            // update state
            s0 += 2 * (xx - y0);
            s1 += 2 * (y0 - y1);
            s2 += 2 * (y1 - y2);
            s3 += 2 * (y2 - t4*y3);

            zi = input;

            return y3*volume;                
        }

        void reset()
        {  	zi = 0;
            s0 = s1 = s2 = s3 = 0;
        }
        
    };
}
