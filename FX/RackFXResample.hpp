#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Resample
    {
    public:
        Resample(int type);
        /*
        Types:
                SRC_SINC_BEST_QUALITY       = 0,
                SRC_SINC_MEDIUM_QUALITY     = 1,
                SRC_SINC_FASTEST            = 2,
                SRC_ZERO_ORDER_HOLD         = 3,
                SRC_LINEAR                  = 4
        */

        ~Resample();
        void cleanup();
        void out(DspFloatType *inl, DspFloatType *inr, DspFloatType *outl, DspFloatType *outr, int frames, double ratio);
        void mono_out(DspFloatType *inl, DspFloatType *outl, int frames, double ratio, int o_frames);

        // ratio Equal to output_sample_rate / input_sample_rate.


        SRC_DATA srcinfor;
        SRC_DATA srcinfol;


    private:

        int errorl,errorr;


        SRC_STATE *statel;
        SRC_STATE *stater;
    };
}