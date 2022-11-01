#pragma once

#include "SoundObject.hpp"

namespace Analog::Filters::SVF
{

    // needs smoothing
    struct AnalogSVF : public FilterProcessor
    {    
        DspFloatType fc,fs,q,K;
        DspFloatType lp,hp,bp,ubp,shelf,notch,apf,peak;
        DspFloatType z1,z2;

        enum {
            LP,
            HP,
            BP,
            UBP,
            SHELF,
            NOTCH,
            APF,
            PEAK,
            LAST
        };
        int type = LP;

        AnalogSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) : FilterProcessor() {
            fc = Fc;
            fs = Fs;
            q = Q;
            if(q == 0) q = 0.01;
            z1 = z2 = 0.0;
            K = 1;
        }
        void setCutoff(DspFloatType f) {
            if(f < 0) f = 0;        
            fc = 0.995*f;
        }
        void setQ(DspFloatType Q) {   
            if(q <= 0.0) q = 0.01;        
            q = Q;
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
            PORT_TYPE
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_TYPE: type = ((int)v) % LAST; break;
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 1, DspFloatType Y = 1)
        {        
            DspFloatType wd = 2*M_PI*fc;
            DspFloatType T  = 1/fs;
            DspFloatType temp = X*wd*T/2;
            DspFloatType wa;

            if(fabs(temp) != M_PI/2)
                wa = (2/T)*tan(temp);
            else
                wa = (2/T)*tan(temp*0.995);
            
            DspFloatType g  = wa*T/2;
            DspFloatType xn = A*I;
            DspFloatType  R  = fabs(Y)*1.0/(2*(q));
            if(R == 0) R = 0.001;
                        
            hp = (xn - (2*R+g)*z1 - z2) / (1.0 + 2*R*g + g*g);
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

            DspFloatType out;
            switch(type) {
                case LP: out = lp; break;
                case HP: out = hp; break;
                case BP: out = bp; break;
                case UBP: out = ubp; break;
                case SHELF: out = shelf; break;
                case NOTCH: out = notch; break;
                case APF: out = apf; break;
                case PEAK: out = peak; break;
                default: out = lp; break;                
            }
            return tanh(A*out);
        }
        void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, DspFloatType * input) {
            ProcessBlock(n,input,input);
        }
    };

    struct StereoSVF : public StereoFXProcessor
    {
        AnalogSVF left,right;

        StereoSVF(DspFloatType sr) : 
            StereoFXProcessor(),
            left(sr,1000.0f,0.5),
            right(sr,1000.0f,0.5)
        {

        }
        void setCutoff(DspFloatType f) {
            left.setCutoff(f);
            right.setCutoff(f);
        }
        void setQ(DspFloatType q) {
            left.setQ(q);
            right.setQ(q);
        }
        void ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            left.ProcessBlock(n,inputs[0],outputs[0]);
            right.ProcessBlock(n,inputs[1],outputs[1]);
        }
    };
}