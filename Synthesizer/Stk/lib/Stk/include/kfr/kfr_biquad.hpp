#pragma once 

namespace DSP {
    template<typename T>
    struct BiQuadParams
    {
        std::vector<kfr::biquad_params<T>>  bq;

        BiQuadParams() = default;
        ~BiQuadParams() = default;

        kfr::biquad_params<T>* get_params() { return bq.data(); }

        kfr::biquad_params<T> biquad_notch(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_lowpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_highpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_peak(T f, T q, T gain) {
            return(kfr::biquad_params<T>(kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain))));
        }
        kfr::biquad_params<T> biquad_lowshelf(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_highshelf(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_bandpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }

        void add_notch(T f, T q) {
            kfr::biquad_params<T> p  = biquad_notch(f,q);
            bq.push_back(p);
        }
        void add_lowpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_lowpass(f,q);
            bq.push_back(p);
        }
        void add_highpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_highpass(f,q);
            bq.push_back(p);
        }
        void add_peak(T f, T q, T gain) {
            kfr::biquad_params<T> p  = biquad_peak(f,q, gain);
            bq.push_back(p);
        }
        void add_lowshelf(T f, T q) {
            kfr::biquad_params<T> p  = biquad_lowshelf(f,q);
            bq.push_back(p);
        }
        void add_highself(T f, T q) {
            kfr::biquad_params<T> p  = biquad_highshelf(f,q);
            bq.push_back(p);            
        }
        void add_bandpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_bandpass(f,q);
            bq.push_back(p);
        }
    };
    
    template<typename T>
    struct Biquad
    {
    
        kfr::biquad_filter<T> * filter;

        void init(const BiQuadParams<T> & params)
        {                       
            filter = new kfr::biquad_filter<T>(params.bq.data(),params.bq.size());
        }
        void init(const kfr::biquad_params<T> & params)
        {                       
            kfr::biquad_params<T> pa[1] = {params};
            filter = new kfr::biquad_filter<T>(pa,1);    
        }
        void init(const std::vector<kfr::biquad_params<T>> & params)
        {                       
            filter = new kfr::biquad_filter<T>(params.data(),params.size());    
        }
    

        Biquad() = default;

        Biquad(const kfr::biquad_params<T> & params) {         
            init(params); 
        }        
        Biquad(const BiQuadParams<T> & params) {         
            init(params); 
        }        
        ~Biquad() { if(filter) delete filter; }

        
        void notch(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void lowpass(T f, T q) {
            kfr::biquad_params<T> params =  kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void highpass(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void peak(T f, T q, T gain) {
            kfr::biquad_params<T> params = kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain));
            init(params);
        }
        void lowshelf(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void highshelf(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void bandpass(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter->apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter->apply(dst,src);
        }

        void reset() { filter->reset(); }
    };

    
    template<typename T>
    void add_notch(BiQuadParams<T> &bqp, T f, T q) {
        kfr::biquad_params<T> p  = biquad_notch(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_lowpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_lowpass(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_highpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_highpass(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_peak(BiQuadParams<T> &bqp,T f, T q, T gain) {
        kfr::biquad_params<T> p  = biquad_peak(f,q, gain);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_lowshelf(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_lowshelf(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_highself(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_highshelf(f,q);
        bqp.bq.push_back(p);            
    }
    template<typename T>    
    void add_bandpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_bandpass(f,q);
        bqp.bq.push_back(p);
    }

    template<typename T>
    kfr::biquad_params<T> notch_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowpass_params(T f, T q) {
        kfr::biquad_params<T> params =  kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highpass_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> peak_params(T f, T q, T gain) {
        kfr::biquad_params<T> params = kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowshelf_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highshelf_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> bandpass_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }

    
    template<typename T>
    struct NotchFilter {
        Biquad<T> filter;

        NotchFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.notch(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct LowPassFilter {
        Biquad<T> filter;

        LowPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.lowpass(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct HighPassFilter {
        Biquad<T> filter;

        HighPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.highpass(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct BandPassFilter {
        Biquad<T> filter;

        BandPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.bandpass(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct PeakFilter {
        Biquad<T> filter;

        PeakFilter( const BiQuadParams<T>& p, T f, T q, T gain) {        
            filter.peak(f,q,gain);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct LowShelfFilter {
        Biquad<T> filter;

        LowShelfFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.lowshelf(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct HighShelfFilter {
        Biquad<T> filter;

        HighShelfFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.highshelf(f,q);
        }

        void apply(kfr::univector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    // simple apply biquad functions
    template<typename T>
    kfr::univector<T> biquad(const BiQuadParams<T> & bp, kfr::univector<T> & input) {
        return kfr::univector<T>(kfr::biquad(bp.bq,input));
    }

    // simple biquad filter functions
    template<typename T>
    kfr::univector<T> lowpassfilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowpass(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    kfr::univector<T> highpassfilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highpass(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    kfr::univector<T> notchfilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.notch(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    kfr::univector<T> bandpassfilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.bandpass(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    kfr::univector<T> peakfilter(kfr::univector<T> & input, T freq, T q, T gain) {
        Biquad<T> filter;
        filter.peak(freq,q,gain);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    kfr::univector<T> lowshelffilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowshelf(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;    
    }
    template<typename T>
    kfr::univector<T> highshelffilter(kfr::univector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highshelf(freq,q);
        kfr::univector<T> r(input);
        filter.apply(r,input);
        return r;
    }
}