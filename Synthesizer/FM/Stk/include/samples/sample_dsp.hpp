#pragma once 

#include "sample.hpp"

#include "AudioFFT/AudioFFT.h"
#include "Kfr/kfrcore.hpp"
#include "AudioFFT/FFTConvolver.h"


namespace SoundAlchemy
{
    template<typename T>
    std::vector<std::complex<T>> dft_forward(std::vector<std::complex<T>> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_dft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<std::complex<T>> dft_inverse(std::vector<std::complex<T>> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_idft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<std::complex<T>> rdft_forward(std::vector<T> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_realdft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<T> rdft_inverse(std::vector<std::complex<T>> & in)
    {
        kfr::univector<T> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_irealdft(input);
        std::vector<std::complex<T>> r(output.size());
        std::copy(output.begin(),output.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> dct_forward(std::vector<T> in)
    {        
        DSP::DCTPlan<T> dct(in.size());
        kfr::univector<T> out(in.size());
        dct.execute(out,in,false);
        std::vector<T> r(out.size());
        memcpy(r.data(),out.data(),out.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> dct_inverse(std::vector<T> in)
    {        
        DSP::DCTPlan<T> dct(in.size());
        kfr::univector<T> out(in.size());
        dct.execute(out,in,true);
        std::vector<T> r(out.size());
        memcpy(r.data(),out.data(),out.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> convolve(std::vector<T> &a, std::vector<T> & b)
    {        
        kfr::univector<T> A(a.size());
        kfr::univector<T> B(b.size());        
        kfr::univector<T> R = kfr::convolve(A,B);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> correlate(std::vector<T> &a, std::vector<T> & b)
    {        
        kfr::univector<T> A(a.size());
        kfr::univector<T> B(b.size());        
        kfr::univector<T> R = kfr::correlate(A,B);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> autocorrelate(std::vector<T> &a)
    {        
        kfr::univector<T> A(a.size());        
        kfr::univector<T> R = kfr::autocorrelate(A);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> convolve_filter(size_t block_size,std::vector<T> & impulse, std::vector<T> &x)
    {        
        kfr::univector<T> imp(impulse.size());
        std::copy(impulse.begin(),impulse.end(),imp.begin());
        DSP::ConvolveFilter<T> filter(imp,block_size);
        kfr::univector<T> in(x.size());
        std::copy(x.begin(),x.end(),in.begin());
        filter.apply(in);
        std::vector<T> r(in.size());
        std::copy(in.begin(),in.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> dcremove(T cutoff, std::vector<T> & in)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> r = DSP::dcremove(i,cutoff);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }

    template<typename T>
    std::vector<T> sinewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::sinewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> squarewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::squarewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> trianglewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::trianglewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> sawtoothewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::squarewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }    

    template<typename T>
    std::vector<T> load_wav(const char * filename)
    {
        kfr::univector<T> out = DSP::load_wav<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> load_mp3(const char * filename)
    {
        kfr::univector<T> out = DSP::load_mp3<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> load_flac(const char * filename)
    {
        kfr::univector<T> out = DSP::load_flac<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    void save_wav(std::vector<T> & v, const char * filename, size_t channels, int sample_type, double sample_rate, bool use_w64=false)
    {
        kfr::univector<T> in(v.size());
        std::copy(v.begin(),v.end(),in.begin());
        DSP::write_wav(in,filename,channels,sample_type,sample_rate,use_w64);
        
    }

    
    template<typename T> 
    std::vector<T> bessel_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselLowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselHighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselBandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselBandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }

    template<typename T> 
    std::vector<T> butterworth_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthLowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthHighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthBandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthBandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    
    template<typename T> 
    std::vector<T> chebyshev1_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1LowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1HighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1BandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1BandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }

    template<typename T> 
    std::vector<T> chebyshev2_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2LowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2HighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2BandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2BandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    

    
    template<typename T>
    std::vector<T> lowpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::lowpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::lowpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::lowpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::lowpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::highpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::highpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::highpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::highpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::bandpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::bandpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::bandpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::bandpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> notch1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::notchfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::notch_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::notch_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::notch_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    template<typename T>
    std::vector<T> allpass1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::allpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::allpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> peak1_filter(std::vector<T> & in, T freq, T q, T gain)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::peakfilter(i,freq,q,gain);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak2_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::peak_params(freq,T(0.54119610),gain);
        DSP::BiQuadParams<T> f2 = DSP::peak_params(freq,T(1.3065460),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak3_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.51763809),gain);
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.70710678),gain);
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(1.9318517),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak4_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::peak_params(freq,T(0.50979558),gain);
        DSP::BiQuadParams<T> f2 = DSP::peak_params(freq,T(0.60134489),gain);
        DSP::BiQuadParams<T> f3 = DSP::peak_params(freq,T(0.89997622),gain);
        DSP::BiQuadParams<T> f4 = DSP::peak_params(freq,T(2.5629154),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::lowshelffilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::lowshelf_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::lowshelf_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::lowshelf_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

template<typename T>
    std::vector<T> highshelf1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::highshelffilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::highshelf_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::highshelf_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::highshelf_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    enum class QualityType : int
    {
        draft   = 4,
        low     = 6,
        normal  = 8,
        high    = 10,
        perfect = 12,
    };
    template<typename T> 
    std::vector<T> resample(QualityType quality,std::vector<T> &in, T input_rate, T output_rate)
    {
        kfr::univector<T> input(in.size());
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = DSP::resample(input,(kfr::sample_rate_conversion_quality)quality,output_rate,input_rate);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
   
    enum WindowType 
    {
        rectangular     = 1,
        triangular      = 2,
        bartlett        = 3,
        cosine          = 4,
        hann            = 5,
        bartlett_hann   = 6,
        hamming         = 7,
        bohman          = 8,
        blackman        = 9,
        blackman_harris = 10,
        kaiser          = 11,
        flattop         = 12,
        gaussian        = 13,
        lanczos         = 14,
    };
 
    template<typename T>
    kfr::expression_pointer<T> make_window(WindowType type, size_t s, const T x = -1)
    {
        switch(type)
        {
        case hann: return DSP::make_window_hann_ptr<T>(s);
        case hamming: return DSP::make_window_hamming_ptr<T>(s);
        case blackman: return DSP::make_window_blackman_ptr<T>(s);
        case blackman_harris: return DSP::make_window_blackman_ptr<T>(s);
        case gaussian: return DSP::make_window_gaussian_ptr<T>(s);
        case triangular: return DSP::make_window_triangular_ptr<T>(s);
        case bartlett: return DSP::make_window_bartlett_ptr<T>(s);
        case cosine: return DSP::make_window_cosine_ptr<T>(s);
        case bartlett_hann: return DSP::make_window_bartlett_hann_ptr<T>(s);
        case bohman: return DSP::make_window_bohman_ptr<T>(s);
        case lanczos: return DSP::make_window_lanczos_ptr<T>(s);
        case flattop: return DSP::make_window_flattop_ptr<T>(s);
        case rectangular: return DSP::make_window_rectangular_ptr<T>(s);
        case kaiser: return DSP::make_window_kaiser_ptr<T>(s);
        }
    }
    template<typename T>
    std::vector<T> fir_lowpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_lowpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_highpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_highpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_bandpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_bandpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_bandstop(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_bandstop(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    std::vector<std::complex<float>> audio_fft_forward(std::vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        std::vector<float> temp(s);
        std::vector<float> real(s);
        std::vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        memcpy(temp.data(),in.data(),in.size()*sizeof(float));
        fft.init(in.size());
        fft.fft(temp.data(),real.data(),imag.data());
        std::vector<std::complex<float>> out(s);
        for(size_t i = 0; i < s; i++)
        {
            out[i].real(real[i]);
            out[i].imag(imag[i]);
        }
        return out;
    }

    std::vector<float> audio_fft_inverse(std::vector<std::complex<float>> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        std::vector<float> temp(s);
        std::vector<float> real(s);
        std::vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        for(size_t i = 0; i < in.size(); i++)
        {
            real[i] = in[i].real();
            imag[i] = in[i].imag();
        }
        fft.init(in.size());
        fft.ifft(temp.data(),real.data(),imag.data());        
        return temp;
    }

    std::vector<float> audio_convolve(std::vector<float>& in, size_t block_size, std::vector<float> & ir)
    {
        std::vector<float> out(in.size());
        fftconvolver::FFTConvolver conv;
        conv.init(block_size,ir.data(),ir.size());
        conv.process(in.data(),out.data(),in.size());
        return out;
    }
    template<typename T>
    // r = frac
    // x = [i]
    // y = [i+1]
    T linear_interpolate(T x, T y, T r)
    {        
        return r +*x (1.0-r)*y;
        
    }
    template<typename T>
    T cubic_interpolate(T finpos, T xm1, T x0, T x1, T x2)
    {
        //T xm1 = x [inpos - 1];
        //T x0  = x [inpos + 0];
        //T x1  = x [inpos + 1];
        //T x2  = x [inpos + 2];
        T a = (3 * (x0-x1) - xm1 + x2) / 2;
        T b = 2*x1 + xm1 - (5*x0 + x2) / 2;
        T c = (x1 - xm1) / 2;
        return (((a * finpos) + b) * finpos + c) * finpos + x0;
    }
    // original
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite1(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite2(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        T c2 = y0 - y1 + c1 - c3;
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite3(T x, T y0, T y1, T y2, T y3)
    {
            // 4-point, 3rd-order Hermite (x-form)
            T c0 = y1;
            T c1 = 0.5f * (y2 - y0);
            T y0my1 = y0 - y1;
            T c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
            T c2 = y0my1 + c1 - c3;

            return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // laurent de soras
    template<typename T>
    // x[i-1]
    // x[i]
    // x[i+1]
    // x[i+2]    
    inline T hermite4(T frac_pos, T xm1, T x0, T x1, T x2)
    {
        const T    c     = (x1 - xm1) * 0.5f;
        const T    v     = x0 - x1;
        const T    w     = c + v;
        const T    a     = w + v + (x2 - x0) * 0.5f;
        const T    b_neg = w + a;

        return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
    }

    /// Sndfile   
    /// Resampler
    /// SpeexResampler

    template<typename T>
    std::vector<T> upsample2x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()*2);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*2] = in[i];
        return out;
    }
    template<typename T>
    std::vector<T> upsample4x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()*4);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*4] = in[i];
        return out;
    }
    template<typename T>
    std::vector<T> downsample2x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()/2);
        zeros(out);
        for(size_t i = 0; i < in.size()/2; i++)
            out[i] = in[i*2];
        return out;
    }
    template<typename T>
    std::vector<T> downsample4x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()/4);
        zeros(out);
        for(size_t i = 0; i < in.size()/4; i++)
            out[i] = in[i*4];
        return out;
    }

    
}