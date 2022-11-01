#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Control
{
    struct ADenv : public GeneratorProcessorPlugin<daisyp::AdEnv>
    {
        ADenv() : GeneratorProcessorPlugin<daisyp::AdEnv>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_TRIGGER,
            PORT_ATTACK,
            PORT_DECAY,
            PORT_CURVE,
            PORT_MIN,
            PORT_MAX,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TRIGGER: if(v == 1.0) this->Trigger(); break;
                case PORT_ATTACK: this->SetTime(daisysp::AdEnvSegment::ADENV_ATTACK,v); break;
                case PORT_DECAY: this->SetTime(daisysp::AdEnvSegment::ADENV_DECAY,v); break;
                case PORT_CURVE: this->SetCurve(v); break;
                case PORT_MIN: this->SetMin(v); break;
                case PORT_MAX: this->SetMax(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return this->Process();
        }
    };
}