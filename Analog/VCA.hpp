#pragma once

#include <cmath>
#include "SoundObject.hpp"

#include "FX/WDFDiodeClipper.hpp"
#include "FX/DiodeSimulator.hpp"
#include "FX/Diode.hpp"

namespace Analog
{
    struct VCA : AmplifierProcessor
    {
    private:
        WDFDiodeClipper clip;
    public:
        DspFloatType gain;
        FX::Filters::Smoothers::CSmoothFilter gainSmooth,vtSmooth,etaSmooth,isSmooth;
        DspFloatType Vt = 0.0253;
        DspFloatType Eta = 1.68;
        DspFloatType Is = .105;    
        DspFloatType dcBias = 0.0;
        
        

        VCA(DspFloatType g = 1.0) 
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

        enum {
            PORT_GAIN,
            PORT_VT,
            PORT_ETA,
            PORT_IS,
            PORT_DCBIAS,
            PORT_RESET,
            PORT_CIRCUIT,
            PORT_RANDOMIZE,
        };
        void setPort(int port, DspFloatType v)
        {
            switch(port)
            {
                case PORT_GAIN: gain = pow(10.0,v/20.0); break;
                case PORT_VT: Vt = v; break;
                case PORT_ETA: Eta = v; break;
                case PORT_IS: Is = v; break;
                case PORT_DCBIAS: dcBias = v; break;
                case PORT_RESET: Reset(); break;
                case PORT_CIRCUIT: clip.setCircuitParams(v); break;
                case PORT_RANDOMIZE: randomize(); break;
            }
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
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1)
        {        
            DspFloatType r = gainSmooth.process(gain)*(I+dcBias);                
            if(r < X) r = X;
            if(r > Y) r = Y;
            r = std::tanh(FX::Distortion::Diode::Diode(A*r,vtSmooth.process(Vt), etaSmooth.process(Eta), isSmooth.process(Is)));            
            clip.processSample(A*r);
            r -= dcBias;                            
            return r;
        }

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}

