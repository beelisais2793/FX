#pragma once 
#include <cmath>
#include "ClipFunctions.hpp"
#include "Undenormal.hpp"

namespace Analog::Filters::Moog::MoogFilter1
{
    struct MoogFilter1 
    {
    //Init
    // cutoff = cutoff freq in Hz
    //fs = sampling frequency //(e.g. 44100Hz)
    //res = resonance [0 - 1] //(minimum - maximum)

        float f,fs,k,p,scale,r,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
        float cutoff,Q;
        float x;

        MoogFilter1(float sampleRate, float cutoff, float resonance) {
                    
            coefficients(sampleRate,cutoff,resonance);
            x=y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
        }

        void coefficients(float sampleRate,float frequency, float resonance) 
        {
            fs = sampleRate;
            cutoff = frequency;
            Q = resonance;

            f = 2 * cutoff / fs; //[0 - 1]
            k = 3.6*f - 1.6*f*f -1; //(Empirical tuning)
            p = (k+1)*0.5;

            // resonance sucks 
            scale = std::exp((1-p)*1.386249);
            r = resonance*scale;        
            //float t=(1.f-p)*1.386249f;
            //float t2=12.f+t*t;
            //r = Q*(t2+6.f*t)/(t2-6.f*t);
        }
        void setCutoff(float c) {        
            c = clamp(c,0,fs/2);
            coefficients(fs,c,Q);
        }
        void setResonance(float res) {
            res = clamp(res,0,1);
            coefficients(fs,cutoff,res);
        }
        float Tick(float input, float A=1, float X=0, float Y=0)
        {
            Undenormal denormal;
            float c = cutoff;
            float res = Q;
            coefficients(fs,c + 0.5*X*c,Q + 0.5*Y*Q);
            //Loop
            //--Inverted feed back for corner peaking
            x = input - r*y4;                
            
            //Four cascaded onepole filters (bilinear transform)
            y1=x*p + oldx*p - k*y1;        
            y2=y1*p+oldy1*p - k*y2;        
            y3=y2*p+oldy2*p - k*y3;        
            y4=y3*p+oldy3*p - k*y4;        

            coefficients(fs,c,res);

            //Clipper band limited sigmoid
            y4 = y4 - (y4*y4*y4)/6;        
            oldx  = x;
            oldy1 = y1;
            oldy2 = y2;
            oldy3 = y3;

            return A*y4;
        }
    };
}