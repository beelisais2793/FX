#pragma once 

namespace DSP 
{
    template<typename T>
    struct ButterworthFilter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        static constexpr size_t maxorder=32;

        ButterworthFilter(int _order) : order(_order) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::butterworth<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::butterworth<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::butterworth<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::butterworth<T>(order),lo_cutoff,hi_cutoff,sample_rate);
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
    struct ButterworthLowPassFilter {
        ButterworthFilter<T> filter;

        ButterworthLowPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct ButterworthHighPassFilter {
        ButterworthFilter<T> filter;

        ButterworthHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandPassFilter {
        ButterworthFilter<T> filter;

        ButterworthBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandStopFilter {
        ButterworthFilter<T> filter;

        ButterworthBandStopFilter(int order, T lo_cutoff, T hi_cutoff ,T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(kfr::univector<T> &in) { filter.apply(in);}
        void apply(const kfr::univector<T> &in, kfr::univector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    kfr::univector<T> butterworth_lowpass(int order, T lo, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        ButterworthLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> butterworth_highpass(int order, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        ButterworthHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> butterworth_bandpass(int order, T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        ButterworthBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    kfr::univector<T> butterworth_bandstop(int order,  T lo, T hi, T sample_rate, const kfr::univector<T> & in) {
        kfr::univector<T> out(in.size());
        ButterworthBandStopFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    


}