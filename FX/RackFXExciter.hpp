#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class Exciter
    {
    public:
        Exciter (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Exciter ();
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
        void setlpf (int value);
        void sethpf (int value);
        void sethar(int num, int value);


        int Prm[10];
        int Pvolume;
        int lpffreq;
        int hpffreq;

        DspFloatType rm[10];



        class HarmEnhancer *harm;

        class FPreset *Fpre;

    };
}