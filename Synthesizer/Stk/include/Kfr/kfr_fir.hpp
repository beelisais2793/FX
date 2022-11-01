#pragma once 

namespace DSP {
    template<typename T>
    struct FIRFilter {
    private:
        kfr::filter_fir<SampleType> * filter;
        kfr::univector<T> taps;
        
    public:
        
        FIRFilter(size_t num_taps) { 
            taps.resize(num_taps); 
            filter = nullptr;
        }
        ~FIRFilter() {
            if(filter != NULL) delete filter;
        }
        void bandpass(T x, T y, kfr::expression_pointer<T> & window, bool normalize=true ) {        
            kfr::fir_bandpass(taps,x,y,window,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void bandstop(T x, T y, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_bandstop(taps,x,y, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void highpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_highpass(taps,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void lowpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_lowpass(taps,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        
        void apply(kfr::univector<T> & data) {
            filter->apply(data);
        }
        void apply(kfr::univector<T> & out, const kfr::univector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }

    };

    template<typename T>
    struct FIRBandpassFilter
    {
        FIRFilter<T> * filter;

        FIRBandpassFilter(size_t num_taps, T x, T y, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->bandpass(x,y,window,normalize);
        }
        ~FIRBandpassFilter() {
            if(filter) delete filter;
        }
        void apply(kfr::univector<T> & data) {
            filter->apply(data);
        }
        void apply(kfr::univector<T> & out, const kfr::univector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRBandstopFilter
    {
        FIRFilter<T> * filter;

        FIRBandstopFilter(size_t num_taps, T x, T y, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->bandstop(x,y,window,normalize);
        }
        ~FIRBandstopFilter() {
            if(filter) delete filter;
        }
        void apply(kfr::univector<T> & data) {
            filter->apply(data);
        }
        void apply(kfr::univector<T> & out, const kfr::univector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRLowpassFilter
    {
        FIRFilter<T> * filter;

        FIRLowpassFilter(size_t num_taps, T x, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->lowpass(x,window,normalize);
        }
        ~FIRLowpassFilter() {
            if(filter) delete filter;
        }
        void apply(kfr::univector<T> & data) {
            filter->apply(data);
        }
        void apply(kfr::univector<T> & out, const kfr::univector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRHighpassFilter
    {
        FIRFilter<T> * filter;

        FIRHighpassFilter(size_t num_taps, T x, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->highpass(x,window,normalize);
        }
        ~FIRHighpassFilter() {
            if(filter) delete filter;
        }
        void apply(kfr::univector<T> & data) {
            filter->apply(data);
        }
        void apply(kfr::univector<T> & out, const kfr::univector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };



    template<typename T>
    kfr::univector<T> fir_lowpass(const kfr::univector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.lowpass(cutoff, window_type, normalize);
        kfr::univector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    kfr::univector<T> fir_highpass(const kfr::univector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.highpass(cutoff, window_type, normalize);
        kfr::univector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    kfr::univector<T> fir_bandpass(const kfr::univector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandpass(x, y, window_type, normalize);
        kfr::univector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    kfr::univector<T> fir_bandstop(const kfr::univector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandstop(x, y, window_type, normalize);
        kfr::univector<T> out(input.size());
        f.apply(out,input);
        return out;
    }


}