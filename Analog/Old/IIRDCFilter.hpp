#pragma once

namespace Filters::IIR
{
    class CDCFilter : public FilterProcessor {
    public:
        
        CDCFilter(double Fs=44100.0f, double Fc=10)
        : FilterProcessor()
        {
            a0 = 1.0; b1 = 0.0; z1 = 0.0;
            setCutoff(Fc/Fs);
        };
        
        void setCutoff(double Fc)
        {
            b1 = exp(-2.0 * M_PI * Fc);
            a0 = 1.0 - b1;
        }
        double process(double in)
        {
            z1 = in * a0 + z1 * b1;
            return in - z1;
        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return process(I);
        }
        
    protected:    
        double a0, b1, z1;
    };


    class CDCRemover : public FilterProcessor {
    public:

        CDCRemover(double Fs=44100.0f, double Fc=10.0f) : FilterProcessor() {
            a0 = 1.0; b1 = 0.0; z1 = 0.0;
            setCutoff(Fc/Fs);
        };
        
        // this is a high pass
        void setCutoff(double Fc)
        {
            b1 = -exp(-2.0 * M_PI * (0.5 - Fc));
            a0 = 1.0 + b1;
        }
        double process(double in)
        {
            return z1 = in * a0 + z1 * b1;
        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return process(I);
        }
    protected:    
        double a0, b1, z1;
    };
}
