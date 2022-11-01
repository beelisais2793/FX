#pragma once

#include "Stk.hpp"
#include "SoundObject.hpp"


namespace STK1000
{
    struct Asymp : public GeneratorProcessor
    {
        stk::Asymp * asymp;

        Asymp() : GeneratorProcessor() {
            asymp = new Asymp;
            assert(asymp != nullptr);
        }
        ~Asymp() {
            if(asymp) delete asymp;
        }

        vod setTau(double tau) {
            asymp->setTau(tau);
        }
        void setTime(double t) {
            asymp->setTime(t);
        }
        void setT60(double t60) {
            asymp->setT60(t60);
        }
        void setTarget(double t) {
            asymp->setTarget(t);
        }
        void setValue(double v) {
            asymp->setValue(v);
        }
        double lastOut() { 
            return asymp->lastOut(); 
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {        
            double r = I*asymp->tick();
            return A*r;
        }
    };
}