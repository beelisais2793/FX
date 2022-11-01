#pragma once

namespace DSPFX
{

    struct FxDecimator : public MonoFXProcessor
    {
        Decimator * deci;

        /** Resampling Factor constants */
        enum factor
        {
            /** 2x resampling */
            X2 = 0,

            /** 4x resampling */
            X4,

            /** 8x resampling */
            X8,

            /** 16x resampling */
            /*X16,*/

            /** number of resampling factors */
            N_FACTORS
        };
        FxDecimator(factor fac) : MonoFXProcessor() {
            deci = DecimatorInit((ResampleFactor_t)fac);
            assert(deci != NULL);
        }
        ~FxDecimator() {
            if(deci) DecimatorFree(deci);
        }
        void flush() {
            DecimatorFlush(deci);
        }
        void ProcessBlock(size_t n, const DspFloatType * in, DspFloatType * out) {
            Error_t err = DecimatorProcess(deci,out,in,n);
        }
    };

}

