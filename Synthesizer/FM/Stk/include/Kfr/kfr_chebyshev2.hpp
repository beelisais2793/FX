#pragma once 

namespace DSP 
{
    template<typename T>
    struct Chebyshev2Filter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        T rs;
        static constexpr size_t maxorder=32;

        Chebyshev2Filter() = default;
        Chebyshev2Filter(int _order, T _rs) : order(_order), rs(_rs) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::chebyshev2<T>(order,rs),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::chebyshev2<T>(order,rs),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev2<T>(order,rs),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev2<T>(order,rs),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(kfr::univector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const kfr::univector<T> &in, kfr::univector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    
    template<typename T>
    struct Chebyshev2LowPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2LowPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev2HighPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2HighPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandPassFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandStopFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandStopFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    kfr::univector<T> chebyshev2_lowpass(int order, T rp, T lo, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev2LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev2_highpass(int order, T rp,T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev2HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev2_bandpass(int order, T rp, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev2BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> chebyshev2_bandstop(int order, T rp, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        Chebyshev2BandStopFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }


}