#pragma once

#include "Stk.hpp"
#include "SoundObject.hpp"

namespace STK1000
{
    struct BiQuad : public FilterProcessor
    {
        stk::BiQuad * biquad;
        double cutoff,q;

        enum {
            Lowpass,
            Highpass,
            Bandpass,
            Notch,
            BandReject,
            Allpass
        };
        
        int type = Lowpass;

        BiQuad() : FilterProcessor() {
            biquad = new stk::BiQuad;
            cutoff = 1000.0f;
            q = 0.5;
        }
        ~stkBiQuad() {
            if(biquad) delete biquad;
        }

        void setCoefficients(double b0, double b1, double b2, double a1, double a2, bool clear = false) {
            biquad->setCoefficients(b0,b1,b2,a1,a2,clear);
        }
        void setB0(double b0) { 
            biquad->setB0(b0);
        }
        void setB1(double b1) { 
            biquad->setB1(b1);
        }
        void setB2(double b2) { 
            biquad->setB2(b2);
        }
        void setA1(double a1) { 
            biquad->setA1(a1);
        }
        void setA2(double a2) { 
            biquad->setA2(a2);
        }
        void setResonance(double f, double r, bool n = false) {
            biquad->setResonance(f,r,n);
        }
        void setNotch(double f, double r) {
            type = Notch;
            biquad->setNotch(f,r);
        }
        void setLowPass(double f, double Q=RECIP_SQRT_2) {
            type = Lowpass;
            biquad->setLowPass(f,Q);
        }
        void setHighPass(double f, double Q=RECIP_SQRT_2) {
            type = Highpass;
            biquad->setHighPass(f,Q);
        }
        void setBandPass(double f, double Q=RECIP_SQRT_2) {
            type = Bandpass;
            biquad->setBandPass(f,Q);
        }
        void setBandReject(double f, double Q=RECIP_SQRT_2) {
            type = Bandreject;
            biquad->setBandReject(f,Q);
        }
        void setAllPass(double f, double Q=RECIP_SQRT_2) {
            type = Allpass;
            biquad->setAllPass(f,Q);
        }
        void setEqualGainZeros() {
            biquad->setEqualGainZeros();
        }
        double lastOut() { 
            return biquad->lastOut(); 
        }
        void setCutoff(float f) {
            switch(type)
            {
                case Lowpass: setLowPass(f,q); break;
                case Highpass: setHighPass(f,q); break;
                case Bandpass: setBandPass(f,q); break;
                case BandReject: setBandReject(f,q); break;
                case Allpass: setAllPass(f,q); break;
                case Notch: setNotch(f,q); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            double tc = cutoff;
            double tq = q;
            setCutoff(cutoff*X);
            setResonance(q*Y);
            double out = biquad->tick(I);
            setCutoff(tc);
            setResonance(q);
            return A*out;
        }
    };
}