#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Echo
    {
    public:
        Echo (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Echo ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

        DspFloatType outvolume;

        void Tempo2Delay(int delay);

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
        int Pdirect;

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setdelay (int Pdelay);
        void setlrdelay (int Plrdelay);
        void setlrcross (int Plrcross);
        void setfb (int Pfb);
        void sethidamp (int Phidamp);
        void setreverse (int Preverse);
        void setdirect (int Pdirect);

        //Parametrii reali
        void initdelays ();

        int dl, dr;
        DspFloatType delay, lrdelay;
        int kl, kr, rvkl, rvkr, rvfl, rvfr, maxx_delay, fade;

        class delayline *ldelay, *rdelay;
        DspFloatType ltime, rtime;
        DspFloatType oldl, oldr;		//pt. lpf
        DspFloatType panning, lrcross, fb, hidamp, reverse, ireverse;
        DspFloatType  Srate_Attack_Coeff;
    };
}