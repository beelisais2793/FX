#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class beattracker
    {
    public:
        beattracker ();
        ~beattracker ();
        void cleanup ();
        void detect (DspFloatType * smpsl, DspFloatType * smpsr);
        DspFloatType get_tempo();  //returns tempo in DspFloatType beats per minute
        int *index;

    private:

        long timeseries[20];
        int tsidx;
        long tscntr;
    //Variables for TrigStepper detecting trigger state.
        DspFloatType peakpulse, peak, envrms, peakdecay, trigthresh;
        int trigtimeout, trigtime, onset, atk;
        DspFloatType targatk, lmod, rmod;

        class RBFilter *rmsfilter, *peaklpfilter, *peakhpfilter, *peaklpfilter2;

        DspFloatType oldbpm, oldmost;
        DspFloatType avbpm[17], statsbin[17];
        int maxptr;
        int bpm_change_cntr;

        void calc_tempo();   //called by detect() on every beat detected
    };
}