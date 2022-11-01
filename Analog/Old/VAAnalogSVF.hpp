#pragma once

#include "SoundObject.hpp"

namespace Analog::Filters::AnalogSVF
{
    struct AnalogSVF : public FilterProcessor
    {    
        double fc,fs,q,K;
        double lp,hp,bp,ubp,shelf,notch,apf,peak;
        double z1,z2;

        AnalogSVF(double Fs, double Fc, double Q) : FilterProcessor() {
            fc = Fc;
            fs = Fs;
            q = Q;
            if(q == 0) q = 0.01;
            z1 = z2 = 0.0;
            K = 1;
        }
        void setCutoff(double f) {
            if(f < 0) f = 0;        
            fc = 0.995*f;
        }
        void setQ(double Q) {   
            if(q <= 0.0) q = 0.01;        
            q = Q;
        }
        double Tick(double I, double A = 1, double X = 1, double Y = 1, double Z = 1, double OA=1, double minC = -1, double maxC = 1)
        {        
            double wd = 2*M_PI*fc;
            double T  = 1/fs;
            double temp = X*wd*T/2;
            double wa;

            if(fabs(temp) != M_PI/2)
                wa = (2/T)*tan(temp);
            else
                wa = (2/T)*tan(temp*0.995);
            
            double g  = wa*T/2;
            double xn = A*I;
            double  R  = fabs(Y)*1.0/(2*(q));
            if(R == 0) R = 0.001;
            
            if(xn < minC) xn = minC;
            if(xn > maxC) xn = maxC;

            hp = (xn - (2*R+Z*g)*z1 - z2) / (1.0 + Z*2*R*g + Z*Z*g*g);
            bp = g*hp + z1;
            lp = g*bp + z2;        
            // not sure these work right yet
            ubp = 2 * R * bp;
            // dont know exactly what K is it's not explained
            shelf = xn + 2*K*R*bp;
            notch = xn - 2*R*bp;
            apf   = xn - 4*R*bp;
            peak  = lp - hp;

            // delay feedback
            z1 = g*hp + bp;
            z2 = g*bp + lp;
                    
            return tanh(OA*lp);
        }
        void ProcessBlock(size_t n, double * input, double * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, double * input) {
            ProcessBlock(n,input,input);
        }
    };

    
}