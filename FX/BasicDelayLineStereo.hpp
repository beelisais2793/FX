
#pragma once

#include "BasicDelayLine.hpp"

namespace Delays
{
    struct StereoBasicDelayLine : public StereoFXProcessor
    {
        BasicDelayLine delayL,delayR;

        StereoBasicDelayLine(const int sr, const DspFloatType d_ms, const DspFloatType fb, const DspFloatType mix) : StereoFXProcessor(),
        delayL(sr,d_ms,fb,mix),
        delayR(sr,d_ms,fb,mix),
        {

        }
        DspFloatType Tick(DspFloatType IL, DspFloatType IR, DspFloatType &L, DspFloatType &R, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            L = delayL.Tick(IL,A,X,Y);
            R = delayR.Tick(IR,A,X,Y);
            return 0.5*(L+R);
        }
        void ProcessBlock(size_t num, DspFloatType ** in, DspFloatType ** out)
        {
            delayL.ProcessBlock(num,in[0],out[0]);
            delayR.ProcessBlock(num,in[0],out[0]);

        }
    };
}