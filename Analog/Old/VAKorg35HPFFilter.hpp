
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

namespace Filters::Korg35
{
    class Korg35HPF {
    public:
        Korg35HPF();
        ~Korg35HPF();

        void init(float sample_rate);
        void clear() noexcept;

        void process(
            const float *in0,
            float *out0,
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
            float init;
            float min;
            float max;
        };

        static const char *parameter_label(unsigned index) noexcept;
        static const char *parameter_short_label(unsigned index) noexcept;
        static const char *parameter_symbol(unsigned index) noexcept;
        static const char *parameter_unit(unsigned index) noexcept;
        static const ParameterRange *parameter_range(unsigned index) noexcept;
        static bool parameter_is_trigger(unsigned index) noexcept;
        static bool parameter_is_boolean(unsigned index) noexcept;
        static bool parameter_is_integer(unsigned index) noexcept;
        static bool parameter_is_logarithmic(unsigned index) noexcept;

        float get_parameter(unsigned index) const noexcept;
        void set_parameter(unsigned index, float value) noexcept;

        
        float get_cutoff() const noexcept;
        
        float get_q() const noexcept;
        
        
        void set_cutoff(float value) noexcept;
        
        void set_q(float value) noexcept;
        
        float Tick(float input) {
            float r = input;
            process(&input,&r,1);
            return r;
        }
        void Process(size_t n, float * in, float * out) {
            process(in,out,n);
        }
        void Process(float * samples, size_t n) {
            for(size_t i = 0; i < n; i++)
                samples[i] = Tick(samples[i]);
        }

    public:
        class BasicDsp;

    private:
        std::unique_ptr<BasicDsp> fDsp;
    };
}
#include "VAKorg35HPFFilter.hpp"