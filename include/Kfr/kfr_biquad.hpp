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
    
        kfr::biquad_filter<T> *filter;

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
    
        Biquad<T>& operator = (Biquad<T> & in) {
            assert(filter != NULL);
            assert(in.filter != NULL);
            *filter = *in.filter;
            reset();
            return *this;
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
        void allpass(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_allpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
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
    kfr::biquad_params<T> allpass_params(T f, T q) {
        kfr::biquad_params<T> params =  kfr::biquad_allpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
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



/////////////////////////////////////////////////////////
// Notch
// Order 1,2,3,4 (number of cascades supported by Kfr)
/////////////////////////////////////////////////////////

    template<typename T>
    struct NotchFilter {
        Biquad<T> filter;
        T Fc,Fs,W,Q;
        unsigned Order;

        void notch1_filter()
        {
            filter.notch(W,Q);
        }
        void notch2_filter()
        {
            kfr::biquad_params<T> f1 = DSP::notch_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::notch_params(W,T(1.3065460));                
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);                
            filter.init(fv);                            
        }
        void notch3_filter()
        {
            kfr::biquad_params<T> f1 = DSP::notch_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::notch_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::notch_params(W,T(1.9318517));
            DSP::Biquad<T> filter;
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);            
        }
        void notch4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::notch_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::notch_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::notch_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::notch_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        NotchFilter( unsigned int order, T f, T fs, T q=0.707) {        
            Fc = f;
            Fs = fs;
            W /= fs;
            Q  = q;
            Order = order;
            switch(order)
            {
            case 1:
                notch1_filter();
                break;
            case 2:                        
                notch2_filter();
                break;
            case 3:
                notch3_filter();
                break;
            case 4:
                notch4_filter();
                break;
            default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;            
            NotchFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;
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



/////////////////////////////////////////////////////////
// Allpass    
// Order 1,2,3,4 (number of cascades supported by Kfr)
/////////////////////////////////////////////////////////
    template<typename T>
    struct AllPassFilter {
        Biquad<T> filter;
        T Fc,Fs,W,Q;
        unsigned int Order;

        
        void allpass1_filter()
        {                    
            return filter.allpass(W,Q);
        }
        
        void allpass2_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::allpass_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::allpass_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);                
            filter.init(fv);
            
        }
        
        void allpass3_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::allpass_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::allpass_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::allpass_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);                
            fv.bq.push_back(f3);                
            filter.init(fv);
            
        }
        
        void allpass4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::allpass_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::allpass_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::allpass_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::allpass_params(W,T(2.5629154));
            DSP::Biquad<T> filter;
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);                
            fv.bq.push_back(f3);                
            fv.bq.push_back(f4);                
            filter.init(fv);            
        }

        AllPassFilter( unsigned int order, T f, T fs, T q=0.707) {        
            Fc = W;
            Fs = fs;    
            W  = Fc/Fs;
            Order =order;
            switch(order)
            {
                case 1: allpass1_filter(); break;
                case 2: allpass2_filter(); break;
                case 3: allpass3_filter(); break;
                case 4: allpass4_filter(); break;
                default: assert(1==0);
            }
        }
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            AllPassFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;
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


/////////////////////////////////////////////////////////
// Lowpass    
// Order 1,2,3,4 (number of cascades supported by Kfr)
/////////////////////////////////////////////////////////

    template<typename T>
    struct LowPassFilter {
        Biquad<T> filter;
        T Fc,Fs,Q,W;
        unsigned Order;

        void lowpass1_filter()
        {        
            filter.lowpass(W,Q);                    
        }
    
        void lowpass2_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::lowpass_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::lowpass_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void lowpass3_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::lowpass_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::lowpass_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::lowpass_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }        
        void lowpass4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::lowpass_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::lowpass_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::lowpass_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::lowpass_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        LowPassFilter( unsigned int order, T f, T sr,T q=0.707) 
        {
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            Order = order;

            switch(order)
            {
                case 1: lowpass1_filter(); break;
                case 2: lowpass2_filter(); break;
                case 3: lowpass3_filter(); break;
                case 4: lowpass4_filter(); break;
                default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            LowPassFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter;
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

/////////////////////////////////////////////////////////
// Highpass    
// Order 1,2,3,4 (number of cascades supported by Kfr)
/////////////////////////////////////////////////////////

    template<typename T>
    struct HighPassFilter {
        Biquad<T> filter;
        T Fc,Fs,Q,W;
        unsigned Order;

        void highpass1_filter()
        {        
            filter.highpass(W,Q);
        }
    
        void highpass2_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::highpass_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::highpass_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void highpass3_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::highpass_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::highpass_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::highpass_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }

        void highpass4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::highpass_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::highpass_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::highpass_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::highpass_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        HighPassFilter( unsigned int order, T f, T sr, T q=0.707) 
        {
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            Order = order;
            switch(order)
            {
                case 1: highpass1_filter(); break;
                case 2: highpass2_filter(); break;
                case 3: highpass3_filter(); break;
                case 4: highpass4_filter(); break;
                default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            HighPassFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;            
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
        T Fc,Fs,Q,W;
        unsigned Order;

        void bandpass1_filter()
        {        
            filter.bandpass(W,Q);
        }
    
        void bandpass2_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::bandpass_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::bandpass_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void bandpass3_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::bandpass_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::bandpass_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::bandpass_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }

        void bandpass4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::bandpass_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::bandpass_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::bandpass_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::bandpass_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        BandPassFilter( unsigned int order, T f, T sr, T q=0.707) 
        {
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            Order =order;
            switch(order)
            {
                case 1: bandpass1_filter(); break;
                case 2: bandpass2_filter(); break;
                case 3: bandpass3_filter(); break;
                case 4: bandpass4_filter(); break;
                default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            BandPassFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;
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
        T Fc,Fs,Q,W,G;
        unsigned Order;
        void peak1_filter()
        {        
            filter.peak(W,Q,G);
        }
    
        void peak2_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::peak_params(W,T(0.54119610),G);
            kfr::biquad_params<T> f2 = DSP::peak_params(W,T(1.3065460),G);            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void peak3_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::peak_params(W,T(0.51763809),G);
            kfr::biquad_params<T> f2 = DSP::peak_params(W,T(0.70710678),G);
            kfr::biquad_params<T> f3 = DSP::peak_params(W,T(1.9318517),G);            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }

        void peak4_filter()
        {        
            kfr::biquad_params<T> f1 = DSP::peak_params(W,T(0.50979558),G);
            kfr::biquad_params<T> f2 = DSP::peak_params(W,T(0.60134489),G);
            kfr::biquad_params<T> f3 = DSP::peak_params(W,T(0.89997622),G);
            kfr::biquad_params<T> f4 = DSP::peak_params(W,T(2.5629154),G);
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        PeakFilter( unsigned order, T f, T sr, T gain, T q=0.707) {        
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            G = gain;
            Order =order;
            switch(order)
            {
                case 1: peak1_filter(); break;
                case 2: peak2_filter(); break;
                case 3: peak3_filter(); break;
                case 4: peak4_filter(); break;
                default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            PeakFilter<T> new_filter(Order,Fc,Fs,G,Q);
            filter = new_filter.filter;
        }

        void setGain(T g) {
            G =g;
            switch(Order)
            {
                case 1: peak1_filter(); break;
                case 2: peak2_filter(); break;
                case 3: peak3_filter(); break;
                case 4: peak4_filter(); break;
                default: assert(1==0);
            }
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
        T Fc,Fs,Q,W;
        unsigned Order;
        void filter1()
        {        
            filter.lowshelf(W,Q);
        }
    
        void filter2()
        {        
            kfr::biquad_params<T> f1 = DSP::lowshelf_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::lowshelf_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void filter3()
        {        
            kfr::biquad_params<T> f1 = DSP::lowshelf_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::lowshelf_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::lowshelf_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }

        void filter4()
        {        
            kfr::biquad_params<T> f1 = DSP::lowshelf_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::lowshelf_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::lowshelf_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::lowshelf_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        LowShelfFilter( unsigned int order, T f, T sr, T q=0.707) 
        {
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            Order =order;
            switch(order)
            {
                case 1: filter1(); break;
                case 2: filter2(); break;
                case 3: filter3(); break;
                case 4: filter4(); break;
                default: assert(1==0);
            }
        }
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            LowShelfFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;
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
        T Fc,Fs,Q,W;
        unsigned Order;
        void filter1()
        {        
            filter.highshelf(W,Q);
        }
    
        void filter2()
        {        
            kfr::biquad_params<T> f1 = DSP::highshelf_params(W,T(0.54119610));
            kfr::biquad_params<T> f2 = DSP::highshelf_params(W,T(1.3065460));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            filter.init(fv);             
        }
        
        void filter3()
        {        
            kfr::biquad_params<T> f1 = DSP::highshelf_params(W,T(0.51763809));
            kfr::biquad_params<T> f2 = DSP::highshelf_params(W,T(0.70710678));
            kfr::biquad_params<T> f3 = DSP::highshelf_params(W,T(1.9318517));            
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            filter.init(fv);                            
        }

        void filter4()
        {        
            kfr::biquad_params<T> f1 = DSP::highshelf_params(W,T(0.50979558));
            kfr::biquad_params<T> f2 = DSP::highshelf_params(W,T(0.60134489));
            kfr::biquad_params<T> f3 = DSP::highshelf_params(W,T(0.89997622));
            kfr::biquad_params<T> f4 = DSP::highshelf_params(W,T(2.5629154));
            DSP::BiQuadParams<T> fv;            fv.bq.push_back(f1);
            fv.bq.push_back(f2);
            fv.bq.push_back(f3);
            fv.bq.push_back(f4);
            filter.init(fv);                            
        }
        HighShelfFilter( unsigned int order, T f, T sr, T q=0.707) 
        {
            Fc = f;
            Fs = sr;
            W = f/sr;
            Q = q;
            Order =order;
            switch(order)
            {
                case 1: filter1(); break;
                case 2: filter2(); break;
                case 3: filter3(); break;
                case 4: filter4(); break;
                default: assert(1==0);
            }
        }
        
        void setCutoff(T f) {
            Fc = f;
            W  = f/Fs;
            HighShelfFilter<T> new_filter(Order,Fc,Fs,Q);
            filter = new_filter.filter;
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

    /* crackles
    // simple apply biquad functions
    template<typename T>
    kfr::univector<T> biquad(const BiQuadParams<T> & bp, kfr::univector<T> & input) {
        return kfr::univector<T>(kfr::biquad(bp.bq,input));
    }

    // simple biquad filter functions
    template<typename T>
    kfr::univector<T> allpassfilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.allpass(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    // simple biquad filter functions
    template<typename T>
    kfr::univector<T> lowpassfilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.lowpass(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    template<typename T>
    kfr::univector<T> highpassfilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.highpass(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    template<typename T>
    kfr::univector<T> notchfilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.notch(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    template<typename T>
    kfr::univector<T> bandpassfilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.bandpass(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    template<typename T>
    kfr::univector<T> peakfilter(kfr::univector<T> & input, T W, T q, T gain) {
        Biquad<T> filter;
        filter.peak(W,q,gain);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    template<typename T>
    kfr::univector<T> lowshelffilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.lowshelf(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;    
    }
    template<typename T>
    kfr::univector<T> highshelffilter(kfr::univector<T> & input, T W, T q) {
        Biquad<T> filter;
        filter.highshelf(W,q);
        kfr::univector<T> r(input);
        filter.apply(input,r);
        return r;
    }
    */
}