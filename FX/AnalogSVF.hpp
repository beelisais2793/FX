#pragma once

#include "SoundObject.hpp"

namespace FX::Filters::AnalogSVF
{

    // needs smoothing
    struct AnalogSVF : public FilterProcessor
    {    
        DspFloatType fc,fs,q,K;
        DspFloatType lp,hp,bp,ubp,shelf,notch,apf,peak;
        DspFloatType z1,z2;
        
        enum
        {
            LOWPASS,
            HIGHPASS,
            BANDPASS,
            UBANDPASS,
            SHELF,
            NOTCH,
            ALLPASS,
            PEAK
        };
        int type = LOWPASS;

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
            PORT_TYPE,
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_CUTOFF=0\n");
            printf("PORT_Q=1\n");
            printf("PORT_TYPE=2\n");
        }
        void setPort(int port, DspFloatType v)
        {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_TYPE: type = (int)v % 8; break;
            }
        }
        DspFloatType getPort(int port) {
            switch(port) {
                case PORT_CUTOFF: return fc;
                case PORT_Q: return q;
                case PORT_TYPE return type;
            }
            return 0;
        }
        DspFloatType Compute(DspFloatType I, DspFloatType A = 1, DspFloatType X = 1, DspFloatType Y = 1, DspFloatType Z = 1, DspFloatType OA=1, DspFloatType minC = -1, DspFloatType maxC = 1)
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

            switch(type)            
            {
            case LOWPASS:
            default: return tanh(OA*lp);
            case HIGHPASS: return tanh(QA*hp);
            case BANDPASS: return tanh(QA*bp);
            case UBANDPASS: return tanh(QA*ubp);
            case SHELF: return tanh(QA*shelf);
            case NOTCH: return tanh(QA*notch);
            case ALLPASS: return tanh(QA*apf);
            case PEAK: return tanh(QA*peak);
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return Compute(I,A,X,Y);
        }
        void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, DspFloatType * input) {
            ProcessBlock(n,input,input);
        }
    };

    struct AnalogHighPassSVF : public AnalogSVF
    {
        AnalogHighPassSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->hp;
        }
    };

    struct AnalogLowPassSVF : public AnalogSVF
    {
        AnalogLowPassSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->lp;
        }
    };

    struct AnalogBandPassSVF : public AnalogSVF
    {
        AnalogBandPassSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->bp;
        }
    };

    struct AnalogBandPass2SVF : public AnalogSVF
    {
        AnalogBandPass2SVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->ubp;
        }
    };

    struct AnalogShelfSVF : public AnalogSVF
    {
        AnalogShelfSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->shelf;
        }
    };

    struct AnalogNotchSVF : public AnalogSVF
    {
        AnalogNotchSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->botch;
        }
    };

    struct AnalogAllPassSVF : public AnalogSVF
    {
        AnalogAllPassSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->ap;
        }
    };

    struct AnalogPeakSVF : public AnalogSVF
    {
        AnalogPeakSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) 
        : AnalogSVF(Fs,Fc,Q)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            this->Compute(I,A,X,Y);
            return this->peak;
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