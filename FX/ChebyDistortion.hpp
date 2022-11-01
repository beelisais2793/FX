#pragma once

#include "SoundObject.hpp"

namespace FX::Distortion::Chebyshev
{
    template<int N>
    struct ChebyDistortion : public MonoFXProcessor
    {
        DspFloatType A[N];

        ChebyDistortion() : MonoFXProcessor() {
            randomize();
        }
        void randomize()
        {
            for(size_t i = 0; i < N; i++)
                A[i] = noise.rand();
        }
        DspFloatType Tick(DspFloatType I, DspFloatType G=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType out = I;
            for(size_t i = 0; i < N; i++)
            {
                out += X*A[i]*Distortion::cheby(i,Y*I);
            }
            return G*(out/(DspFloatType)N);
        }
        DspFloatType Tick(DspFloatType I, std::initializer_list<DspFloatType> index)
        {
            DspFloatType out = I;
            std::vector<DspFloatType> lfo(N);
            std::copy(index.begin(),index.end(),lfo.begin());
            for(size_t i = 0; i < N; i++)
            {
                out += (lfo[i]*A[i])*Distortion::cheby(i,I);
            }
            return out/(DspFloatType)N;
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out)
        {
            for(size_t i=0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}