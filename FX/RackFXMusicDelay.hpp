#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
     class MusicDelay
    {
    public:
        MusicDelay (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~MusicDelay ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;


    private:

        void setvolume (int Pvolume);
        void setpanning (int num, int Ppanning);
        void setdelay (int num, int Pdelay);
        void setgain (int num, int Pgain);
        void setlrdelay (int Plrdelay);
        void setlrcross (int Plrcross);
        void setfb (int num, int Pfb);
        void sethidamp (int Phidamp);
        void settempo (int Ptempo);
        void initdelays ();



        //Parametrii
        int Pvolume;			//Volumul or E/R
        int Ppanning1;		//Panning
        int Ppanning2;
        int Pgain1;
        int Pgain2;
        int Pdelay1;
        int Pdelay2;
        int Plrdelay;			// L/R delay difference
        int Plrcross;			// L/R Mixing
        int Pfb1;			//Feed-back-ul
        int Pfb2;
        int Phidamp;
        int Ptempo;

        //Parametrii reali
        int dl1, dr1, dl2, dr2, delay1, delay2, lrdelay;
        int kl1, kr1, kl2, kr2;
        int maxx_delay;
        DspFloatType panning1, panning2, lrcross, fb1, fb2, hidamp;
        DspFloatType gain1, gain2;
        DspFloatType *ldelay1, *rdelay1, *ldelay2, *rdelay2;
        DspFloatType oldl1, oldr1, oldl2, oldr2;	//pt. lpf

        class FPreset *Fpre;

    };
}