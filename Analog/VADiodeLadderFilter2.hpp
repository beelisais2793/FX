#pragma once

#include "MusicFunctions.hpp"
#include "Undenormal.hpp"
#include "FX/Diode.hpp"

namespace Analog::Filters::DiodeLadderFilter2
{
    struct VirtualAnalogDiodeLadderFilter
    {
        DspFloatType eta = 1.836f;
        DspFloatType VT = 0.0260f;
        DspFloatType gamma = eta * VT;
        DspFloatType C = 1.0e-7f;
        DspFloatType Mp = 1.0e-4f;
        
        DspFloatType VC1, VC2, VC3, VC4;
        DspFloatType u1, u2, u3, u4, u5;
        DspFloatType s1, s2, s3, s4;
        DspFloatType Vin;
        DspFloatType Vout;
        DspFloatType VoutPrev;
        DspFloatType Fs;
        DspFloatType inputFs;

        int iteration = 0;
        int maxNrIterations = 100;
        DspFloatType biasParameter = 0.5;
        DspFloatType gainParameter = 0.5;

        VirtualAnalogDiodeLadderFilter(DspFloatType sr = 44100.0f)
        {
            VC1 = 0.0f;
            VC2 = 0.0f;
            VC3 = 0.0f;
            VC4 = 0.0f;
            u1 = 0.0f;
            u2 = 0.0f;
            u3 = 0.0f;
            u4 = 0.0f;
            u5 = 0.0f;
            s1 = 0.0f;
            s2 = 0.0f;
            s3 = 0.0f;
            s4 = 0.0f;
            Vout = 0.0f;
            VoutPrev = 0.0f;        
            Fs = sr;
            biasParameter = 1.0;
            gainParameter  = 0.5;
        }

        void setCutoff(DspFloatType c) {
            biasParameter = cv2freq(c);
        }
        void setResonance(DspFloatType q) {
            gainParameter = q*20*3.459431619;
        }
        enum {
                PORT_CUTOFF,
                PORT_Q,
            };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setResonance(v); break;
                default: printf("No port %d\n", port);
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            Undenormal noDenormals;            
                
            auto I0 = 8 * C * VT * 2 * Fs * tan((M_PI * biasParameter)/ Fs);
            DspFloatType K = gainParameter;
            Vin = I;
            iteration = 0;
            while (1) {
                u1 = tanh((Vin - VoutPrev) / (2 * VT));
                VC1 = (I0 / (4.0 * C * Fs)) * (u2 + u1) + s1;
                u2 = tanh((VC2 - VC1) / (2 * gamma));
                VC2 = (I0 / (4.0 * C * Fs) * (u3 - u2)) + s2;
                u3 = tanh((VC3 - VC2) / (2 * gamma));
                VC3 = (I0 / (4.0 * C * Fs) * (u4 - u3)) + s3;
                u4 = tanh((VC4 - VC3) / (2 * gamma));
                VC4 = (I0 / (4.0 * C * Fs) * (-u5 - u4)) + s4;
                u5 = tanh(VC4 / (6.0f * gamma));
                Vout = (K + 0.5f) * FX::Distortion::Diode::Diode(VC4);
                if (fabs(Vout - VoutPrev) >= Mp * fabs(VoutPrev) || iteration > maxNrIterations)
                {
                    VoutPrev = Vout;
                    break;
                }
                VoutPrev = Vout;
                iteration++;
            }
            s1 = 1 / (2 * Fs) * u1 + VC1;
            s2 = 1 / (2 * Fs) * u2 + VC2;
            s3 = 1 / (2 * Fs) * u3 + VC3;
            s4 = 1 / (2 * Fs) * u4 + VC4;
            
            return Vout;
        }    
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out, DspFloatType * A=NULL, DspFloatType * X=NULL, DspFloatType * Y=NULL)
        {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i],A != NULL? A[i]:1, X != NULL? X[i]:1, Y != NULL? Y[i]:1);
        }
    };
}