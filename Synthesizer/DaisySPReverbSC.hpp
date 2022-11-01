#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct ReverbSC : public StereoFXProcessorPlugin<daisysp::ReverbSc>
    {
        ReverbSC() : StereoFXProcessorPlugin<daisysp::ReverbSc>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_FEEDBACK,
            PORT_LPFREQ,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FEEDBACK: this->SetFeedback(v); break;
                case PORT_LPFREQ: this->SetLpFreq(v); break;
            }
        }
        double Tick(double iL, double iR, double & oL, double &oR, double A=1, double X=1, double Y=1) {
            float L,R;
            L = iL;
            R = iR;
            float o1,o2;
            this->Process(L,R,&o1,&o2);
            oL = L;
            oR = R;
            return 0.5*(oL+oR);
        }
        void ProcessBlock(size_t n, float ** in, float ** out) {
            for(size_t i = 0; i < n; i++) 
            {
                Tick(in[0][i],in[1][i],out[0][i],out[1][i]);
            }
        }
    };
}