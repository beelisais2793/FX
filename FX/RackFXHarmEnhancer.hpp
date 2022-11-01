#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class HarmEnhancer
    {
    public:
        HarmEnhancer(DspFloatType *harmonics, DspFloatType hfreq, DspFloatType lfreq, DspFloatType gain);
        ~HarmEnhancer();
        void cleanup();
        void chebpc(DspFloatType c[], DspFloatType d[]);
        void calcula_mag(DspFloatType *Rmag);
        void harm_out(DspFloatType *smpsl, DspFloatType *smpsr);
        void set_vol(int mode, DspFloatType gain);
        void set_freqh(int mode, DspFloatType freq);
        void set_freql(int mode, DspFloatType freq);

        DspFloatType realvol;
        DspFloatType hpffreq;
        DspFloatType lpffreq;

    private:

        DspFloatType *inputl;
        DspFloatType *inputr;
        DspFloatType vol;
        DspFloatType itm1l;
        DspFloatType itm1r;
        DspFloatType otm1l;
        DspFloatType otm1r;

        DspFloatType p[HARMONICS];

        AnalogFilter *hpfl, *hpfr;
        AnalogFilter *lpfl, *lpfr;

        class Compressor *limiter;

    };
}