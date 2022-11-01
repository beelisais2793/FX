#pragma once

#include "FX/WDFDiodeClipper.hpp"
#include "FX/DiodeSimulator.hpp"
#include "FX/Diode.hpp"

namespace Analog
{
    struct VCA : AmplifierProcessor
    {
        float gain;
        FX::Filters::Smoothers::CSmoothFilter gainSmooth,vtSmooth,etaSmooth,isSmooth;
        double Vt = 0.0253;
        double Eta = 1.68;
        double Is = .105;    
        double dcBias = 0.0;
        WDFDiodeClipper clip;
        

        VCA(float g = 1.0) 
        :   AmplifierProcessor(),
            gainSmooth(sampleRate,1/0.1),
            vtSmooth(sampleRate,1/0.1),
            etaSmooth(sampleRate,1/0.1),
            isSmooth(sampleRate,1/0.1) 
        {
            gain = pow(10,g/20.0);
            clip.prepare(sampleRate);
            clip.setCircuitParams(1000.0f);
        }
        void Reset()
        {
            Vt = 0.0253;
            Eta = 1.68;
            Is = 0.105;
        }
        void Randomize()
        {
            int n = noise.randint(-1,1);
            Vt += n <= 0? -1.0:1.0f*noise.rand()*0.001;
            if(Vt < 0.01) Vt = 0.01;
            if(Vt > 0.05) Vt = 0.05;
            
            n = noise.randint(-1,1);
            Eta += n <= 0? -1.0:1.0f*noise.rand()*0.01;
            if(Eta < 1.5) Eta = 1.5;
            if(Eta > 1.7) Eta = 1.7;
            
            n = noise.randint(-1,1);        
            Is += n <= 0? -1.0:1.0f*noise.rand()*0.005;
            if(Is < 0.05) Is = 0.05;
            if(Is > 0.2) Is = 0.2;
            
        }
        double Tick(double I, double A = 1, double X = -1, double Y = 1)
        {        
            double r = gainSmooth.process(gain)*(I+dcBias);                
            if(r < X) r = X;
            if(r > Y) r = Y;
            r = std::tanh(FX::Distortion::Diode::Diode(A*r,vtSmooth.process(Vt), etaSmooth.process(Eta), isSmooth.process(Is)));            
            clip.processSample(A*r);
            r -= dcBias;                            
            return r;
        }
    };
}

