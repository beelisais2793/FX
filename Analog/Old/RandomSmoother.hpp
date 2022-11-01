#pragma once

namespace Analog
{
    struct CRandomSmoother {
        
        CRandomSmoother(double Fs, double Fc, double _min = 0, double _max=1 ) {
            a0 = 1.0; b1 = 0.0; z1 = 0.0;
            fs = Fs;
            max = _max;
            min = _min;
            setTime(Fc);        
            retarget();
        };    
        void retarget()
        {        
            target = ((float)rand() / (float)RAND_MAX) * (max-min) + min;
        }
        void setTime(float ms)
        {
            float Fc = ms;
            f = Fc/fs;        
            b1 = exp(-2.0 * M_PI *f);
            a0 = 1.0 - b1;
        }
        float process()
        {
            float in = target;
            z1 = in * a0 + z1 * b1;
            if(fabs(z1 - target) < 1e-3) retarget();
            if(z1 < min) z1 = min;
            if(z1 > max) z1 = max;
            return z1;
        }
        double max,min;
        double a0, b1, z1, f, fs;
        double target;
    };
}