#pragma once 

#include<samplerate.h>

namespace SoundWave {

    enum ConverterType
    {
        SRC_SINC_BEST_QUALITY		= 0,
        SRC_SINC_MEDIUM_QUALITY		= 1,
        SRC_SINC_FASTEST			= 2,
        SRC_ZERO_ORDER_HOLD			= 3,
        SRC_LINEAR					= 4,
    } ;

    template<typename T>
    struct Resampler
    {
        SRC_STATE * src;
        T ratio;
        int error;

        Resampler(ConverterType type, int channels, T ratio)
        {
            int error = 0;
            src = src_new(type,channels,&error);
            assert(src != NULL);            
            this->ratio = ratio;
            error = 0;
        }
        ~Resampler() {
            if(src) src_delete(src);
        }

        const char* GetError() { 
            return src_strerror(error);
        }

        int Process(size_t n, std::vector<T> & input, std::vector<T> & output) {
            SRC_DATA data;
            memset(&data,0x00,sizeof(data));
            output.resize(ratio*n);
            data.data_in = input.data();            
            data.data_out = output.data();
            data.input_frames = input.size();
            data.output_frames = output.size();
            data.src_ratio = ratio;
            error = src_process(src,&data);
            if(error != 0) return -1;
            return data.output_frames_gen;
        }

        int Simple(size_t n, size_t channels, std::vector<T> & input, std::vector<T> & output) {
            SRC_DATA data;
            memset(&data,0x00,sizeof(data));
            output.resize(n*ratio);
            data.data_in = input.data();            
            data.data_out = output.data();
            data.input_frames = input.size();
            data.output_frames = output.size();
            data.src_ratio = ratio;
            error = src_simple(&data,0,channels);
            if(error != 0) return -1;
            return data.output_frames_gen;
        }
    };
}