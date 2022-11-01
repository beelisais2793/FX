#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    class Synthfilter
    {
    public:
        Synthfilter (DspFloatType * efxoutl_, DspFloatType * efxoutr_);
        ~Synthfilter ();
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

        //Control parameters
        void setvolume (int Pvolume);
        void setdistortion (int Pdistortion);
        void setwidth (int Pwidth);
        void setfb (int Pfb);
        void setdepth (int Pdepth);

        //Phaser parameters
        int Pvolume;			 //0//Used in Process.C to set wet/dry mix
        int Pdistortion;		 //1//0...127//Model distortion
        //2//Tempo//LFO frequency
        //3//0...127//LFO Random
        //4//0...max types//LFO Type
        //5//0...127//LFO stereo offset
        int Pwidth;			 //6//0...127//Phaser width (LFO amplitude)
        int Pfb;			 //7//-64...64//feedback
        int Plpstages;	         //8//0...12//Number of first-order Low-Pass stages
        int Phpstages;		 //9//0...12//Number of first-order High-Pass stages
        int Poutsub;			 //10//0 or 1//subtract the output instead of the adding it
        int Pdepth;			 //11//0...127//Depth of phaser sweep
        int Penvelope;		 //12//-64...64//envelope sensitivity
        int Pattack;			 //13//0...1000ms//Attack Time
        int Prelease;			 //14//0...500ms//Release Time
        int Pbandwidth;		 //15//0...127//Separate high pass & low pass



        //Internal Variables
        DspFloatType distortion, fb, width, env, envdelta, sns, att, rls, fbl, fbr, depth, bandgain;
        DspFloatType *lyn1, *ryn1, *lx1hp, *ly1hp, *rx1hp, *ry1hp;
        DspFloatType oldlgain, oldrgain, inv_period;

        DspFloatType delta;
        DspFloatType Rmin;	// 2N5457 typical on resistance at Vgs = 0
        DspFloatType Rmax;	// Resistor parallel to FET
        DspFloatType C, Clp, Chp;	        // Capacitor
        EffectLFO lfo;	         //Filter modulator

        class FPreset *Fpre;

    };
}