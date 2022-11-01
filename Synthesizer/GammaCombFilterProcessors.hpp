#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Delay
{
    // todo: other interpolations
    struct Comb : public FunctionProcessorPlugin<gam::Comb<double>
    {
        Comb() : FunctionProcessorPlugin<gam::Comb<double>>()
        {

        }
        Comb(float delay, float ffd=0.0, float fbk=0.0) 
        : FunctionProcessorPlugin<gam::Comb<double>>(),
          gam::Comb<double>(delay,ffd,fbk)
        {

        }
        Comb(float maxDelay, float delay, float ffd=0.0, float fbk=0.0) 
        : FunctionProcessorPlugin<gam::Comb<double>>(),
          gam::Comb<double>(maxDelay,delay,ffd,fbk)
        {

        }
        enum {
            PORT_ALLPASS,
            PORT_FBK,
            PORT_FFD,  
            PORT_DECAY,          
            PORT_NEXTFBK,
            PORT_NORM,
            PORT_NORMFBK,
            PORT_NORMFFD,    
            PORT_FBW,        
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_ALLPASS: this->allPass(v); break;
                case PORT_FBK: this->fbk(v); break;
                case PORT_FFD: this->ffd(v); break;
                case PORT_DECAY: this->decay(v); break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_FBK: return this->fbk(); 
                case PORT_FBW: return this->fbw();     
                case PORT_NORM: return this->norm(); 
                case PORT_NORMFBK: return this->normfbk(); 
                case PORT_NORMFFD: return this->normffd(); 
            }
            return 0.0;
        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            return (*this)(I);
        }
    };
}