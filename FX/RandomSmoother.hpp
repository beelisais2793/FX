#pragma once

namespace Analog
{
    struct CRandomSmoother {
        
        CRandomSmoother(DspFloatType Fs, DspFloatType Fc, DspFloatType _min = 0, DspFloatType _max=1 ) {
            a0 = 1.0; b1 = 0.0; z1 = 0.0;
            fs = Fs;
            max = _max;
            min = _min;
            setTime(Fc);        
            retarget();
        };    
        void retarget()
        {        
            target = ((DspFloatType)rand() / (DspFloatType)RAND_MAX) * (max-min) + min;
        }
        void setTime(DspFloatType ms)
        {
            DspFloatType Fc = ms;
            f = Fc/fs;        
            b1 = exp(-2.0 * M_PI *f);
            a0 = 1.0 - b1;
        }
        DspFloatType process()
        {
            DspFloatType in = target;
            z1 = in * a0 + z1 * b1;
            if(fabs(z1 - target) < 1e-3) retarget();
            if(z1 < min) z1 = min;
            if(z1 > max) z1 = max;
            return z1;
        }
        DspFloatType max,min;
        DspFloatType a0, b1, z1, f, fs;
        DspFloatType target;
    };
}