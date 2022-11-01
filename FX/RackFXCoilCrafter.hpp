#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class CoilCrafter
    {
    public:
        CoilCrafter (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~CoilCrafter ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();


        int Ppreset;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;

    private:

        void setvolume (int value);
        void sethpf (int value);
        void setfreq1();
        void setfreq2();
        void setq1();
        void setq2();

        int Pvolume;
        int Ppo;
        int Ppd;
        int Ptone;
        int Pq1;
        int Pfreq1;
        int Pq2;
        int Pfreq2;
        int Pmode;

        DspFloatType tfreqs[10];
        DspFloatType tqs[10];
        DspFloatType rm[10];
        DspFloatType freq1,q1,freq2,q2;
        DspFloatType att;

        class HarmEnhancer *harm;
        class AnalogFilter *RB1l, *RB1r;
        class AnalogFilter *RB2l,*RB2r;

        class FPreset *Fpre;

    };
}