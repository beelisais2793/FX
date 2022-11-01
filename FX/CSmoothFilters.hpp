#pragma once

#include <cmath>


namespace FX::Filters::Smoothers
{
    // is just the one pole again
    class CSmoothFilter : public FilterProcessor
     {
    public:
                
        CSmoothFilter(DspFloatType Fs=44100.0f, DspFloatType Fc=0.1f) : FilterProcessor() {
            a0 = 1.0; b1 = 0.0; z1 = 0.0;
            fs = Fs;            
            setCutoff(Fc);        
        };
        
        void setCutoff(DspFloatType Fc)
        {            
            f = Fc/fs;                    
            b1 = exp(-2.0 * M_PI *f);
            a0 = 1.0 - b1;
        }
        void setTarget(DspFloatType t)
        {
            target = t;
        }
        bool isAtTarget() {
            return fabs(z1 - target) < 1e-3;
        }	    	
        DspFloatType process()
        {
            return z1 = target * a0 + z1 * b1;
        }        
        DspFloatType process(DspFloatType in) {            
            return z1 = in * a0 + z1 * b1;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return process(I);
        }
    protected:    
        DspFloatType a0, b1, z1, f, fs;
        DspFloatType target = 1.0;
    };

    class CParamSmooth : public FilterProcessor
    {
    public:
        CParamSmooth() : FilterProcessor() { a = 0.99f; b = 1.f - a; z = 0; };
        ~CParamSmooth();
        inline DspFloatType Process(DspFloatType in) { z = (in * b) + (z * a); return z; }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return Process(I);
        }
    private:
        DspFloatType a, b, z;
    };

    class CSmoothTime : public FilterProcessor
    {
    public:

        CSmoothTime(DspFloatType smoothingTimeInMs, DspFloatType samplingRate)
        : FilterProcessor()
        {
            const DspFloatType c_twoPi = 6.283185307179586476925286766559f;

            a = exp(-c_twoPi / (smoothingTimeInMs * 0.001f * samplingRate));
            b = 1.0f - a;
            z = 0.0f;
        }

        ~CSmoothTime()
        {

        }

        inline DspFloatType process(DspFloatType in)
        {
            z = (in * b) + (z * a);
            return z;
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return process(I);
        }
    private:
        DspFloatType a;
        DspFloatType b;
        DspFloatType z;
    };
}