#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    #define  ECHOTRON_F_SIZE   128       //Allow up to 150 points in the file
    #define  ECHOTRON_MAXFILTERS  32      //filters available


    class Echotron
    {
    public:
        Echotron (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Echotron ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        int setfile (int value);
        int Ppreset;

        int Pchange;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;

        char Filename[128];


    private:

        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void sethidamp (int Phidamp);
        void setlpf (int Plpf);
        void setfb(int value);
        void init_params();
        void modulate_delay();
        void modulate_filters();
        void loaddefault();


        //User input parameters
        EffectLFO lfo;
        EffectLFO dlfo;
        int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing  //
        int Phidamp;
        int Puser;		//0,1//
        int Ptempo;		//Tempo, BPM//For stretching reverb responses
        int Filenum;
        int Pfb;		//-64 ... 64// amount of feedback
        int Pdepth;
        int Pwidth;
        int Pfilters;         //0 or 1// use or don't use filters in delay line
        int Pmodfilts;	//0 or 1// apply LFO to filter freqs
        int Pmoddly;		//0 or 1// apply LFO to delay time
        int Pstdiff;
        int Plength;


        int offset;
        int maxx_size;


        //arrays of parameters from text file:
        DspFloatType fPan[ECHOTRON_F_SIZE];  //1+Pan from text file
        DspFloatType fTime[ECHOTRON_F_SIZE];
        DspFloatType fLevel[ECHOTRON_F_SIZE];
        DspFloatType fLP[ECHOTRON_F_SIZE];
        DspFloatType fBP[ECHOTRON_F_SIZE];
        DspFloatType fHP[ECHOTRON_F_SIZE];
        DspFloatType fFreq[ECHOTRON_F_SIZE];
        DspFloatType fQ[ECHOTRON_F_SIZE];
        int iStages[ECHOTRON_F_SIZE];
        DspFloatType subdiv_dmod;
        DspFloatType subdiv_fmod;
        int f_qmode;

        DspFloatType rtime[ECHOTRON_F_SIZE];
        DspFloatType ltime[ECHOTRON_F_SIZE];
        DspFloatType ldata[ECHOTRON_F_SIZE];
        DspFloatType rdata[ECHOTRON_F_SIZE];

    //end text configurable parameters

        int initparams;

        DspFloatType ldmod, rdmod, oldldmod, oldrdmod, interpl, interpr;
        DspFloatType dlyrange;

        DspFloatType width, depth;
        DspFloatType lpanning, rpanning, hidamp, alpha_hidamp, convlength;

        delayline *lxn, *rxn;

        DspFloatType level,fb, rfeedback, lfeedback,levpanl,levpanr, lrcross, ilrcross;
        DspFloatType tempo_coeff;

        class AnalogFilter *lpfl, *lpfr;	//filters

        struct {
            DspFloatType sfreq, sq,sLP,sBP,sHP, sStg;
            class RBFilter *l, *r;

        } filterbank[ECHOTRON_MAXFILTERS];

        class FPreset *Fpre;

    };

}