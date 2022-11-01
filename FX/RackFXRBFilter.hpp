#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class RBFilter:public Filter_
    {
    public:
        RBFilter (int Ftype, DspFloatType Ffreq, DspFloatType Fq,
                int Fstages);
        ~RBFilter ();
        void filterout (DspFloatType * smp);
        DspFloatType filterout_s (DspFloatType smp);

        void setfreq (DspFloatType frequency);
        void setfreq_and_q (DspFloatType frequency, DspFloatType q_);
        void setq (DspFloatType q_);
        void directmod(DspFloatType lfo);

        void settype (int type_);
        void setgain (DspFloatType dBgain);
        void setstages (int stages_);
        void setmix (int mix, DspFloatType lpmix, DspFloatType bpmix, DspFloatType hpmix);
        void setmode(int mode);
        void cleanup ();

    private:

        struct fstage {
            DspFloatType low, high, band, notch;
        } st[MAX_FILTER_STAGES + 1];

        struct parameters {
            DspFloatType f, q, q_sqrt;
        } par, ipar;

        void singlefilterout (DspFloatType * smp, fstage & x, parameters & par);
        DspFloatType singlefilterout_s (DspFloatType smp, fstage & x, parameters & par);
        void computefiltercoefs ();
        void computefiltercoefs_hiQ ();

        int type;			//The type of the filter (LPF1,HPF1,LPF2,HPF2...)
        int stages;			//how many times the filter is applied (0->1,1->2,etc.)
        int abovenq;			//this is 1 if the frequency is above the nyquist
        int oldabovenq;
        int needsinterpolation, firsttime, en_mix;
        int qmode;                   //set to true for compatibility to old presets.  0 means Q = 1/q

        DspFloatType freq;		//Frequency given in Hz
        DspFloatType q;			//Q factor (resonance or Q factor)
        DspFloatType gain;		//the gain of the filter (if are shelf/peak) filters
        DspFloatType hpg, lpg, bpg;
        DspFloatType oldq, oldsq, oldf;
        DspFloatType a_smooth_tc, b_smooth_tc;
        DspFloatType iper;			//inverse of PERIOD
    };
}
