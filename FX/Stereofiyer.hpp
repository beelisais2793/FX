#pragma once

#include "Oscillators/WaveGenerators.hpp"

namespace FX
{
    struct Stereofyier
    {
        DspFloatType pan = 0.5;
        SinewaveGenerator lfo;
        DspFloatType WideCoeff = 1;
        DspFloatType rotAmt = 0.5;
        DspFloatType lfoFreq = 0.01;

        Stereofyier() : lfo(lfoFreq,sampleRate)
        {
            
        }
        void setFrequency(DspFloatType f) {
            lfo.setFrequency(f);
        }
        void setPan(DspFloatType p) {
            pan = p;
        }
        DspFloatType panLeft(DspFloatType x, DspFloatType lfoTick) {

            return x + rotAmt *x * sin(lfoTick * (1-pan)*M_PI/2);        
        }
        DspFloatType panRight(DspFloatType x, DspFloatType lfoTick) {
            return x + rotAmt * x * cos(lfoTick * pan*M_PI/2);
        }
        void ProcessBuffer(size_t n, DspFloatType ** inputs, DspFloatType ** outputs) 
        {            
            for(size_t i = 0; i < n; i++)
            {
                DspFloatType inL,inR;
                DspFloatType lfoTick = lfo.Tick();
                inL = inputs[0][i];
                inR = inputs[1][i];
            
                DspFloatType MonoSign   = (inL + inR)/2.0;
                DspFloatType DeltaLeft  = inL - MonoSign;
                DeltaLeft *= WideCoeff;
                DspFloatType DeltaRight = inR - MonoSign;
                DeltaRight *= WideCoeff;

                inL += DeltaLeft;
                inR += DeltaRight;

                // calculate scale coefficient
                DspFloatType coef_S = WideCoeff*0.5;

                // then do this per sample
                DspFloatType m = MonoSign;
                DspFloatType s = (inL - inR )*coef_S;

                DspFloatType out_left  = m - s;
                DspFloatType out_right = m + s;

                outputs[0][i] = panRight(out_left,lfoTick);
                outputs[1][i] = panLeft(out_right,lfoTick);  
            }
        }

        void InplaceProcess(size_t numSamples, DspFloatType ** buffer)
        {
            ProcessBuffer(numSamples,buffer,buffer);
        }
    };
}