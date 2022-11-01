#pragma once 

namespace DSP 
{
    template<typename T>
    struct BesselFilter 
    {        
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;

        int order;
        static constexpr size_t maxorder=32;

        BesselFilter() = default;
        BesselFilter(int _order) : order(_order) {} 
        
        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::bessel<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::bessel<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::bessel<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::bessel<T>(order),lo_cutoff,hi_cutoff,sample_rate);
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
    struct BesselLowPassFilter {
        BesselFilter<T> filter;

        BesselLowPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct BesselHighPassFilter {
        BesselFilter<T> filter;

        BesselHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandPassFilter {
        BesselFilter<T> filter;

        BesselBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandStopFilter {
        BesselFilter<T> filter;

        BesselBandStopFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    kfr::univector<T> bessel_lowpass(int order, T lo, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        BesselLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> bessel_highpass(int order, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        BesselHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> bessel_bandpass(int order, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        BesselBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> bessel_bandstop(int order, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        BesselBandStopFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    


}