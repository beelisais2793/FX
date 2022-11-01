// FFTProcessor
// perform fft
// multiply by vector
// bin[i] = function(bin[i])
// bin[i] = luafunction(bin[i])
// bins   = function(bins)
// bins   = luafunction(bins)
// preform ifft

// Gamma = dft dct cfft rfft stft sdft
// VCVFFT/PFFFT
// audiofft
// kfr dft dct
// fftw3 c2r c2c r2c r2r
// fftwpp

struct FFTProcessor
{
    /// Gamma::FFT
    /// PFFFT from VCVRack        
    /// Kfr DFT    
    //  audiofft::AudioFFT fft;

    DSP::R2CF forward;
    DSP::C2RF inverse;
    DSP::Rectangle<float> window;
    float max;
    int   max_bin;
    // Convolution
    // Deconvolution
    // Correlation
    // AutoCorrelation
    // Resampler
    // FIR Filter 
    // Equalizer
    
    
    std::vector<float> bins;
    std::vector<float> mags;
    std::vector<float> phases;

    FFTProcessor(int size=BufferSize) :
        forward(size),
        inverse(size),
        window(size)
    {
        //fft.init(size);
        bins.resize(size);
        mags.resize(size);
        phases.resize(size);
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
        /*       
        audiofft
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
        */

        // FFTW
        sample_vector<float> temp(n);
        memcpy(temp.data(),buffer,n*sizeof(float));
        for(size_t i = 0; i < n; i++) temp[i] *= window[i];
        forward.set_input(temp);
        forward.Execute();
        DSP::complex_vector<float> out = forward.get_output();        

        max = -1e120;    
        max_bin = 0;
        for(size_t i = 0; i < n; i++)
        {
            out[i] /= n;
        }
        
        for(size_t i = 0; i < n/2; i++)
        {
            mags[i] = std::abs(out[i]);
            if(mags[i] > max) { max = mags[i]; max_bin = i; }
            phases[i] = std::arg(out[i]);
        }
        out[0] = 0.0f;
        out[n/2-1]=0.0f;
        for(size_t i = 0; i < n/2; i++)
        {
            //if((i < (max_bin-8)) && (i > (max_bin+8)))
            out[i] *= std::complex<float>(0.05,-0.0001);                                                
            out[(n-1)-i] = 0;
        }
        
        inverse.set_input(out);
        inverse.Execute();
        temp = inverse.get_output();
        memcpy(buffer,temp.data(),n*sizeof(float));   
        
    }
};