#pragma once
#include "Amplifiers.hpp"

namespace FX::Distortion
{
    struct Udo1 : public AmplifierProcessor
    {
        DspFloatType gain = 1.0;
        Udo1(DspFloatType g = 1) : AmplifierProcessor()
        {
            setGain(g);
        }
        void setGain(DspFloatType a) {
            gain = pow(10.0,a/20.0);
        }
        enum {
            PORT_GAIN
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_GAIN) setGain(v); 
            else printf("No port %d\n",port);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            return udo1(I,A*gain);
        }
    };
}