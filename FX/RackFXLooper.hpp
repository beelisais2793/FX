#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Looper
    {
    public:
        Looper (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType size);
        ~Looper ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void loadpreset (int npar, int value);  // to set one from a preset
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanuppt1 ();
        void cleanuppt2 ();
        void cleanup ();
        void settempo(int value);
        void setmvol(int value);
        int looper_bar;
        int looper_qua;
        int Ppreset;
        int progstate[6];

        void getstate ();

        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

        int Pplay;	//set to 1
        int Pstop;	//set to 1


    private:

        void initdelays ();
        void setfade ();
        void setbar(int value);
        void timeposition(int value);
        int set_len(int value);
        int cal_len(int value);



        //Parameters
        int Pvolume;	//Mix
        int Precord;	//set to 1
        int Pclear;   //set to 1
        int Preverse;	//0 or 1
        int Pfade1;    //0...127//  crossfade track1 & 2
        int Pfade2;
        int PT1;
        int PT2;
        int Pautoplay;
        int Prec1;
        int Prec2;
        int Plink;
        int Ptempo;
        int Pbar;
        int Pmetro;
        int Pms;

        int kl, kl2, rvkl, rvkl2, maxx_delay, fade, dl, dl2, first_time1, first_time2, rplaystate;
        int barlen, looper_ts;

        DspFloatType *ldelay, *rdelay;
        DspFloatType *t2ldelay, *t2rdelay;

        DspFloatType oldl, oldr;		//pt. lpf

        DspFloatType  Srate_Attack_Coeff, track1gain, track2gain, fade1, fade2, pregain1, pregain2;
        DspFloatType mvol;

        class FPreset *Fpre;
        class metronome ticker;

    };
}