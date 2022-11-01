#pragma once

#include "HammerFX.hpp"

namespace FX::HammerFX
{
    struct Distortion
    {
        distort_params cp;

        Distortion() {
            distort_create(&cp);
        }
        ~Distortion() {
            distort_destroy(&cp);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            distort_filter(&cp,n,in,out);
        }
    };

    struct Distortion2
    {
        distort_params2 cp;

        Distortion2() {
            distort2_create(&cp);
        }
        ~Distortion2() {
            distort2_destroy(&cp);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            distort2_filter(&cp,n,in,out);
        }
    };
}
