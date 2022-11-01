#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct Drip : public GeneratorProcessorPlugin<daisysp::Drip>
    {
        Drip(float deattack) : GeneratorProcessorPlugin<daisysp::Drip>()
        {
            this->Init(sampleRate,deattack);
        }
        double Tick(double I=0, double A=1, double X=1, double Y=0) {
            return A*this->Process((bool)I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}