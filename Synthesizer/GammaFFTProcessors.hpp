#pragma once

namespace Gamma::FFT
{
    struct CFFTPadProcessor
    {
        gam::CFFT<float> fft;
        std::vector<std::complex<float>> temp;
        size_t block_size;

        CFFTPadProcessor(size_t blocks, size_t zeros=1) : fft(blocks*zeros)
        {
            temp.resize(blocks*2*zeros);
            memset((float*)temp.data(),0,2*blocks*zeros*sizeof(float)*2);
            block_size = blocks;
        }
        void forward(std::vector<std::complex<float>> & input,std::vector<std::complex<float>> & output, bool normalize=true, float nrmGain=1.0f )
        {
            temp = input;
            for(size_t i = input.size(); i < temp.size(); i++) temp[i] = std::complex<float>(0,0);
            fft.forward(static_cast<float*>(temp.data()),normalize,nrmGain);
            for(size_t i = 0; i < block_size; i++) output[i] = temp[i];
        }
        void inverse(std::vector<std::complex<float>> & input,std::vector<std::complex<float>> & output)
        {
            temp = input;
            for(size_t i = input.size(); i < temp.size(); i++) temp[i] = std::complex<float>(0,0);
            fft.inverse(static_cast<float*>(temp.data()));
            for(size_t i = 0; i < block_size; i++) output[i] = temp[i];
        }
    };

    struct RFFTPadProcessor
    {
        gam::RFFT<float> fft;
        std::vector<float> temp;
        std::vector<std::complex<float>> tempc;
        size_t block_size;

        RFFTPadProcessor(size_t blocks, size_t zeros=1) : fft(blocks*zeros)
        {
            temp.resize(blocks*2*zeros);
            memset((float*)temp.data(),0,2*blocks*zeros*sizeof(float));
            block_size = blocks;
        }
        void forward(std::vector<float> & input,std::vector<std::complex<float>> & output, bool normalize=true, float nrmGain=1.0f )
        {
            temp = input;
            for(size_t i = input.size(); i < temp.size(); i++) temp[i] = std::complex<float>(0,0);
            fft.forward(static_cast<float*>(temp.data()),normalize,nrmGain);
            for(size_t i = 0; i < block_size; i++) {
                output[i].real(temp[2*i]);
                output[i].imag(temp[2*i+1]);
            }
        }
        void inverse(std::vector<std::complex<float>> & input,std::vector<float> & output)
        {
            tempc = input;
            for(size_t i = input.size(); i < tempc.size(); i++) tempc[i] = std::complex<float>(0,0);
            fft.inverse(static_cast<float*>(tempc.data()));
            memcpy(output,(float*)tempc.data(),block_size*sizeof(float));            
        }
    };
}