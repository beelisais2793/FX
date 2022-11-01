#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Drums
{
    struct AnalogBassDrum : public GeneratorProcessorPlugin<daisyp::AnalogBassDrum>
    {        
        AnalogBassDrum() : GeneratorProcessorPlugin<daisyp::AnalogBassDrum>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_TRIG,            
            PORT_SUSTAIN,
            PORT_ACCENT,
            PORT_FREQ,
            PORT_TONE,
            PORT_DECAY,
            PORT_ATTACKFM,
            PORT_SELFFM,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIG: this->Trig(); break;                
                case PORT_SUSTAIN: this->SetSustain(v); break;
                case PORT_ACCENT: this->SetAccent(v); break;
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_TONE: this->SetTone(v); break;
                case PORT_DECAY: this->SetDecay(v); break;
                case PORT_ATTACKFM: this->SetAttackFmAmount(v); break;
                case PORT_SELFFM: this->SetSelfFmAmount(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1 )
        {
            return A*this->Process(I);
        }        
    };
}