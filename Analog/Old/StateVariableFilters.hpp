#pragma once
#include <cmath>
#include "FX/ClipFunctions.hpp"

namespace Filters::StateVariable
{
    struct ChamberlinSVF
    {
        /*
        //Input/Output
        I - input sample
        L - lowpass output sample
        B - bandpass output sample
        H - highpass output sample
        N - notch output sample
        F1 - Frequency control parameter
        Q1 - Q control parameter
        D1 - delay associated with bandpass output
        D2 - delay associated with low-pass output
        */
        float x,L,B,H,N,F1,Q1,D1,D2;
        float Fc,Fs,R;
        ChamberlinSVF(float sr, float fc, float q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(float fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(float r) { R = r; Q1 = 1.0-r; }

        float Tick(float I, float A=1, float X=0, float Y=0)
        {    
            Undenormal denormal;
            x = I;
            // algorithm
            // loop
            L = D2 + F1 * D1;
            H = I - L - Q1*D1;
            B = F1 * H + D1;
            N = H + L;

            // store delays
            D1 = B;
            D2 = L;

            // outputs
            //L,H,B,N
            return L;
        }
    };

    // it should be oversampled
    struct AnalogSVF
    {    
        double fc,fs,q,K;
        double lp,hp,bp,ubp,shelf,notch,apf,peak;
        double z1,z2;

        AnalogSVF(double Fs, double Fc, double Q) {
            fc = Fc;
            fs = Fs;
            q = Q;
            if(q == 0) q = 0.001;
            z1 = z2 = 0.0;
            K = 1;
        }
        void setCutoff(float f) {
            fc = f;
        }
        void setQ(float Q) {   
            if(q <= 0.0) q = 0.001;        
            q = Q;
        }
        double Tick(double I, double A = 1, double X = 0, double Y = 0)
        {
            double wd = 2*M_PI*fc;
            double T  = 1/fs;
            double wa = (2/T)*tan(wd*T/2);
            double g  = wa*T/2;
            double xn = I;
            float  R  = 1.0/(2*(q));
            
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
            
            return lp;
        }
        void ProcessBlock(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, float * input) {
            ProcessBlock(n,input,input);
        }
    };

    struct StereoSVF
    {
        AnalogSVF left,right;

        StereoSVF(float sr) : 
            left(sr,1000.0f,0.5),
            right(sr,1000.0f,0.5)
        {

        }
        void setCutoff(float f) {
            left.setCutoff(f);
            right.setCutoff(f);
        }
        void setQ(float q) {
            left.setQ(q);
            right.setQ(q);
        }
        void ProcessBlock(size_t n, float ** inputs, float ** outputs)
        {
            left.ProcessBlock(n,inputs[0],outputs[0]);
            right.ProcessBlock(n,inputs[1],outputs[1]);

        }
    };

    struct StateVariableFilter
    {
        /*
        cutoff = cutoff freq in Hz
        fs = sampling frequency //(e.g. 44100Hz)
        f = 2 sin (pi * cutoff / fs) //[approximately]
        q = resonance/bandwidth [0 < q <= 1]  most res: q=1, less: q=0
        low = lowpass output
        high = highpass output
        band = bandpass output
        notch = notch output
        */
        float cutoff,scale,fs,low,high,band,notch;
            
        StateVariableFilter(float Fc, float Fs, float Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(float F) { cutoff = F; }
        void setResonance(float R) { scale = 1.25*(1.0-R); }
        float Tick(float I, float A = 1, float X=0, float Y=0)
        {
            Undenormal denormal;
            float f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            //I = tanhify(I);
            low = low + f * band;
            high = scale * I - low - scale*band;
            band = f * high + band;
            notch = high + low;
            return low;
        }
    };

}
