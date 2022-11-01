#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_src = fv3::src_;        
    #else
    using fv3_src = fv3::src_f;
    #endif
    struct UpSampleRateConverter : public StereoFXProcessorPlugin<fv3_src>
    {
        enum
        {
        FV3_SRC_SINC_BEST_QUALITY        =   0,
        FV3_SRC_SINC_MEDIUM_QUALITY      =   1,
        FV3_SRC_SINC_FASTEST             =   2,
        FV3_SRC_ZERO_ORDER_HOLD          =   3,
        FV3_SRC_LINEAR                   =   4,
        FV3_SRC_SINC_SLOW_BEST_QUALITY   =  10,
        FV3_SRC_SINC_SLOW_MEDIUM_QUALITY =  11,
        FV3_SRC_LPF_IIR_1                = 100,
        FV3_SRC_LPF_IIR_2                = 101,
        };

        UpSampleRateConverter(long src, DspFloatType lpfbw, int type = FV3_SRC_SINC_MEDIUM_QUALITY) 
        : StereoFXProcessorPlugin<fv3_src>()
        {
            this->setSRCFactor(src,type);
            this->setLPFBW(lpfbw);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
            this->usrc(in[0],in[1],out[0],out[1],n);
        }
    };

    struct DownSampleRateConverter : public StereoFXProcessorPlugin<fv3_src>
    {
        enum
        {
        FV3_SRC_SINC_BEST_QUALITY        =   0,
        FV3_SRC_SINC_MEDIUM_QUALITY      =   1,
        FV3_SRC_SINC_FASTEST             =   2,
        FV3_SRC_ZERO_ORDER_HOLD          =   3,
        FV3_SRC_LINEAR                   =   4,
        FV3_SRC_SINC_SLOW_BEST_QUALITY   =  10,
        FV3_SRC_SINC_SLOW_MEDIUM_QUALITY =  11,
        FV3_SRC_LPF_IIR_1                = 100,
        FV3_SRC_LPF_IIR_2                = 101,
        };

        DownSampleRateConverter(long src, DspFloatType lpfbw, int type = FV3_SRC_SINC_MEDIUM_QUALITY) 
        : StereoFXProcessorPlugin<fv3_src>()
        {
            this->setSRCFactor(src,type);
            this->setLPFBW(lpfbw);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
            this->dsrc(in[0],in[1],out[0],out[1],n);
        }
    };
}    