#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Reverb
    {
    public:
        Reverb (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Reverb ();
        void out (DspFloatType * smps_l, DspFloatType * smps_r);
        void cleanup ();

        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);

        int Ppreset;
        DspFloatType outvolume;		//this is the volume of effect and is public because need it in system effect. The out volume of su

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;



    private:

        void setvolume (int Pvolume);
        void setpan (int Ppan);
        void settime (int Ptime);
        void setlohidamp (int Plohidamp);
        void setidelay (int Pidelay);
        void setidelayfb (int Pidelayfb);
        void sethpf (int Phpf);
        void setlpf (int Plpf);
        void settype (int Ptype);
        void setroomsize (int Proomsize);
        void processmono (int ch, DspFloatType * output);



        //Parametrii
        //Amount of the reverb,
        int Pvolume;

        //LefT/Right Panning
        int Ppan;

        //duration of reverb
        int Ptime;

        //Initial delay
        int Pidelay;

        //Initial delay feedback
        int Pidelayfb;

        //delay between ER/Reverbs
        int Prdelay;

        //EarlyReflections/Reverb Balance
        int Perbalance;

        //HighPassFilter
        int Plpf;

        //LowPassFilter
        int Phpf;

        //Low/HighFrequency Damping
        int Plohidamp;	// 0..63 lpf,64=off,65..127=hpf(TODO)

        //Reverb type
        int Ptype;

        //Room Size
        int Proomsize;

        //Parametrii 2
        int lohidamptype;		//0=disable,1=highdamp(lowpass),2=lowdamp(highpass)
        int idelaylen, rdelaylen;
        int idelayk;
        int comblen[REV_COMBS * 2];
        int aplen[REV_APS * 2];


        int combk[REV_COMBS * 2];
        int apk[REV_APS * 2];

        DspFloatType lohifb, idelayfb, roomsize, rs;	//rs is used to "normalise" the volume according to the roomsize
        DspFloatType rs_coeff;
        //parameter control
        DspFloatType pan, erbalance;


        //Valorile interne

        DspFloatType *comb[REV_COMBS * 2];

        DspFloatType combfb[REV_COMBS * 2];	//feedback-ul fiecarui filtru "comb"
        DspFloatType lpcomb[REV_COMBS * 2];	//pentru Filtrul LowPass

        DspFloatType *ap[REV_APS * 2];
        DspFloatType *inputbuf;
        DspFloatType *idelay;

        class AnalogFilter *lpf, *hpf;	//filters
        class FPreset *Fpre;

    };
}