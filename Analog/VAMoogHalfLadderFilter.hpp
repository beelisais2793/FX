
//------------------------------------------------------------------------------
// This file was generated using the Faust compiler (https://faust.grame.fr),
// and the Faust post-processor (https://github.com/jpcima/faustpp).
//
// Source: mooghalfladder.dsp
// Name: MoogHalfLadder
// Author: Eric Tarr
// Copyright: 
// License: MIT-style STK-4.3 license
// Version: 
//------------------------------------------------------------------------------

#pragma once

#include <memory>

namespace Filters::Moog
{
    class MoogHalfLadder {
    public:
        MoogHalfLadder();
        ~MoogHalfLadder();

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
        
        enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,			
        };
        void setPort(int port, double v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                set_cutoff(v);
                break;
            case PORT_RESONANCE:
                set_q(v);
                break;		
            }
        }
        void Process(size_t n, float * input, float * output) {
            process(input,output,n);
        }
        void Process(float * input, size_t n ) {
            process(input,input,n);
        }
        float Tick(float input) {
            float r = 0.0;
            Process(1,&input,&r);
            return r;	
        }
    public:
        class BasicDsp;

    private:
        std::unique_ptr<BasicDsp> fDsp;



    };
}

#include "VAMoogHalfLadderFilter.cpp"