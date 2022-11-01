#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Reverb
{
    struct AllPassReverb : public ATKFilter
    {
        ATK::AllPassReverbFilter<DspFloatType> * filter;

        AllPassReverb() : ATKFilter(size_t max, DspFloatType fbk) {
            filter = new ATK::AllPassReverbFilter<DspFloatType>(max);
            filter->set_feedback(fbk);
        }
        ~AllPassReverb() {
            if(filter) delete filter;
        }
        enum {
            PORT_DELAY,
            PORT_FEEDBACK
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAY: filter->set_delay(v); break;
                case PORT_FEEDBACK: filter->set_feedback(v); break;
            }
        }
    };
    struct LowPassReverb : public ATKFilter
    {
        ATK::LowPassReverbFilter<DspFloatType> * filter;

        LowPassReverb() : ATKFilter(size_t max, DspFloatType fbk, DspFloatType cutoff) {
            filter = new ATK::LowPassReverbFilter<DspFloatType>(max);
            filter->set_feedback(fbk);
            filter->set_cutoff(cutoff);
        }
        ~LowPassReverb() {
            if(filter) delete filter;
        }
        enum {
            PORT_DELAY,
            PORT_FEEDBACK,
            PORT_CUTOFF,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAY: filter->set_delay(v); break;
                case PORT_FEEDBACK: filter->set_feedback(v); break;
                case PORT_CUTOFF: filter->set_cutoff(v); break;
            }
        }
    };
    struct ConvolutionFilter : public ATKFilter
    {
        ATK::ConvolutionFilter<DspFloatType> * filter;

        ConvolutionFilter(const std::vector<DspFloatType> & ir, size_t split_size = 1) : ATKFilter() {
            filter = new ATK::ConvolutionFilter<DspFloatType>();
            ATK::AlignedScalarVector v(ir.size());
            memcpy(v.data(),ir.data(),sizeof(DspFloatType)*ir.size());
            filter->set_impulse(impulse);
            filter->set_split_size(1);
        }
        ~ConvolutionFilter() {
            if(filter) delete filter;
        }
    };
}