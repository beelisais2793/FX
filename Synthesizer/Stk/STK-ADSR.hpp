#pragma once

#include "Stk.hpp"
#include "SoundObject.hpp"


namespace STK1000
{
    struct ADSR  : public GeneratorProcessor
    {
        ADSR * adsr;
        double sustain;
        ADSR() : GeneratorProcessor() {
            adsr = new ADSR;            
            assert(adsr != nullptr);
        }
        ADSR(double a, double d, double s, double r) : GeneratorProcessor() {
            adsr = new ADSR;            
            adsr->setAllTimes(a,d,s,r);
            sustain = s;
            assert(adsr != nullptr);
        }
        ~ADSR() {
            if(adsr) delete adsr;
        }

        void setAllTimes(double a, double d, double s, double r)
        {
            adsr->setAllTimes(a,d,s,r);
        }
        void setAttackTime(double t) {
            adsr->setAttackTime(t);
        }
        void setDecayTime(double t) {
            adsr->setDecayTime(t);
        }
        void setReleaseTime(double t) {
            adsr->setReleaseTime(t);
        }
        void setSustainLevel(double amt) {
            adsr->setSutainLevel(amt);
        }
        void setTarget(double t) {
            adsr->setTarget(t);
        }
        void setValue(double v) {
            adsr->setValue(v);
        }
        void noteOn() {
            adsr->keyOn();
        }
        void noteOff() {
            adsr->keyOff();
        }
        double lastOut() { 
            return asymp->lastOut(); 
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            double s = sustain;        
            sustain *= X*Y;
            double r = I*adsr->tick();
            sustain = s;
            return A*r;
        }                
    };
}


