#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct BiQuad : public FilterProcessorPlugin<stk::BiQuad>
    {
        BiQuad() : FilterProcessorPlugin<stk::BiQuad>()
        {
            this->setAllPass(1000.0,0.5);
        }
        enum {            
            PORT_A1,
            PORT_A2,
            PORT_B0,
            PORT_B1,
            PORT_B2,
            PORT_RESONANCE,
            PORT_NOTCH,
            PORT_LOWPASS,
            PORT_HIGHPASS,
            PORT_BANDPASS,
            PORT_BANDREJECT,
            PORT_ALLPASS,
            PORT_EQUALGAINZEROS,
            PORT_LASTOUT
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_A1: this->setA1(v); break;
                case PORT_A2: this->setA2(v); break;
                case PORT_B0: this->setB0(v); break;
                case PORT_B1: this->setB1(v); break;
                case PORT_B2: this->setB2(v); break;
                case PORT_EQUALGAINZEROS: this->setEqualGainZeros(); break;
                case PORT_LOWPASS: this->setLowPass(v); break;
                case PORT_HIGHPASS: this->setHighPass(v); break;
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_RESONANCE: this->setResonance(a,b,true); break;
                case PORT_NOTCH: this->setNotch(a,b); break;
                case PORT_LOWPASS: this->setLowPass(a,b); break;
                case PORT_HIGHPASS: this->setHighPass(a,b); break;
                case PORT_BANDPASS: this->setBandPass(a,b); break;
                case PORT_BANDREJECT: this->setBandReject(a,b); break;
                case PORT_ALLPASS: this->setAllPass(a,b); break;
                
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);
        }
    };
}