#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_delayline = fv3::delayline_;        
    #else
    using fv3_delayline = fv3::delayline_f;
    #endif
    struct MultiTapDelayLine : public FunctionProcessorPlugin<fv3_delayline>
    {
        DspFloatType feedback;
        DspFloatType depth = 0.01;
        DspFloatType mix = 0.5;
        std::vector<int> taps;
        MultiTapDelayLine(size_t size, DspFloatType fbk) : FunctionProcessorPlugin<fv3_delayline>()
        {
            this->setsize(size);            
            feedback = fbk;
        }
        void addTap(int t) {
            taps.push_back(-fabs(t));
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {
            DspFloatType tap = 0;
            for(size_t i = 0; i < taps.size(); i++)
            {
                DspFloatType x = this->at(taps[i]);
                tap += x;
            }
            tap /= (DspFloatType)taps.size();
            int idx = Y * this->getsize() * depth;
            DspFloatType ft = feedback;
            feedback *= X;
            DspFloatType r1 = this->at(idx);
            r1 += tap;
            DspFloatType out = this->process(I + feedback*r1);            
            feedback = ft;
            return mix*I + (1.0-mix)*out;
        }
    };
}