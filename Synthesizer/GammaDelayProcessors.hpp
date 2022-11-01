#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Delay
{
    struct Delay1 : public FunctionProcessorPlugin<gam::Delay1<double>>
    {
        Delay1(double value=0) 
        : FunctionProcessorPlugin<gam::Delay1<double>>(),
          gam::Delay1<double>(value),
        {

        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return (*this)(I);
        }
    };

    struct Delay2 : public FunctionProcessorPlugin<gam::Delay2<double>>
    {
        Delay2(double value=0) : 
        FunctionProcessorPlugin<gam::Delay2<double>>(),
        gam::Delay2<double>(value)
        {

        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return (*this)(I);
        }
    };

    struct Delay : public FunctionProcessorPlugin<gam::Delay<double>>
    {
        Delay(double value=0) : FunctionProcessorPlugin<gam::Delay<double>>(),
        gam::Delay<double>(value)
        {

        }
        enum {
            PORT_DELAY,
            PORT_DELAYSAMPLES,
            PORT_DELAYSAMPLESR,
            PORT_DELAYUNIT,
            PORT_FREQ,
            PORT_IPLTYPE,
            PORT_MAXDELAY
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DELAY: this->delay(v); break;
                case PORT_DELAYSAMPLES: this->delaySamples(v); break;
                case PORT_DELAYSAMPLESR: this->delaySamplesR(v); break;
                case PORT_DELAYUNIT: this->delayUnit(v); break;
                case PORT_FREQ: this->freq(v); break;
                case PORT_IPLTYPE: this->ipolType((gam::ipl::Type)v); break;
                case PORT_MAXDELAY: this->maxDelay(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {            
            return A*(*this)(I);
        }
    };

    struct Multitap : public FunctionProcessorPlugin<gam::Multitap<double>>
    {
        Multitap(double value=0) : FunctionProcessorPlugin<gam::Multitap<double>>(),
        gam::Multitap<double>(value)
        {

        }
        enum {
            PORT_DELAY,
            PORT_FREQ,
            PORT_TAPS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TAPS: this->taps(v); break;
            }
        }
        void setPort2(int port, double x double y) {
            switch(port) {
                case PORT_DELAY: this->delay(x,y); break;
                case PORT_FREQ: this->freq(x,y); break;
            }
        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return A*(*this)(I);
        }
    };
}
