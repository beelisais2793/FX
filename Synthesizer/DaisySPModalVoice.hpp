#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct ModalVoice : public GeneratorProcessorPlugin<daisysp::ModalVoice>
    {
        ModalVoice() : GeneratorProcessorPlugin<daisysp::ModalVoice>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_TRIG,
            PORT_SUSTAIN,
            PORT_FREQ,
            PORT_ACCENT,
            PORT_STRUCTURE,
            PORT_BRIGHTNESS,
            PORT_DAMP,
            PORT_AUX
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIG: this->Trig(); break;
                case PORT_SUSTAIN: this->Sustain(v); break;
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_ACCENT: this->SetAccent(v); break;
                case PORT_STRUCTURE: this->SetStructure(v); break;
                case PORT_BRIGHTNESS: this->SetBrightness(v); break;
                case PORT_DAMP: this->SetDamping(v); break;                
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1) {
            return A* this->Process(I);
        }
    };
}