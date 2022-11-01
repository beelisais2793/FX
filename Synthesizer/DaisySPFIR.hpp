#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Filters
{
    struct FIR : public FilterProcessorPlugin<daisysp::FIRFilterImplGeneric>
    {
        FIR(const float * ir, size_t len, bool rev) : FilterProcessorPlugin<daisysp::FIRFilterImplGeneric>()
        {
            this->SetIR(ir,len,rev);
        }
        enum {
            PORT_RESET,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_RESET: this->Reset(); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            this->ProcessBlock(in,out,block);
        }
    };
}