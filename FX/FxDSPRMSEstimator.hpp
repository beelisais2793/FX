#pragma once

namespace DSPFX
{

    struct FxRMSEstimator
    {
        RMSEstimator *rms;

        RMSEstimator(DspFloatType avg, DspFloatType sr=sampleRate)
        {
            rms = RMSEstimatorInitD(avg,sr);
            assert(rms != NULL);
        }
        ~RMSEstimator()
        {
            if(rms) RMSEstimatorFreeD(rms);
        }
        void flush() { RMSEstimatorFlushD(rms); }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            RMSEstimatorProcessD(rms,out,in,n);
        }
        // this is never modulated
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return RMSEstimatorTickD(I);
        }
    };
}
