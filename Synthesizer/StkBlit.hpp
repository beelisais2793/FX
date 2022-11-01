#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Blit : public OscillatorProcessorPlugin<stk::Blit>
    {
        Blit() : OscillatorProcessorPlugin<stk::Blit>()
        {

        }
        enum {
            PORT_PHASE,
            PORT_RESET,
            PORT_FREQ,
            PORT_HARMONICS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PHASE: this->setPhase(v); break;
                case PORT_RESET: this->reset(); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_HARMONICS: this->setHarmonics(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
    // todo: ports
    struct BlitSaw : public OscillatorProcessorPlugin<stk::BlitSaw>
    {
        BlitSaw() : OscillatorProcessorPlugin<stk::BlitSaw>()
        {

        }
        enum {
            PORT_PHASE,
            PORT_RESET,
            PORT_FREQ,
            PORT_HARMONICS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PHASE: this->setPhase(v); break;
                case PORT_RESET: this->reset(); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_HARMONICS: this->setHarmonics(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }        
    };
    // todo: ports
    struct BlitSquare : public OscillatorProcessorPlugin<stk::BlitSquare>
    {
        BlitSquare() : OscillatorProcessorPlugin<stk::BlitSquare>()
        {

        }
        enum {
            PORT_PHASE,
            PORT_RESET,
            PORT_FREQ,
            PORT_HARMONICS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_PHASE: this->setPhase(v); break;
                case PORT_RESET: this->reset(); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_HARMONICS: this->setHarmonics(v); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }      
    };
}