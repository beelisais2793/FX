#pragma once

namespace DSPFX
{

    struct FxMetering
    {

        enum
        {
            FULL_SCALE,
            K_12,
            K_14,
            K_20
        };

        static DspFloatType phase_correlation(DspFloatType * left, DspFloatType * right, size_t n) {
            return ::phase_correlationD(left,right,n);
        }
        static DspFloatType balance(DspFloatType * left, DspFloatType * right, size_t n) {
            return ::balanceD(left,right,n);
        }
        static DspFloatType vu_peak(DspFloatType * signal, size_t n, int scale);
            return ::vu_peakD(left,right,n);
        }

    };
}
