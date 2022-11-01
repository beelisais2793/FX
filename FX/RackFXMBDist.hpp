#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class MBDist
    {
    public:
        MBDist (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~MBDist ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType *lowl;
        DspFloatType *lowr;
        DspFloatType *midl;
        DspFloatType *midr;
        DspFloatType *highl;
        DspFloatType *highr;


    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setlrcross (int Plrcross);
        void setCross1 (int value);
        void setCross2 (int value);

        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing
        int Pdrive;		//the input amplification
        int Plevel;		//the ouput amplification
        int PtypeL;
        int PtypeM;
        int PtypeH;
        int PvolL;
        int PvolM;
        int PvolH;
        int PdriveL;
        int PdriveM;
        int PdriveH;
        //MBDist type
        int Pnegate;	//if the input is negated
        int Cross1;		//lowpass filter
        int Cross2;		//highpass filter
        int Pstereo;	//0=mono,1=stereo


        //Parametrii reali
        DspFloatType panning, lrcross;
        DspFloatType volL,volM,volH;
        AnalogFilter *lpf1l, *lpf1r, *hpf1l, *hpf1r;
        AnalogFilter *lpf2l, *lpf2r, *hpf2l, *hpf2r;
        AnalogFilter *DCl, *DCr;

        class Waveshaper *mbwshape1l, *mbwshape2l, *mbwshape3l;
        class Waveshaper *mbwshape1r, *mbwshape2r, *mbwshape3r;

        class FPreset *Fpre;

    };
}