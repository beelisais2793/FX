
//------------------------------------------------------------------------------
// This file was generated using the Faust compiler (https://faust.grame.fr),
// and the Faust post-processor (https://github.com/jpcima/faustpp).
//
// Source: korg35hpf.dsp
// Name: Korg35HPF
// Author: Eric Tarr
// Copyright: 
// License: MIT-style STK-4.3 license
// Version: 
//------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <utility>
#include <cmath>
#include "SoundObject.hpp"

namespace Filters::Korg35
{
    class Korg35HPF : public FilterProcessor {
    public:
        Korg35HPF();
        ~Korg35HPF();

        void init(DspFloatType sample_rate);
        void clear() noexcept;

        void process(
            const DspFloatType *in0,
            DspFloatType *out0,
            unsigned count) noexcept;

        enum { NumInputs = 1 };
        enum { NumOutputs = 1 };
        enum { NumActives = 2 };
        enum { NumPassives = 0 };
        enum { NumParameters = 2 };

        enum Parameter {
            p_cutoff,
            p_q,
            
        };

        struct ParameterRange {
            DspFloatType init;
            DspFloatType min;
            DspFloatType max;
        };

    /*
        static const char *parameter_label(unsigned index) noexcept;
        static const char *parameter_short_label(unsigned index) noexcept;
        static const char *parameter_symbol(unsigned index) noexcept;
        static const char *parameter_unit(unsigned index) noexcept;
        static const ParameterRange *parameter_range(unsigned index) noexcept;
        static bool parameter_is_trigger(unsigned index) noexcept;
        static bool parameter_is_boolean(unsigned index) noexcept;
        static bool parameter_is_integer(unsigned index) noexcept;
        static bool parameter_is_logarithmic(unsigned index) noexcept;
    
        DspFloatType get_parameter(unsigned index) const noexcept;
        void set_parameter(unsigned index, DspFloatType value) noexcept;
    */    
        DspFloatType get_cutoff() const noexcept;        
        DspFloatType get_q() const noexcept;
                
        void set_cutoff(DspFloatType value) noexcept;        
        void set_q(DspFloatType value) noexcept;
        
        enum {
            PORT_CUTOFF,
            PORT_RESONANCE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: set_cutoff(v); break;
                case PORT_RESONANCE: set_q(v); break;
                default: printf("No port %d\n",port);
            }
        }
        DspFloatType Tick(DspFloatType input, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            DspFloatType r = input;
            DspFloatType c = get_cutoff();
            DspFloatType q = get_q();
            set_cutoff(c * fabs(X));
            set_q(q * fabs(Y));
            process(&input,&r,1);
            set_cutoff(c);
            set_q(q);
            return A*r;
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            process(in,out,n);
        }
        void InplaceProcess(size_t n, DspFloatType * samples) {
            for(size_t i = 0; i < n; i++)
                samples[i] = Tick(samples[i]);
        }

    public:
        class BasicDsp;

    private:
        std::unique_ptr<BasicDsp> fDsp;
    };
    #include "VAKorg35HPFFilter.hpp"
}
