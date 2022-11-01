#pragma once

namespace DSP
{
    // Mixer Processor
    // ProcessBlock(n,block,float**,float*)  pan/morph
    // ProcessBlock(n,block,float**,float**) pan/morph
    
    struct SignalMixdown : public MixerProcessor
    {
        double mix;

        SignalMixdown(double m) : MixerProcessor() {
            mix = m;
        }
        void setMix(double m) {
            mix = m;
        }
        void ProcessBlock(size_t n, size_t block, float ** inputs, float * output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = 0;
                for(size_t j = 0; j < block; j++)
                {
                    output[i] += mix*inputs[i][j];
                }
                output[i] /= (double)n;
            }

        }
    };

    struct SignalMixer : public MixerProcessor
    {
        std::vector<double> mix;

        SignalMixdown(size_t n) : MixerProcessor() {
            mix.resize(n);
            for(size_t i = 0; i < n; i++) mix[i] = 1.0;
        }
        void setMix(size_t channel, double m) {
            mix[channel] = 1;
        }
        void ProcessBlock(size_t n, size_t block, float ** inputs, float * output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = 0;
                for(size_t j = 0; j < block; j++)
                {
                    output[i] += mix[i]*inputs[i][j];
                }
                output[i] /= (double)n;
            }
        }
    };
    
    struct MonoSignalMorphingMixer
    {
        double frac;

        MonoSignalMorphingMixer(double f) 
        {
            frac = f;
        }
        void setFrac(float f) {
            frac = f;
        }
        double Tick(double X, double Y)
        {
            return X + frac*(Y-X);
        }
    };

    struct StereoSignalMorphingMixer 
    {
        double frac;

        StereoSignalMorphingMixer(double f) 
        {
            frac = f;
        }
        void setFrac(float f) {
            frac = f;
        }
        double Tick(double X1, double X2, double Y1, double Y2, double &L, double &R)
        {
            L =  X1 + frac*(Y1-X1);
            R =  X2 + frac*(Y2-X2);
            return (L+R)*0.5;
        }
    };
}    