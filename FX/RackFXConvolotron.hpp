#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{

    class Convolotron
    {
    public:
        Convolotron (DspFloatType * efxoutl_, DspFloatType * efxoutr_,int DS, int uq, int dq);
        ~Convolotron ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        int setfile (int value);
        void adjust(int DS);
        void loaddefault();

        int Ppreset;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;

        char Filename[128];


    private:
        //Parametrii
        int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
        int Ppanning;	//Panning
        int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
        int Phidamp;
        int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
        int Psafe;
        int Plength;		//5...500 ms// Set maximum length of IR.
        int Puser;		//-64...64//Feedback.
        int Filenum;
        int Pfb;		//-64 ... 64// amount of feedback
        void setvolume (int Pvolume);
        void setpanning (int Ppanning);
        void sethidamp (int Phidamp);
        void process_rbuf();
        void UpdateLength ();

        int offset;
        int maxx_size,maxx_read,real_len,length;
        int DS_state;
        int nPERIOD;
        int nSAMPLE_RATE;

        double u_up;
        double u_down;
        DspFloatType nfSAMPLE_RATE;


        DspFloatType lpanning, rpanning, hidamp, alpha_hidamp, convlength, oldl;
        DspFloatType *rbuf, *buf, *lxn;
        DspFloatType *templ, *tempr;

        DspFloatType level,fb, feedback;
        DspFloatType levpanl,levpanr;

        SNDFILE *infile;
        SF_INFO sfinfo;

        //Parametrii reali

        class Resample *M_Resample;
        class Resample *U_Resample;
        class Resample *D_Resample;

        class FPreset *Fpre;

        fft_filter impulse;


    };
}