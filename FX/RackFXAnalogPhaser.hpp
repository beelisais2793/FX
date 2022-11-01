#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{


    class Analog_Phaser
    {
    public:
        Analog_Phaser (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Analog_Phaser ();
        void out (DspFloatType * smpsl, DspFloatType * smpsr);
        void setpreset (int npreset);
        void changepar (int npar, int value);
        int getpar (int npar);
        void cleanup ();
        int Ppreset;
        DspFloatType *efxoutl;
        DspFloatType *efxoutr;
        DspFloatType outvolume;

    private:
        //Phaser parameters
        EffectLFO lfo;		//Phaser modulator
        int Pvolume;        //Used in Process.C to set wet/dry mix
        int Pdistortion;    //Model distortion added by FET element
        int Pwidth;		//Phaser width (LFO amplitude)
        int Pfb;		//feedback
        int Poffset;	//Model mismatch between variable resistors
        int Pstages;	//Number of first-order All-Pass stages
        int Poutsub;	//if I wish to subtract the output instead of the adding it
        int Phyper;		//lfo^2 -- converts tri into hyper-sine
        int Pdepth;         //Depth of phaser sweep
        int Pbarber;         //Enable barber pole phasing

        //Control parameters
        void setvolume (int Pvolume);
        void setdistortion (int Pdistortion);
        void setwidth (int Pwidth);
        void setfb (int Pfb);
        void setoffset (int Poffset);
        void setstages (int Pstages);
        void setdepth (int Pdepth);

        //Internal Variables
        bool barber;			//Barber pole phasing flag
        DspFloatType distortion, fb, width, offsetpct, fbl, fbr, depth;
        DspFloatType *lxn1, *lyn1,*rxn1, *ryn1, *offset;
        DspFloatType oldlgain, oldrgain, rdiff, ldiff, invperiod;

        DspFloatType mis;
        DspFloatType Rmin;	// 2N5457 typical on resistance at Vgs = 0
        DspFloatType Rmax;	// Resistor parallel to FET
        DspFloatType Rmx;		// Rmin/Rmax to avoid division in loop
        DspFloatType Rconst;      // Handle parallel resistor relationship
        DspFloatType C;	        // Capacitor
        DspFloatType CFs;		// A constant derived from capacitor and resistor relationships


        class FPreset *Fpre;

    };

}