#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{   
    #ifdef DSPFLOATDOUBLE
    using fv3_allpassm = fv3::allpassm_;
    #else
    using fv3_allpassm = fv3::allpassm_f;
    #endif
    struct FV3AllPassFilterModulatedDecay : public FilterProcessorPlugin<fv3_allpassm>
    {
        FV3AllPassFilterModulatedDecay(size_t size, size_t msize, DspFloatType fbk, DspFloatType dcy) 
        : FilterProcessorPlugin<fv3_allpassm>()
        {
            this->setsize(size);            
            this->setfeedback(fbk);
            this->setdecay(dcy);        
        }
        enum {
            PORT_SIZE,
            PORT_FEEDBACK,
            PORT_MUTE,
            PORT_DECAY,
            PORT_90DEG,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_SIZE: this->setsize(v); break;
                case PORT_FEEDBACK: this->setfeedback(v); break;
                case PORT_MUTE: this->mute(); break;
                case PORT_DECAY: this->setdecay(v); break;
                default: printf("No port %d\n", port);
            }
        }
        void setPort2(int port, DspFloatType a, DspFloatType b) {
            if(port == PORT_90DEG) this->set_90degfq(a,b); 
            else if(port == PORT_SIZE) this->setsize(a,b);
            else printf("No port %d\n", port);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {        
            DspFloatType r = this->process_dc(I,X,Y);
            return A*r;
        }        
    };
}