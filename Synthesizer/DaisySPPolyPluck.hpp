#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct PolyPluck : public GeneratorProcessorPlugin<daisysp::PolyPluck>
    {
        float trig=0;
        float note=0;
        PolyPluck() : GeneratorProcessorPlugin<daisysp::PolyPluck>()
        {
            this->Init(sampleRate);
        }
    };
    enum {
        PORT_DECAY,
        PORT_TRIG,
        PORT_NOTE,
    };
    void setPort(int port, double v) {
        switch(port) {
            case PORT_DECAY: this->SetDecay(v); break;
            case PORT_TRIG: trig = v; break;
            case PORT_NOTE: note = v; break;
        }
    }
    double getPort(int port) {
        switch(port) {
            case PORT_TRIG: return trig;                
        }
        return 0.0;
    }

    double Tick(double I=1, double A=1, double X=1, double Y=1) {
        return A*this->Process(trig,note);
    }
}