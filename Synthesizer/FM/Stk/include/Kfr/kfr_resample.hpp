#pragma once 

namespace DSP
{
    template<typename T>
    kfr::univector<T> resample(kfr::sample_rate_conversion_quality quality, const kfr::univector<T> & input, size_t output_sr, size_t input_sr) {    
        auto r = kfr::resampler<T>(quality,output_sr, input_sr);    
        kfr::univector<T> output(input.size() * output_sr/input_sr + r.get_delay());
        r.process(output,input);
        return output;    
    }

    template<typename T>
    kfr::univector<T> convert_sample(const kfr::univector<T> & input) {
        kfr::univector<T> output(input.size());
        kfr::convert(output.data(),input.data(),input.size());
        return output;
    }

}