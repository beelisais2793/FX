#pragma once

#include "StkHeaders.hpp"

namespace Stk::Envelopes
{
    struct ADSR : public GeneratorProcessorPlugin<Stk::ADSR>
    {
        ADSR() : GeneratorProcessorPlugin<Stk::ADSR>
        {

        }
        ADSR(float a, float d, float s, float r)
        : GeneratorProcessor(),
          Stk::ADSR()
        {
            this->setAllTimes(a,d,s,r);
        }

        enum {
            PORT_ATTACKRATE,
            PORT_DECAYRATE,
            PORT_SUSTAIN,
            PORT_RELEASERATE,
            PORT_ATTACKTIME,
            PORT_DECAYTIME,
            PORT_RELEASETIME,
            PORT_TARGET,
            PORT_VALUE,
        };
        void setPort(int port, double value) {
            switch(port) {
                case PORT_SUSTAIN: this->setSustainLevel(value); break;
                case PORT_ATTACKRATE: this->setAttackRate(value); break;
                case PORT_DECAYRATE: this->setDecayRate(value); break;
                case PORT_RELEASERATE: this->setReleaseRate(value); break;
                case PORT_ATTACKTIME: this->setAttackTime(value); break;
                case PORT_DECAYTIME: this->setDecayTime(value); break;
                case PORT_RELEASETIME: this->setReleaseTime(value); break;                                
                case PORT_TARGET: this->setTarget(value); break;
                case PORT_VALUE: this->setValue(value); break;
            }
        }
        void noteOn() {
            this->keyOn();
        }
        void noteOff() {
            this->keyOff();
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return this->tick();
        }
    };

