#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{
    struct Chorus : public MonoFXProcessorPlugin<gam::Chorus<double>>
    {
        Chorus() : MonoFXProcessorPlugin<gam::Chorus<double>>()
        {

        }
        enum {
            PORT_MODULATE,
            PORT_MAXDELAY,
            PORT_DELAY,
            PORT_FBK,
            PORT_FFD,
            PORT_FREQ,
            PORT_DEPTH,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_MODULATE: this->modulate(); break;
                case PORT_MAXDELAY: this->maxDelay(v); break;
                case PORT_DELAY: this->delay(v); break;
                case PORT_FBK: this->fbk(v); break;
                case PORT_FFD: this->ffd(v); break;
                case PORT_FREQ: this->freq(v); break;
                case PORT_DEPTH: this->depth(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };

    struct StereoChorus : public StereoFXProcessorPlugin<gam::Chorus<double>>
    {
        StereoChorus() : StereoFXProcessorPlugin<gam::Chorus<double>>()
        {

        }
        enum {
            PORT_MODULATE,
            PORT_MAXDELAY,
            PORT_DELAY,
            PORT_FBK,
            PORT_FFD,
            PORT_FREQ,
            PORT_DEPTH,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_MODULATE: this->modulate(); break;
                case PORT_MAXDELAY: this->maxDelay(v); break;
                case PORT_DELAY: this->delay(v); break;
                case PORT_FBK: this->fbk(v); break;
                case PORT_FFD: this->ffd(v); break;
                case PORT_FREQ: this->freq(v); break;
                case PORT_DEPTH: this->depth(v); break;
            }
        }
        double Tick(double iL, double iR, double &L, double &R, double A=1, double X=1, double Y=1) {
            (*this)(iL,iR,L,R);
            L *= A;
            R *= A;
            return 0.5*(L+R);
        }
        void ProcessBlock(size_t n, float ** in, float ** out) {
            for(size_t i = 0; i < n; i++) 
                Tick(in[i][0],in[i][1],out[0][i],out[1][i]);
        }
    };
}