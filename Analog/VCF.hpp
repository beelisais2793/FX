#pragma once

#include <cmath>
#include "SoundObject.hpp"

#include "FX/LiquidNeuron.hpp"
#include "FX/Amplifiers.hpp"
#include "FX/ClipFunctions.hpp"

namespace Analog
{

    struct VCF : FilterProcessor
    {        
        //DSP::Decimator9 decimator;
        DspFloatType f,fs,k,p,scale,r,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
        DspFloatType cutoff,Q;
        DspFloatType F,R,P,SCALE,K;
        DspFloatType x;
        DspFloatType envScale = 0.5f;
        int oversample = 1;
        DspFloatType low,high,band,notch;
        DspFloatType machoScale = 1.0;
        DspFloatType preGain=1.0;
        DspFloatType postGain=1.0;
        DspFloatType dcBias=0.0;
        DspFloatType cMin=-1.0;
        DspFloatType cMax=1.0;

        bool sat1=false;
        bool sat2=false;
        bool sat3=false;
        bool sat4=false;
        bool sigd1=false;
        bool sigd2=false;
        bool invertP = false;
                
        enum {
            ONEPOLE,
            TWOPOLE,
            THREEPOLE,
            FOURPOLE,
        };
        enum {
            MOOG,
            LOWPASS,
            HIGHPASS,
            BANDPASS,
            NOTCH,
            APF,
            UBS,
            PEAK,
            SHELF,        
        };

        int poles = FOURPOLE;
        int type  = MOOG;

        enum {
            PORT_CUTOFF,
            PORT_RESONANCE,
            PORT_SAT1,
            PORT_SAT2,
            PORT_SAT3,
            PORT_SAT4,
            PORT_SIGD1,
            PORT_SIGD2,
            PORT_INVERTP,
            PORT_ONEPOLE,
            PORT_TWOPOLE,
            PORT_THREEPOLE,
            PORT_FOURPOLE,
            PORT_MOOG,
            PORT_LP,
            PORT_HP,
            PORT_BP,
            PORT_NOTCH,
            PORT_APF,
            PORT_UBS,
            PORT_PEAK,
            PORT_SHELF,
            PORT_DCBIAS,
            PORT_OVERSAMPLE,
            PORT_ENVSCALE,
            PORT_PREGAIN,
            PORT_POSTGAIN,
        };

        Liquid::LiquidPole p1;
        FX::Filters::Smoothers::CSmoothFilter integrator;
        FX::Filters::Smoothers::CSmoothFilter cutSmooth,resSmooth,outSmooth, x1Smooth,x2Smooth,x3Smooth,x4Smooth;
        

        VCF(DspFloatType sampleRate, DspFloatType cutoff, DspFloatType resonance)
        : FilterProcessor(),
        integrator(0.9*sampleRate,sampleRate*oversample),
        cutSmooth(sampleRate,1/0.001),
        resSmooth(sampleRate,1/0.001),
        outSmooth(sampleRate,1/0.1),
        x1Smooth(sampleRate,1/0.1),
        x2Smooth(sampleRate,1/0.1),
        x3Smooth(sampleRate,1/0.1),
        x4Smooth(sampleRate,1/0.1)
        {                
            coefficients(sampleRate,cutoff,resonance);
            x=y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
            low=high=band=notch=0;
        }

        void coefficients(DspFloatType sampleRate,DspFloatType frequency, DspFloatType resonance) 
        {
            if(frequency < 0.01) frequency = 0.01;
            if(frequency > 10) frequency = 10;
            if(resonance < 0.05) resonance = 0.05;
            if(resonance > 1) resonance = 1;

            F = frequency;            
            R = resonance;
            fs = sampleRate;

            cutoff = cutSmooth.process(F);
            Q = resSmooth.process(R);
            
            f =  cv2freq(cutoff) / (oversample*sampleRate); //[0 - 1]
            K = 3.6*f - 1.6*f*f -1; //(Empirical tuning)
            
            // super resonance
            //P = -(k+1)*0.5;
            
            P = (k+1)*0.5;
            if(invertP) P = -P;
                        
            SCALE = std::exp((1-P)*1.386249);            
            SCALE *= machoScale;
            
        }
        void setCutoff(DspFloatType c) {     
            c = clamp(c,0,10);
            coefficients(fs,c,Q);
        }
        void setResonance(DspFloatType res) {
            res = clamp(res,0,1);
            coefficients(fs,cutoff,res);
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
            case PORT_CUTOFF: setCutoff(v); break;
            case PORT_RESONANCE: setResonance(v); break;
            case PORT_SAT1: sat1 = !sat1; break;
            case PORT_SAT2: sat2 = !sat2; break;
            case PORT_SAT3: sat3 = !sat3; break;
            case PORT_SAT4: sat4 = !sat4; break;
            case PORT_SIGD1: sigd1 = !sigd1; break;
            case PORT_SIGD2: sigd2 = !sigd2; break;
            case PORT_INVERTP: invertP = !invertP; break;
            case PORT_ONEPOLE: poles = ONEPOLE; break;
            case PORT_TWOPOLE: poles = TWOPOLE; break;
            case PORT_THREEPOLE: poles = THREEPOLE; break;
            case PORT_FOURPOLE: poles = FOURPOLE; break;
            case PORT_MOOG: type = MOOG; break;
            case PORT_LP: type = LOWPASS; break;
            case PORT_HP: type = HIGHPASS; break;
            case PORT_BP: type = BANDPASS; break;
            case PORT_NOTCH: type = NOTCH; break;
            case PORT_OVERSAMPLE: oversample = (int)v; break;
            case PORT_ENVSCALE: envScale = v; break;
            case PORT_PREGAIN: preGain = v; break;
            case PORT_POSTGAIN: postGain = v; break;
            }
        }
        DspFloatType saturate(DspFloatType x)
        {
            // changing it will change the resonance but too much and it only resonates
            return 1.03 * FX::Distortion::serpent_curve(x);
        }
        DspFloatType Tick(DspFloatType input, DspFloatType A=1, DspFloatType X=2, DspFloatType Y=1)
        {
            Undenormal denormal;
            DspFloatType c = F;
            DspFloatType res = R;
                    
            X *= envScale;
            X = clamp(X,0,1);
            coefficients(fs,c*X,res*Y);
            
            p = P; // x1Smooth.process(P);
            scale = SCALE; //x2Smooth.process(SCALE);
            k = K; //x3Smooth.process(K);
            r = Q*scale; //x4Smooth.process(Q*scale);
                      
            input *= std::pow(10.0,preGain/20.0f);
            if(sigd1) input = FX::Distortion::sigmoid_function(input);

            for(size_t i = 0; i < oversample; i++)
            {
                //--Inverted feed back for corner peaking
                x = input+dcBias - r*y4;
                //Four cascaded onepole filters (bilinear transform)
                y1=x*p + oldx*p - k*y1;      
                
                // saturate very badly messed the reso and cutoff
                if(sat1) y1 = saturate(y1);
                y2=y1*p + oldy1*p - k*y2;                        
                if(sat2) y2 = saturate(y2);
                y3=y2*p + oldy2*p - k*y3;                        
                if(sat3) y3 = saturate(y3);
                y4=y3*p + oldy3*p - k*y4;                        
                if(sat4) y4 = saturate(y4);

                // super but resonance needs to be tamed
                //if(sigd2) y4 = Distortion::serpent_curve(A*y4);
                                       
                oldx  = x;
                oldy1 = y1;
                oldy2 = y2;
                oldy3 = y3;
            }

            coefficients(fs,c,res);

            
            y4 -= dcBias;            
            DspFloatType out = y4; //y4*integrator.Tick(y4);
            if(out < cMin) out = cMin;
            if(out > cMax) out = cMax;

            if(poles == ONEPOLE) out = y1;
            else if(poles == TWOPOLE) out = y2;
            else if(poles == THREEPOLE) out = y3;
            else if(poles == FOURPOLE) out = y4;

            if(type != MOOG) {
                DspFloatType x_scale = 1.5*(1-Q);
                if(x_scale == 0.0f) x_scale = 0.001f;        
                DspFloatType x_f     = std::sin(2 * M_PI * cutoff/fs);        

                low   = low + f * band;
                high  = x_scale*out - low - x_scale*band;
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
            }
            
            out *= A;
            if(sigd2) out = FX::Distortion::serpent_curve(out);
            //for(size_t i = 0; i < oversample/2; i++) out = decimator.Tick(out);
            return out;
        }

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };

    struct VCF2
    {
        VCF f1,f2;

        VCF2(DspFloatType sampleRate, DspFloatType cutoff, DspFloatType resonance) :
            f1(sampleRate,cutoff,resonance),
            f2(sampleRate,cutoff,resonance)
        {

        }

        DspFloatType Cascade(DspFloatType x) {
            DspFloatType r = f1.Tick(x);
            return f2.Tick(x);
        }
        DspFloatType Serial(DspFloatType x) {
            return f1.Tick(x) * f2.Tick(x);
        }
        DspFloatType Parallel(DspFloatType x) {
            return f1.Tick(x) + f2.Tick(x);
        }
        DspFloatType Modulus(DspFloatType x) {
            return fmod(f1.Tick(x), f2.Tick(x));
        }
        DspFloatType Morph(DspFloatType x, DspFloatType f = 0.5) {
            return Liquid::function_morph(f1.Tick(x), f2.Tick(x), f);
        }

    };

    struct VCFBANK
    {
        std::vector<VCF*> filters;

        VCFBANK(size_t n)        
        {
            filters.resize(n);
            for(size_t i = 0; i < n; i++)
                filters[i] = new VCF(sampleRate,1000.0f,0);
        }
        ~VCFBANK() {
            for(size_t i = 0; i < filters.size(); i++)
                delete filters[i];
        }
        DspFloatType Cascade(DspFloatType x) {
            DspFloatType r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = filters[i]->Tick(x);
            return x;
        }
        DspFloatType Serial(DspFloatType x) {
            DspFloatType r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x *= filters[i]->Tick(x);
            return x;
        }
        DspFloatType Parallel(DspFloatType x) {
            DspFloatType r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x += filters[i]->Tick(x);
            return x;
        }
        DspFloatType Modulus(DspFloatType x) {
            DspFloatType r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = fmod(x,filters[i]->Tick(x));
            return x;
        }
        DspFloatType Morph(DspFloatType x, DspFloatType f = 0.5) {
            DspFloatType r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = Liquid::function_morph(x, filters[i]->Tick(x), f);
            return x;        
        }
    };
}