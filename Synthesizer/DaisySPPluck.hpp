#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct Pluck : public GeneratorProcessorPlugin<daisysp::Pluck>
    {
        float * memory;
        float trig = 0;
        Pluck(size_t n, int mode = PLUCK_MODE_RECURSIVE) {
            memory = new float[n];
            this->Init(sampleRate,memory,n,mode);
        }
        ~Pluck() {
            if(memory) delete [] memory;
        }
        enum {
            PORT_AMP,
            PORT_FREQ,
            PORT_DECAY,
            PORT_DAMP,
            PORT_MODE, 
            PORT_TRIG,           
        };
        void setPort(int port, double v) {
            switch(port)
            {
                case PORT_AMP: this->SetAmp(v); break;
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_DECAY: this->SetDecay(v); break;
                case PORT_DAMP: this->SetDamp(v); break;
                case PORT_MODE: this->SetMode(v); break;
                case PORT_TRIG: trig = v; break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_TRIG: return trig;
            }
            return 0.0;
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process(trig);
        }
    };
}