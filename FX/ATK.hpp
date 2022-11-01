
/////////////////////////////////////////////////////////////////////////////////
// AudioTK
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ATK/Core/TypedBaseFilter.h>

#include <ATK/Adaptive/BlockLMSFilter.h>
#include <ATK/Adaptive/LMSFilter.h>
#include <ATK/Adaptive/RLSFilter.h>

#include <ATK/Delay/FeedbackDelayNetworkFilter.h>
#include <ATK/Delay/FixedDelayLineFilter.h>
#include <ATK/Delay/HadamardMixture.h>
#include <ATK/Delay/HouseholderMixture.h>
#include <ATK/Delay/MultipleUniversalFixedDelayLineFilter.h>
#include <ATK/Delay/UniversalFixedDelayLineFilter.h>
#include <ATK/Delay/UniversalVariableDelayLineFilter.h>
#include <ATK/Delay/VariableDelayLineFilter.h>

#include <ATK/Distortion/DiodeClipperFilter.h>
#include <ATK/Distortion/HalfTanhShaperFilter.h>
#include <ATK/Distortion/SD1OverdriveFilter.h>
#include <ATK/Distortion/SimpleOverdriveFilter.h>
#include <ATK/Distortion/TS9OverdriveFilter.h>
#include <ATK/Distortion/TanhShaperFilter.h>
//#include <ATK/Distortion/WaveshaperFilter.h>

#include <ATK/EQ/ButterworthFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/ToneStackFilter.h>

#include <ATK/Preamplifier/TriodeFilter.h>
#include <ATK/Preamplifier/Triode2Filter.h>

#include <ATK/Special/ConvolutionFilter.h>

#include <ATK/Tools/DecimationFilter.h>
#include <ATK/Tools/OversamplingFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

namespace Filters::AudioTK
{

    template <typename DataType>
    class ATKInputFilter : public ATK::TypedBaseFilter<DataType> {
    public:
        using ATK::TypedBaseFilter<DataType>::outputs;

        explicit ATKInputFilter(int channels) : ATK::TypedBaseFilter<DataType>(0, channels), mChannels(channels) {}

        void set_inputs(DataType ** inputs, int size) {
            mInputs = inputs;
            mSize = size;
        }

    protected:
        DataType** mInputs = nullptr;
        int mSize = 0;
        int mChannels = 1;

        virtual void process_impl(int64_t size) const {
            for (int c = 0; c < mChannels; ++c) {
                for (int64_t i = 0; i < size; ++i) {
                    outputs[c][i] = mInputs[c][i];
                }
            }
        }
    };

    template <typename DataType>
    class ATKOutputFilter : public ATK::TypedBaseFilter<DataType> {
    public:
        using ATK::TypedBaseFilter<DataType>::converted_inputs;

        explicit ATKOutputFilter(int channels) : ATK::TypedBaseFilter<DataType>(channels, 0), mChannels(channels) {}

        void set_outputs(DataType** outputs, int size) {
            mOutputs = outputs;
            mSize = size;
        }

    protected:
        DataType** mOutputs = nullptr;
        int mSize = 0;
        int mChannels = 1;

        virtual void process_impl(int64_t size) const {
            for (int c = 0; c < mChannels; ++c) {
                for (int64_t i = 0; i < size; ++i) {
                    mOutputs[c][i] = converted_inputs[c][i];
                }
            }
        }
    };

    
    using InType = ATKInputFilter<DspFloatType>;
    using OutType = ATKOutputFilter<DspFloatType>;
    using BaseFilterType = ATK::TypedBaseFilter<DspFloatType>;
    using SumType = ATK::SumFilter<DspFloatType>;
    using VolumeType = ATK::VolumeFilter<DspFloatType>;
    using OversamplingType_2 = ATK::OversamplingFilter<DspFloatType, ATK::Oversampling6points5order_2<DspFloatType>>;
    using OversamplingType_4 = ATK::OversamplingFilter<DspFloatType, ATK::Oversampling6points5order_4<DspFloatType>>;
    using OversamplingType_8 = ATK::OversamplingFilter<DspFloatType, ATK::Oversampling6points5order_8<DspFloatType>>;
    using DownsamplingType = ATK::DecimationFilter<DspFloatType>;
    using LowPassType = ATK::IIRFilter<ATK::ButterworthLowPassCoefficients<DspFloatType>>;
    using ToneStackType = ATK::IIRFilter<ATK::ToneStackCoefficients<DspFloatType>>;
    using CabType = ATK::ConvolutionFilter<DspFloatType>;
    
    // process a list/vector of filters
    struct ATKMultiFilterProcessor : public StereoFXProcessor
    {
        std::unique_ptr<InType> mInputs;
        std::unique_ptr<OutType> mOutputs;
        std::vector<ATK::BaseFilter*> filters;
        std::unique_ptr<OversamplingType_2> mOversample;
        std::unique_ptr<DownsamplingType> mDecimator;

        ATKMultiFilterProcessor() : StereoFXProcessor()
        {
            mInputs.reset(new InType(1));
            mInputs->set_output_sampling_rate(sampleRate);

            mOutputs.reset(new OutType(1));
            mOutputs->set_input_sampling_rate(sampleRate);        

            mOversample.reset(new OversamplingType_2(1));
            mOversample->set_input_sampling_rate(sampleRate);
            mOversample->set_output_sampling_rate(2*sampleRate);

            mDecimator.reset(new DownsamplingType);
            mDecimator->set_input_sampling_rate(2*sampleRate);
            mDecimator->set_output_sampling_rate(sampleRate);

            
            filters.push_back(mOversample.get());
        }
        ~ATKMultiFilterProcessor() {

        }

        void addFilter(ATK::BaseFilter * p) {
            
        }
        void connect() 
        {
            
            filters.push_back(mDecimator.get());
            

            auto p = filters.begin();
            auto last = p;
            (*p)->set_input_port(0, mInputs.get(), 0);
            (*p)->set_input_port(1, mInputs.get(), 1);
            p++;

            while(p != filters.end())
            {
                (*p)->set_input_port(0,(*last),0);
                (*p)->set_input_port(1,(*last),1);
                last = p++;
            }
            mOutputs->set_input_port(0,(*last),0);
            mOutputs->set_input_port(1,(*last),1);
        }
        void Run(size_t nFrames, DspFloatType ** inputs, DspFloatType ** outputs)
        {        
            mInputs->set_inputs(inputs, nFrames);
            mOutputs->set_outputs(outputs, nFrames);
            mOutputs->process(nFrames);
        }
    };

    // process a filter
    struct ATKFilterProcessor : public StereoFXProcessor
    {
        std::unique_ptr<InType> mInputs;
        std::unique_ptr<OutType> mOutputs;
        ATK::BaseFilter * filter;
        std::unique_ptr<OversamplingType_2> mOversample;
        std::unique_ptr<DownsamplingType> mDecimator;

        ATKFilterProcessor(ATK::BaseFilter* f) : StereoFXProcessor()
        {
            mInputs.reset(new InType(1));
            mInputs->set_output_sampling_rate(sampleRate);

            mOutputs.reset(new OutType(1));
            mOutputs->set_input_sampling_rate(sampleRate);        

            mOversample.reset(new OversamplingType_2(1));
            mOversample->set_input_sampling_rate(sampleRate);
            mOversample->set_output_sampling_rate(2*sampleRate);

            mDecimator.reset(new DownsamplingType);
            mDecimator->set_input_sampling_rate(2*sampleRate);
            mDecimator->set_output_sampling_rate(sampleRate);
            
            filter = f;
            connect();
        }
        ~ATKFilterProcessor() {

        }

        void setFilter(ATK::BaseFilter * p)
        {
            filter = p;
        }

        void connect() 
        {   
            mOversample->set_input_port(0, mInputs.get(), 0);
            mOversample->set_input_port(1, mInputs.get(), 1);

            filter->set_input_port(0, mOversample.get(), 0);
            filter->set_input_port(1, mOversample.get(), 1);

            mDecimator->set_input_port(0, filter, 0);
            mDecimator->set_input_port(1, filter, 1);
                        
                
            mOutputs->set_input_port(0,mDecimator.get(),0);
            mOutputs->set_input_port(1,mDecimator.get(),1);
        }
        void ProcessBlock(size_t nFrames, DspFloatType ** inputs, DspFloatType ** outputs)
        {                    
            mInputs->set_inputs(inputs, nFrames);
            mOutputs->set_outputs(outputs, nFrames);
            mOutputs->process(nFrames);
        }
    };
    
    struct ATKFilter : public StereoFXProcessor
    {
        ATKFilterProcessor * p;
        ATKFilter() : StereoFXProcessor()
        {
            p = NULL;
        }
        ~ATKFilter() {
            if(p) delete p;
        }
        void setFilter(ATK::BaseFilter * f) {
            if(p) delete p;
            p = new ATKFilterProcessor(f);
        }
        void ProcessBlock(size_t nFrames, DspFloatType ** inputs, DspFloatType ** outputs)
        {        
            if(p != NULL)
                p->ProcessBlock(nFrames,inputs,outputs);
        }
    };
   
}