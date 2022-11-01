#pragma once

#include <array>
#include <cmath>
#include "Undernomals.hpp"

// Classes definition
class Leslie
{
    const int DELAY_WIDTH=4096;
    array <DspFloatType> buffer(DELAY_WIDTH);
    DspFloatType v1, yold;
    DspFloatType v1_inv, yold_inv;
    DspFloatType phase;
    DspFloatType fs;
    DspFloatType mind,maxd;
    DspFloatType rate;
    DspFloatType dry,wet;
    DspFloatType factor;
    DspFloatType a1,b0,b1;

    Leslie(DspFloatType Fs)
    {
        fs = Fs;
        reset();
    }
    void reset()
    {   v1 = yold = 0;
        v1_inv = yold_inv = 0;
        phase = 1;
        memset(&buufer[0],0,DELAY_WIDTH*sizeof(DspFloatType));        
    }

    void Update() 
    {

        mind = 7.0/1000*fs;
        maxd = mind + 0.5/1000*fs;

        factor = (2 * PI * rate / fs);
        
        const DspFloatType tanw0 = tan(40 * PI / fs);
        const DspFloatType tanw0plusinv = 1.0 / (tanw0 + 1.0); 
        
        a1 = (tanw0 - 1) * tanw0plusinv;
        b0 = tanw0 * tanw0plusinv;
        b1 = b0;
    }    
    void TickStereo(DspFloatType& sampleL, DspFloatType& sampleR)
    {
        Undenormal denormals;
        Update();
        const DspFloatType input = 0.5 * (sampleL + sampleR);

        phase += factor;
        if (phase > 2 * PI) phase -= 2 * PI;

        const DspFloatType lfo = sin(phase);        
        const DspFloatType lfoD = (maxd - mind) * (0.5 * lfo + 0.5) + mind + 1;
        
        const int k = int (floor(lfoD));
        const DspFloatType frac = lfoD - k;

        const DspFloatType y = buffer[(cpt + k) & (DELAY_WIDTH-1)] * (1 - frac) 
                       + buffer[(cpt + k + 1) & (DELAY_WIDTH-1)] - (1 - frac) * yold;
        yold = y;

        const DspFloatType lfoD_inv = (maxd - mind) * (0.5 - 0.5 * lfo) + mind + 1;

        const int k_inv = int (floor(lfoD_inv));
        const DspFloatType frac_inv = lfoD_inv - k_inv;

        const DspFloatType y_inv = buffer[(cpt + k_inv) & (DELAY_WIDTH-1)] * (1 - frac_inv) 
                           + buffer[(cpt + k_inv + 1) & (DELAY_WIDTH-1)] - (1 - frac_inv) * yold_inv;
        yold_inv = y_inv;

        buffer[cpt] = input;

        // tremolo
        const DspFloatType output = y * (1 + lfo);
        const DspFloatType output_inv = y_inv * (1 - lfo);

        // dry/wet
        sampleL = sampleL * dry + 0.5 * (output + 0.7 * output_inv) * wet;
        sampleR = sampleR * dry + 0.5 * (output_inv + 0.7 * output) * wet;

        
    }
    void Tick(DspFloatType sample)
    {
        DspFloatType L = sample;
        DspFloatType R = sample;
        TickStereo(L,R);
        return 0.5*(L+R);
    }     
};