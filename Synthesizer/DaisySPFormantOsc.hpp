#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct FormantOsc : public GeneratorProcessorPlugin<daisysp::FormantOsc>
    {
        FormantOsc() : GeneratorProcessorPlugin<daisysp::FormantOsc>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FORMANT_FREQ,
            PORT_CARRIER_FREQ,
            PORT_PHASE_SHIFT,
        };
        void setPort(int port, double v)
        {
            switch(port) {
            case PORT_FORMANT_FREQ: this->SetFormantFreq(v); break;
            case PORT_CARRIER_FREQ: this->SetCarrierFreq(v); break;
            case PORT_PHASE_SHIFT: this->SetPhaseShift(v); break;
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}