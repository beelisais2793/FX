#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Dflange
    {
    public:
        Dflange (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Dflange ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();

        int Ppreset;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;


    private:
        //Parameters
        int Pwetdry;		// 0 //Wet/Dry mix.  Range -64 to 64
        int Ppanning;		// 1 //Panning.  Range -64 to 64
        int Plrcross;		// 2 //L/R Mixing.  Range 0 to 127
        int Pdepth;		// 3 //Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 4000
        int Pwidth;		// 4 //LFO amplitude.  Range 0 to 16000 (Hz)
        int Poffset;		// 5 //Offset of notch 1 to notch 2.  Range 0 to 100 (percent)
        int Pfb;		// 6 //Feedback parameter.  Range -64 to 64
        int Phidamp;		// 7 //Lowpass filter delay line.  Range 20 to 20000 (Hz)
        int Psubtract;	// 8 //Subtract wet/dry instead of add.  Nonzero is true
        int Pzero;		// 9 //Enable through-zero flanging,   Nonzero is true
        // 10 //LFO Speed
        // 11 //LFO stereo diff
        // 12 //LFO type
        // 13 //LFO Randomness
        int Pintense;		// 14 //Intense Mode

        DspFloatType wet, dry;		//Wet/Dry mix.
        DspFloatType lpan, rpan;		//Panning.
        DspFloatType flrcross, frlcross;	// L/R Mixing.
        DspFloatType fdepth;		//Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 15000
        DspFloatType fwidth;		//LFO amplitude.
        DspFloatType foffset;		// Offset of notch 1 to notch 2.  Range 0 to 1.0
        DspFloatType ffb;			//Feedback parameter.  Range -0.99 to 0.99
        DspFloatType fhidamp;		//Lowpass filter delay line.  Range 20 to 20000 (Hz)
        DspFloatType fsubtract;		//Subtract wet/dry instead of add.  Nonzero is true
        DspFloatType fzero;		//Enable through-zero flanging
        DspFloatType logmax;
        EffectLFO lfo;		//lfo Flanger

        //Internally used variables
        int maxx_delay;
        int kl, kr, zl, zr;
        int zcenter;

        DspFloatType l, r, ldl, rdl, zdr, zdl;
        DspFloatType rflange0, rflange1, lflange0, lflange1, oldrflange0, oldrflange1, oldlflange0, oldlflange1;
        DspFloatType period_const, base, ibase;
        DspFloatType *ldelay, *rdelay, *zldelay, *zrdelay;
        DspFloatType oldl, oldr;		//pt. lpf
        DspFloatType rsA, rsB, lsA, lsB;	//Audio sample at given delay

        delayline *ldelayline0, *rdelayline0, *ldelayline1, *rdelayline1;
        class FPreset *Fpre;


    };
}