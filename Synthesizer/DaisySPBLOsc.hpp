#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct BLOsc : public GeneratorProcessorPlugin<daisysp::BLOsc>
    {
        BLOsc() : GeneratorProcessorPlugin<daisysp::BLOsc>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_WAVEFORM,
            PORT_FREQ,
            PORT_AMP,
            PORT_PW,
            PORT_RESET
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_WAVEFORM: this->SetWaveform((uint8_t)v); break;
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_AMP: this->SetAmp(v); break;
                case PORT_PW: this->SetPw(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {            
            return A*this->Process(I);
        }        
    };
}