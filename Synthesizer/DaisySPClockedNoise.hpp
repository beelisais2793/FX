#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct ClockedNoise : public GeneratorProcessorPlugin<daisysp::ClockedNoise>
    {
        ClockedNoise() : GeneratorProcessorPlugin<daisysp::ClockedNoise>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FREQ,
            PORT_SYNC,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_SYNC: this->Sync(); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {            
            return A*this->Process(I);
        }        
    };
}