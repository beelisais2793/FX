#pragma once


#include "MoogFilters.hpp"

namespace Liquid
{
    struct LiquidMoog
    {
        Analog::MoogFilters::MoogLadderFilter moog;
        DspFloatType cutoff,Q;
        DspFloatType low,high,band,notch;
        enum {
            LOWPASS,
            HIGHPASS,
            BANDPASS,
            NOTCH,
            APF,
            UBS,
            PEAK,
            SHELF,        
        };
        
        int type  = LOWPASS;

        LiquidMoog(Analog::MoogFilters::MoogModelType type = Analog::MoogFilters::MoogModelType::FINN_MOOG) :
        moog(type,sampleRate)
        {
            cutoff = Q = low = high = band = notch = 0;
        }

        void setCutoff(DspFloatType c) {
            cutoff = cv2freq(c);
            moog.SetCutoff(cutoff);
        }
        void setResonance(DspFloatType q) {
            Q = q;
            moog.SetResonance(q);
        }
        void setType(Analog::MoogFilters::MoogModelType type) {
            moog.setType(type);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            DspFloatType out;

            low = moog.Tick(I,A,X,Y);

            DspFloatType x_scale = 1.5*(1.0-Q);
            if(x_scale == 0.0f) x_scale = 0.001f;
            DspFloatType x_f     = 2*std::sin(2 * M_PI * cutoff/sampleRate);        

            high  = x_scale*I - low - x_scale*band;
            band  = x_f * high + band;
            notch = high + low;
            /*
            ubp = 2 * scale * bp;              
            shelf = input + 2*K*scale*bp;        
            apf   = xn - 4*scale*bp;
            peak  = lp - hp;
            */
            switch(type)
            {
                case LOWPASS: out = low; break;
                case HIGHPASS: out = high; break;
                case BANDPASS: out = band; break;
                case NOTCH: out = notch; break;
            }
            out *= A;
            return out;
        }
    };
}