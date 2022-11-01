#pragma once

/// Generalized Harmonic Analyzer 
/// Spectrogram STFT
// FIR Convolution - https://github.com/DoraSzasz/DigitalFilteringAlgorithms
// FFT EQ - https://github.com/kappalabs/fft_band_equalizer
// Convolution
// Kfr
// PffastConvolution
// ImpulseResponse.hpp
// FFTConvolver
// FFTW3 (not tested)
struct FFTFilterBank
{
    /// Gamma::FFT
    /// PFFFT from VCVRack        
    /// Kfr DFT    
    DSP::R2CF forward;
    DSP::C2RF inverse;
    audiofft::AudioFFT fft;
    DSP::Rectangle<float> window;
    std::vector<float> bins;
    FFTFilterBank(int size=BufferSize) :
        forward(size),
        inverse(size),
        window(size)
    {
        fft.init(size);
        bins.resize(size);
        float gain = 1.0;
        for(size_t i = 0; i < size; i++)
        {
            bins[i] = gain;
            // it's not linear
            gain = 0.5;
        }
        
    }
    void ProcessBuffer(size_t n, float * buffer)
    {        
        sample_vector<float> re(n),im(n),temp(n);
        memcpy(temp.data(),buffer,n*sizeof(float));
        for(size_t i = 0; i < n; i++)
            temp[i] *= window[i];
        fft.fft(temp.data(),re.data(),im.data());                        
        
        for(size_t i = 1; i < n/2; i++)
        {
            std::complex<float> c(bins[i],bins[i]);
            re[i] *= c.real();
            im[i]  *= c.imag();
            re[n-i] *= c.real();                              
            im[n-i] *= c.imag();
        }
        
        //memset(buffer,0,n*sizeof(float));
        fft.ifft(buffer,re.data(),im.data());
     
        /* FFTW at least passes it throguh
        sample_vector<float> temp(n);
        memcpy(temp.data(),buffer,n*sizeof(float));
        forward.set_input(temp);
        forward.Execute();
        complex_vector<float> out = forward.get_output();        
        
        for(size_t i = 0; i < n; i++)
        {
            out[i] /= n;
        }
        
        inverse.set_input(out);
        inverse.Execute();
        temp = inverse.get_output();
        memcpy(buffer,temp.data(),n*sizeof(float));
        */        
    }
};