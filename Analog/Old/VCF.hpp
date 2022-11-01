#pragma once

#include "FX/LiquidNeuron.hpp"
#include "FX/Amplifiers.hpp"
#include "FX/ClipFunctions.hpp"

namespace Analog
{
    struct VCF : FilterProcessor
    {
    //Init
    // cutoff = cutoff freq in Hz
    //fs = sampling frequency //(e.g. 44100Hz)
    //res = resonance [0 - 1] //(minimum - maximum)

        
        DSP::Decimator9 decimator;
        double f,fs,k,p,scale,r,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
        double cutoff,Q;
        double F,R,P,SCALE,K;
        double x;
        double envScale = 0.5f;
        const int oversample = 1;
        double low,high,band,notch;
        double machoScale = 1.0;
        double preGain=1.0;
        double postGain=1.0;
        double dcBias=0.0;
        double cMin=-1.0;
        double cMax=1.0;

        bool sat1=false;
        bool sat2=false;
        bool sat3=false;
        bool sat4=false;
        bool sigd1=false;
        bool sigd2=false;
        bool invertP = false;

        FX::Filters::Smoothers::CSmoothFilter cutSmooth,resSmooth,outSmooth, x1Smooth,x2Smooth,x3Smooth,x4Smooth;
        
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

        int poles = TWOPOLE;
        int type  = HIGHPASS;

        Liquid::LiquidPole p1;
        FX::Filters::Smoothers::CSmoothFilter integrator;

        VCF(double sampleRate, double cutoff, double resonance)
        : FilterProcessor(),
        integrator(0.9*sampleRate,sampleRate*oversample),
        cutSmooth(sampleRate,1/0.1),
        resSmooth(sampleRate,1/0.1),
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

        void coefficients(double sampleRate,double frequency, double resonance) 
        {
            if(frequency < 0.01) frequency = 0.01;
            if(frequency > 10) frequency = 10;
            if(resonance < 0.05) resonance = 0.05;
            if(resonance > 0.95) resonance = 0.95;

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
        void setCutoff(double c) {     
            c = clamp(c,0,10);
            coefficients(fs,c,Q);
        }
        void setResonance(double res) {
            res = clamp(res,0,1);
            coefficients(fs,cutoff,res);
        }
        double saturate(double x)
        {
            // changing it will change the resonance but too much and it only resonates
            return 1.03 * FX::Distortion::serpent_curve(x);
        }
        double Tick(double input, double A=1, double X=1, double Y=1)
        {
            Undenormal denormal;
            double c = F;
            double res = R;
                    
            X *= envScale;
            X = clamp(X,0,1);
            coefficients(fs,c*X,res*Y);
            
            p = x1Smooth.process(P);
            scale = x2Smooth.process(SCALE);
            k = x3Smooth.process(K);
            r = x4Smooth.process(Q*scale);
            //r = Q*scale;
            

            input *= pow(10.0,preGain/20.0f);
            if(sigd1) input = FX::Distortion::sigmoid_function(input);

            for(size_t i = 0; i < oversample; i++)
            {
                //--Inverted feed back for corner peaking
                x = clamp(( input+dcBias) - r*y4,cMin,cMax);                
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
            double out = y4; //y4*integrator.Tick(y4);
            
            if(poles == ONEPOLE) out = y1;
            else if(poles == TWOPOLE) out = y2;
            else if(poles == THREEPOLE) out = y3;
            else if(poles == FOURPOLE) out = y4;

            if(type != MOOG) {
                double x_scale = 1.5*(1-Q);
                if(x_scale == 0.0f) x_scale = 0.001f;        
                double x_f     = std::sin(2 * M_PI * cutoff/fs);        

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
            for(size_t i = 0; i < oversample/2; i++) out = decimator.Tick(out);
            return out;
        }
    };

    struct VCF2
    {
        VCF f1,f2;

        VCF2(double sampleRate, double cutoff, double resonance) :
            f1(sampleRate,cutoff,resonance),
            f2(sampleRate,cutoff,resonance)
        {

        }

        double Cascade(double x) {
            double r = f1.Tick(x);
            return f2.Tick(x);
        }
        double Serial(double x) {
            return f1.Tick(x) * f2.Tick(x);
        }
        double Parallel(double x) {
            return f1.Tick(x) + f2.Tick(x);
        }
        double Modulus(double x) {
            return fmod(f1.Tick(x), f2.Tick(x));
        }
        double Morph(double x, double f = 0.5) {
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
        double Cascade(double x) {
            double r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = filters[i]->Tick(x);
            return x;
        }
        double Serial(double x) {
            double r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x *= filters[i]->Tick(x);
            return x;
        }
        double Parallel(double x) {
            double r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x += filters[i]->Tick(x);
            return x;
        }
        double Modulus(double x) {
            double r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = fmod(x,filters[i]->Tick(x));
            return x;
        }
        double Morph(double x, double f = 0.5) {
            double r = filters[0]->Tick(x);
            for(size_t i = 1; i < filters.size(); i++)
                x = Liquid::function_morph(x, filters[i]->Tick(x), f);
            return x;        
        }
    };
}