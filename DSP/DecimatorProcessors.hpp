#pragma once

namespace DSP::Decimators
{
    struct Downsampler2x5
    {
        Decimator5 dec;

        void ProcessBlock(size_t n, float * in, float * out)
        {
            memset(out,0,n*sizeof(float));
            for(size_t i = 0; i < n; i+=2)
            {
                out[i] = dec.Calc(in[i],in[i+1]);
            }
        }
    };
    struct Downsampler2x7
    {
        Decimator7 dec;

        void ProcessBlock(size_t n, float * in, float * out)
        {
            memset(out,0,n*sizeof(float));
            for(size_t i = 0; i < n; i+=2)
            {
                out[i] = dec.Calc(in[i],in[i+1]);
            }
        }
    };
    struct Downsampler2x9
    {
        Decimator9 dec;

        void ProcessBlock(size_t n, float * in, float * out)
        {
            memset(out,0,n*sizeof(float));
            for(size_t i = 0; i < n; i+=2)
            {
                out[i] = dec.Calc(in[i],in[i+1]);
            }
        }
    };
}