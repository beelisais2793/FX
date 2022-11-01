#pragma once

// https://github.com/titas2001/Diode_Clipper
// https://github.com/titas2001/Diode_Ring_Modulator

#include "FX/LiquidNeuron.hpp"
#include "FX/Diode.hpp"

namespace Analog::Distortion::Diode
{
    struct Dioder
    {
        Liquid::LiquidPole vt,eta,is;
        double Vt,Eta,Is;
        int ctr=0;
        Dioder()  :vt(1/0.02,1000.0),eta(1/0.02,1000),is(1/0.02,1000)
        {        
            Vt  = noise.rand()*0.01;
            Eta = noise.rand()*0.1;
            Is  = noise.rand()*0.001;
            ctr=1000;
        }    
        double Tick(double In, double V=1, double E=1, double I=1)
        {
            ctr++;
            double v = 0.0253+vt.Tick(Vt);        
            double e = 1.68+eta.Tick(Eta);
            double i = .015+is.Tick(Is);
            
            if(ctr >= sampleRate/1000) {
                Vt  = noise.rand()*0.01;
                Eta = noise.rand()*0.1;
                Is  = noise.rand()*0.001;
                ctr = 0;
            }                    
            return FX::Distortion::Diode::Diode(In,v,e,i);
        }
    };    
}