#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{

    #ifdef DSPFLOATDOUBLE
    using fv3_allpass = fv3::allpass_;
    #else
    using fv3_allpass = fv3::allpass_f;
    #endif
    struct FV3AllPassFilter : public FilterProcessorPlugin<fv3_allpass>
    {
        FV3AllPassFilter(size_t size, DspFloatType fbk, DspFloatType dcy) : FilterProcessorPlugin<fv3_allpass>()
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
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType f = this->getfeedback();
            DspFloatType d = this->getdecay();
            this->setfeedback(f*X);
            this->setdecay(d*Y);
            DspFloatType r = this->process(I);
            this->setfeedback(f);
            this->setdecay(d);
            return A*r;
        }        
    };
}