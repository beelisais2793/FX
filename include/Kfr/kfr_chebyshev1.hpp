#pragma once 

namespace DSP 
{
    template<typename T>
    struct Chebyshev1Filter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        T rp;
        static constexpr size_t maxorder=32;

        Chebyshev1Filter() = default;
        Chebyshev1Filter(int _order, T _rp) : order(_order), rp(_rp) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::chebyshev1<T>(order,rp),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::chebyshev1<T>(order,rp),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev1<T>(order,rp),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev1<T>(order,rp),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(kfr::univector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const kfr::univector<T> &in,kfr::univector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    template<typename T>
    struct Chebyshev1LowPassFilter {
        Chebyshev1Filter<T> filter;
        float Fc,Fs;
        
        Chebyshev1LowPassFilter(int order, T cutoff, T sample_rate, T rp = 2) : filter(order,rp) {
            Fc = cutoff;
            Fs = sample_rate;            
            filter.lowpass(Fc,Fs);
        }
        /*
        void setCutoff(T cutoff)
        {
            Fc = cutoff;       
            filter.lowpass(Fc,Fs);
        }
        */
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev1HighPassFilter {
        Chebyshev1Filter<T> filter;
        float Fc,Fs;

        Chebyshev1HighPassFilter(int order, T cutoff, T sample_rate, T rp = 2) : filter(order,rp) {
            Fc = cutoff;
            Fs = sample_rate;
            filter.highpass(cutoff,sample_rate);
        }
        /*
        void setCutoff(T cutoff)
        {
            Fc = cutoff;       
            filter.highpass(Fc,Fs);
        }
        */
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandPassFilter {
        Chebyshev1Filter<T> filter;
        float LoFc,HiFc,Fs;

        Chebyshev1BandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate,T rp = 2) : filter(order,rp) {
            LoFc = lo_cutoff;
            HiFc = hi_cutoff;
            Fs = sample_rate;
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        /*
        void setCutoff(T lo_cutoff, T hi_cutoff)
        {
            LoFc = lo_cutoff;
            HiFc = hi_cutoff;
            filter.bandpass(lo_cutoff,hi_cutoff,Fs);
        }
        */
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandStopFilter 
    {
        Chebyshev1Filter<T> filter;
        float LoFc,HiFc,Fs;

        Chebyshev1BandStopFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate,T rp = 2) : filter(order,rp) {
            LoFc = lo_cutoff;
            HiFc = hi_cutoff;
            Fs = sample_rate;
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        /*
        void setCutoff(T lo_cutoff, T hi_cutoff)
        {
            LoFc = lo_cutoff;
            HiFc = hi_cutoff;
            filter.bandstop(LoFc,HiFc,Fs);
        }
        */
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };

    /* clicks    
    template<typename T>
    kfr::univector<T> chebyshev1_lowpass(int order, T rp, T lo, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev1LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev1_highpass(int order, T rp,T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev1HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev1_bandpass(int order, T rp, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev1BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev1_bandstop(int order, T rp, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev1BandStopFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    */   

}