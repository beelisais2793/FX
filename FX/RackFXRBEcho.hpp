#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class RBEcho
    {
    public:
        RBEcho (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~RBEcho ();
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
        void setpanning (int Ppanning);
        void setdelay (int Pdelay);
        void setlrdelay (int Plrdelay);
        void setlrcross (int Plrcross);
        void setfb (int Pfb);
        void sethidamp (int Phidamp);
        void setreverse (int Preverse);
        void initdelays ();

        //Parametrii
        int Pvolume;	// E/R
        int Ppanning;	//Panning
        int Pdelay;
        int Plrdelay;	// L/R delay difference
        int Plrcross;	// L/R Mixing
        int Pfb;		//Feed-back
        int Phidamp;
        int Preverse;
        int Psubdiv;
        int Pes;

        int maxx_delay;

        DspFloatType delay, lrdelay, ltime, rtime;
        DspFloatType fdelay, subdiv, pes, pingpong, ipingpong;
        DspFloatType rvl, rvr;
        DspFloatType rpanning, lpanning, lrcross, fb, hidamp, reverse, ireverse, lfeedback, rfeedback;
        DspFloatType oldl, oldr;		//pt. lpf
        DspFloatType  Srate_Attack_Coeff;

        class delayline *ldelay, *rdelay;
        class FPreset *Fpre;
    };
}