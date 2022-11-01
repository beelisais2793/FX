#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct OscillatorBank : public GeneratorProcessorPlugin<daisysp::OscillatorBank>
    {
        OscillatorBank() : GeneratorProcessorPlugin<daisysp::OscillatorBank>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_AMP,
            PORT_GAIN,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_GAIN: this->SetGain(v); break;
            }
        }
        void setPort2(int port, double x, double y) {
            switch(port) {
                case PORT_AMP: this->SetSingleAmp(x,y);
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}