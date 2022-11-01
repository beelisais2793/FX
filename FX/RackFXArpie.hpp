#pragma once

#include "RackFX.hpp"

namespace FX:RackFX
{
    class Arpie
    {
    public:
        Arpie (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Arpie ();
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
        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Ppanning;	//Panning
        int Pdelay;
        int Plrdelay;	// L/R delay difference
        int Plrcross;	// L/R Mixing
        int Pfb;		//Feed-back-ul
        int Phidamp;
        int Preverse;
        int Ppattern;
        int Pharms;
        int Psubdiv;


        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setdelay (int Pdelay);
        void setlrdelay (int Plrdelay);
        void setlrcross (int Plrcross);
        void setfb (int Pfb);
        void sethidamp (int Phidamp);
        void setreverse (int Preverse);
        void setpattern (int Ppattern);


        //Parametrii reali
        void initdelays ();

        int dl, dr, delay, lrdelay;
        int kl, kr, rvkl, rvkr, rvfl, rvfr, maxx_delay, fade, harmonic, envcnt, invattack;
        int subdiv;
        int *pattern;

        DspFloatType panning, lrcross, fb, hidamp, reverse;
        DspFloatType *ldelay, *rdelay;
        DspFloatType oldl, oldr;		//pt. lpf
        DspFloatType  Srate_Attack_Coeff, envattack, envswell;

        class FPreset *Fpre;
    };
}