#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Sustainer
    {
    public:
        Sustainer (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Sustainer ();
        void cleanup ();

        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void changepar (int npar, int value);
        int getpar (int npar);
        void setpreset (int npreset);

        int Ppreset;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

    private:
        //Parametrii
        int Pvolume;	//Output Level
        int Psustain;	//Compression amount

        int timer, hold;
        DspFloatType level, fsustain, input,tmpgain;
        DspFloatType prls, compeak, compg, compenv, oldcompenv, calpha, cbeta, cthresh, cratio, cpthresh;

        class FPreset *Fpre;
    };

}