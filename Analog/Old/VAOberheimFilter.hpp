
//------------------------------------------------------------------------------
// This file was generated using the Faust compiler (https://faust.grame.fr),
// and the Faust post-processor (https://github.com/jpcima/faustpp).
//
// Source: oberheim.dsp
// Name: Oberheim
// Author: Christopher Arndt
// Copyright: 
// License: MIT-style STK-4.3 license
// Version: 
//------------------------------------------------------------------------------

#pragma once

#include <memory>

namespace Analog::Filters::Oberheim
{
    class Oberheim {
    public:
        Oberheim();
        ~Oberheim();

        void init(float sample_rate);
        void clear() noexcept;

        void process(
            const float *in0,
            float *out0,float *out1,float *out2,float *out3,
            unsigned count) noexcept;

        enum { NumInputs = 1 };
        enum { NumOutputs = 4 };
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
        
        enum filterOut {
            OUT_1,
            OUT_2,
            OUT_3,
            OUT_4,
        } filterout = OUT_1;

        float Tick(float input) {
            float out1,out2,out3,out4;
            process(&input,&out1,&out2,&out3,&out4,1);
            switch(filterout) {
                case OUT_1: return out1;
                case OUT_2: return out2;
                case OUT_3: return out3;
                case OUT_4: return out4;
            }
        }
        void Process(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void Process(float * samples, size_t n ) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick(samples[i]);
        }

    public:
        class BasicDsp;

    private:
        std::unique_ptr<BasicDsp> fDsp;



    };
}

#include "VAOberheimFilter.cpp"