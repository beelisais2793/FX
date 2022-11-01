#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Valve
    {
    public:
        Valve (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Valve ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        DspFloatType Wshape(DspFloatType x);
        void cleanup ();
        void applyfilters (DspFloatType * efxoutl, DspFloatType * efxoutr);

        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

    private:
        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setlrcross (int Plrcross);
        void setlpf (int Plpf);
        void sethpf (int Phpf);
        void setpresence(int value);
        void init_coefs();


        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing
        int Pdrive;		//the input amplification
        int Plevel;		//the ouput amplification
        int Pnegate;	//if the input is negated
        int Plpf;		//lowpass filter
        int Phpf;		//highpass filter
        int Pstereo;	//0=mono,1=stereo
        int Pprefiltering;	//if you want to do the filtering before the distorsion
        int Q_q;
        int Ped;
        int Presence;

        //Parametrii reali
        DspFloatType panning, lrcross, q, dist, otml,otmr, itml, itmr, factor, atk;
        DspFloatType rm[10];

        DspFloatType coef;
        DspFloatType qcoef;
        DspFloatType fdist;
        DspFloatType inputvol;

        AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;
        class HarmEnhancer *harm;
        class FPreset *Fpre;

    };
}