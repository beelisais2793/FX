#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct Metronome : public GeneratorProcessorPlugin<daisysp::Metro>
    {
        std::function<void (Metronome*)> callback = [](Metronome* ptr){};
        Metronome(double freq) : GeneratorProcessorPlugin<daisysp::Metro>()
        {
            this->Init(freq,sampleRate);
        }
        enum {
            PORT_RESET,
            PORT_FREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_RESET: this->Reset(); break;
                case PORT_FREQ: this->SetFreq(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            if(this->Process()) callback(this);
            return A*I;
        }
    };
}