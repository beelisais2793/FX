///////////////////////////////////////////////////////////////////////////////
// simple amp
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SoundObject.hpp"

DspFloatType amp   = 3.0f;
DspFloatType gain = pow(10,amp/20.0f);

namespace FX
{
    void Sigmoid_ProcessBlock(size_t n, DspFloatType * inputs, DspFloatType * outputs)
    {
        for(size_t i = 0; i < n; i++)
        {
            outputs[i] = Distortion::sigmoid(-gain * inputs[i]);
        }
    }
    void Sigmoid_ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
    {
        for(size_t c = 0; c < 2; c++)
        for(size_t i = 0; i < n; i++)
        {
            outputs[c][i] = Distortion::sigmoid(-gain * inputs[c][i]);
        }
    }
    void DCSigmoid_ProcessBlock(size_t n, DspFloatType * inputs, DspFloatType * outputs)
    {
        DspFloatType dc = 0;
        for(size_t i = 0; i < n; i++)
        {
            DspFloatType x = inputs[i];
            DspFloatType r = (DspFloatType)rand() / (DspFloatType)RAND_MAX;
            if(x > 0) dc = 0.1;
            else if(x < 0) dc = -0.1;
            else dc = 0;
            outputs[i] = Distortion::sigmoid(-gain * (x+dc));
            outputs[i] -= dc;
        }
    }
    void DCSigmoid_ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
    {
        DspFloatType dc = 0;
        for(size_t c = 0; c < 2; c++)
        for(size_t i = 0; i < n; i++)
        {
            DspFloatType x = inputs[c][i];
            DspFloatType r = (DspFloatType)rand() / (DspFloatType)RAND_MAX;
            if(x > 0) dc = 0.1;
            else if(x < 0) dc = -0.1;
            else dc = 0;
            outputs[c][i] = Distortion::sigmoid(-gain * (x+dc));
            outputs[c][i] -= dc;
        }
    }
    void Amplifier_ProcessBlock(size_t n, DspFloatType * inputs, DspFloatType * outputs)
    {
        for(size_t i = 0; i < n; i++)
        {
            outputs[i] = std::tanh(gain * inputs[i]);
        }
    }
    void Amplifier_ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
    {
        for(size_t c = 0; c < 2; c++)
        for(size_t i = 0; i < n; i++)
        {
            outputs[c][i] = std::tanh(gain * inputs[c][i]);
        }
    }

    struct SimpleAmplifier : public AmplifierProcessor
    {
        
        DspFloatType amp;
        DspFloatType gain;

        enum {
            AMP,
            DCS,
            SIG
        };
        enum {
            AMP_PORT,
            GAIN_PORT,
            TYPE_PORT,
        };
        int type = AMP;
        
        SimpleAmplifier(DspFloatType a = 3.0f) : AmplifierProcessor() {
            amp   = a;
            gain = std::pow(10,amp/20.0f);
        }
        void printPorts() {
            printf("AMP_PORT=0\n");
            printf("GAIN_PORT=1\n");
            printf("TYPE_PORT=2\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case AMP_PORT: amp = v; break;
                case GAIN_PORT: gain = pow(10,amp/20.0f); break;
                case TYPE_PORT: type = ((int)v) % 3; break;
                default: printf("No port %d\n",port);
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType dX = 0, DspFloatType dY = 0)
        {        
            DspFloatType z = amp * I;
            if( z < 0) z += dX;
            if( z > 0) z += dY;
            if(z < X) z = X;
            if(z > Y) z = Y;        
            return A*std::tanh(gain*z);        
        }
        void ProcessBlock(size_t numSamples, DspFloatType * in, DspFloatType * out)
        {
            switch(type)
            {
                case AMP: Amplifier_ProcessBlock(numSamples,in,out); break;
                case DCS: DCSigmoid_ProcessBlock(numSamples,in,out); break;
                case SIG: Sigmoid_ProcessBlock(numSamples,in,out); break;
            }            
        }

        void randomize() {
			amp  = noise.rand();
			gain = noise.rand();
            type = noise.randint(0,2);
		}		
    };
    void zeros(std::vector<DspFloatType> & t)
    {
        memset(t.data(),0,sizeof(DspFloatType)*t.size());  
    }

    template<typename T>
    void setData(T x[2],T value) {
        x[0] = value;
        x[1] = value;
    }
}