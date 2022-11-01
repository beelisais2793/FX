#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    //Waveshaping(called by Distorsion effect and waveshape from OscilGen)
    // void waveshapesmps (int n, DspFloatType * smps, int type,
    // 		    int drive);

    class NewDist
    {
    public:
        NewDist (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~NewDist ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        void applyfilters (DspFloatType * efxoutl, DspFloatType * efxoutr);

        int Ppreset;
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType inpll[4096];
        DspFloatType inplr[4096];

    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void setlrcross (int Plrcross);
        void setoctave (int Poctave);
        void setlpf (int Plpf);
        void sethpf (int Phpf);


        //Parametrii
        int Pvolume;	//Volumul or E/R
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing
        int Pdrive;		//the input amplification
        int Plevel;		//the ouput amplification
        int Ptype;		//Distorsion type
        int Pnegate;	//if the input is negated
        int Plpf;		//lowpass filter
        int Phpf;		//highpass filter
        int Prfreq;
        int Pprefiltering;	//if you want to do the filtering before the distorsion
        int Poctave;	//mix sub octave


        DspFloatType rfreq;
        DspFloatType panning, lrcross, octave_memoryl, togglel, octave_memoryr, toggler, octmix;
        DspFloatType *octoutl, *octoutr;


        //Parametrii reali
        AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr, *blockDCl, *blockDCr, *DCl, *DCr;
        class Waveshaper *wshapel, *wshaper;


        class Filter *filterl, *filterr;
        class FilterParams *filterpars;

        class FPreset *Fpre;

    };
}