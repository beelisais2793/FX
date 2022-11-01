#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    #define NUM_INF_BANDS 8

    class Infinity
    {
    public:
        Infinity (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Infinity ();
        void out (DspFloatType * smpsl, DspFloatType * smpsr);

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
        void adjustfreqs();
        void oscillator();
        void reinitfilter ();
        void setq ();

        int Pvolume;		//For wet/dry mix // "Wet/Dry"
        int Pb[NUM_INF_BANDS];          //Pbx -64 to 64// "Band" each label "1" through "8" adjusts mix of each filter band
        int Pq;              //-64 to 64// "Res." filter resonance
        int Pstartfreq;         //0 to 127// "Start" where the filter starts
        int Pendfreq;	       //0 to 127// "End" when the filter ends
        int Prate;           //BPM// "Tempo" how fast it sweeps
        int Pstdf;           //filter bands offset L&R
        int Psubdiv;
        int Pautopan;
        int Preverse;
        int Pstages;        //filter stages

        struct filterstate {
            DspFloatType sinp;
            DspFloatType cosp;      //sinusoid oscillator state variables
            DspFloatType ramp;      //frequency oscillator
            DspFloatType level;
            DspFloatType lfo;
            DspFloatType vol;
        } rbandstate[NUM_INF_BANDS], lbandstate[NUM_INF_BANDS];

        struct phasevars {
            DspFloatType yn1[MAX_PHASER_STAGES];
            DspFloatType xn1[MAX_PHASER_STAGES];
            DspFloatType gain;
        } rphaser[NUM_INF_BANDS], lphaser[NUM_INF_BANDS];
        DspFloatType phaserfb;
        DspFloatType phaser(phasevars *pstruct, DspFloatType fxn, int j);
        /*
        fconst = freq* 2*pi/fs;
        rampconst = 2*freq/fs;
        */
        DspFloatType cfconst, fconst;  //sine wave oscillator frequency constant // 2*pi*freq/fs
        DspFloatType rampconst, irampconst, crampconst, cirampconst;  //ramp oscillator constant// 2*freq/SR
        DspFloatType alpha, beta;
        DspFloatType fstart, fend;  //range of filter sweep
        DspFloatType linconst, logconst;
        DspFloatType minlevel, maxlevel;
        DspFloatType frmin, frmax;
        DspFloatType qq;
        DspFloatType stdiff;
        DspFloatType volmaster;
        DspFloatType autopan, msin, dsin, mcos, mconst;
        DspFloatType ratescale;
        int tflag;

        class RBFilter *filterl[NUM_INF_BANDS], *filterr[NUM_INF_BANDS];

        class FPreset *Fpre;


    };
}