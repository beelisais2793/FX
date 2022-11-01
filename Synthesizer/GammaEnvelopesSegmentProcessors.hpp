#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Envelopes
{

    struct Seg : public GeneratorProcessorPlugin<gam::Seg>
    {
        Seg(double len=0.5, double start=1, double end=0, double phase=0) :
            GeneratorProcessorPlugin<gam::Seg>,
            gam::Seg(len,start,end,phase)
        {
            
        }
        enum {
            PORT_FREQ,
            PORT_LENGTH,
            PORT_PERIOD,
            PORT_PHASE,
            PORT_RESET,
            PORT_DONE,
            PORT_VAL,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->freq(v); break;
                case PORT_LENGTH: this->length(v); break;
                case PORT_PERIOD: this->period(v); break;
                case PORT_PHASE: this->phase(v); break;
                case PORT_RESET: this->reset(); break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_DONE: return this->done();
                case PORT_VAL: return this->val();
            }
            return 0.0;
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return (*this)();
        }
    };

    struct SegExp : public GeneratorProcessorPlugin<gam::SegExp>
    {
        SegExp(double len=0.5, double crv=-3, double start=1, double end=0) :
            GeneratorProcessorPlugin<gam::Seg>,
            gam::Seg(len,crv,start,end)
        {
            
        }
        enum {
            PORT_DONE,
            PORT_CURVE,
            PORT_PERIOD,
            PORT_RESET,            
            PORT_SET,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PERIOD: this->period(v); break;
                case PORT_RESET: this->reset(); break;
                case PORT_CURVE: this->curve(v); break;
            }
        }
        void setPort2(int port, double x, double y) {
            switch(port) {
                case PORT_SET: this->set(x,y); break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_DONE: return this->done();                
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return (*this)();
        }
    };
}

}