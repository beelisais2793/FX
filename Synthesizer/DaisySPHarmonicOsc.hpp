#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Synthesis
{
    struct HarmonicOsc : public GeneratorProcessorPlugin<daisysp::HarmonicOscillator>
    {
        int amp_idx=0;
        HarmonicOsc(float freq, float * amps) : GeneratorProcessorPlugin<daisysp::HarmonicOscillator>()
        {
            this->Init(sampleRate);
            this->SetFreq(freq);
            this->SetAmplitudes(amps);
        }
        enum {
            PORT_FREQ,
            PORT_FIRSTIDX,            
            PORT_AMP,        
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->SetFreq(v); break;
                case PORT_FIRSTIDX: this->SetFirstHarmIdx(v); break;
            }            
        }
        void setPort2(int port, double x, double y) {
            switch(port) {
                case PORT_AMP: this->SetSingleAmp(x,y);
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            return A*this->Process();
        }
    };
}