%{
#include "Gamma/Filter.h"
%}

namespace gam 
{
    enum FilterType{
        LOW_PASS,			/**< Low-pass */
        HIGH_PASS,			/**< High-pass */
        BAND_PASS,			/**< Band-pass */
        RESONANT,			/**< Resonant band-pass */
        BAND_REJECT,		/**< Band-reject */
        ALL_PASS,			/**< All-pass */
        PEAKING,			/**< Peaking */
        LOW_SHELF,			/**< Low-shelf */
        HIGH_SHELF,			/**< High-shelf */
        SMOOTHING,			/**< Smoothing */
        THRU_PASS			/**< Thru-pass (no filter) */
    };

    /// Returns pole radius given a unit bandwidth
    template <class T>
    inline T poleRadius(T bw);

    /// Returns pole radius given a bandwidth and sampling interval
    template <class T>
    inline T poleRadius(T bw, double ups);

    
    template <class T>
    inline T freqToRad(T freq, double ups);

    template<class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class AllPass1 {
    public:
        
        AllPass1(Tp frq = Tp(1000));

        void freq (Tp v);		///< Set cutoff frequency
        void freqF(Tp v);		///< Faster, but slightly less accurate than freq()	
        void zero();
        
        
        
        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
        Tv high(Tv in);			///< High-pass filters sample
        Tv low (Tv in);			///< Low-pass filters sample        
        Tp freq();				///< Get current cutoff frequency        
        void onDomainChange(double r);    
    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class Biquad {
    public:

        Biquad(
            Tp frq = Tp(1000),
            Tp res = Tp(0.707),
            FilterType type = LOW_PASS,
            Tp lvl = Tp(1)
        );


        const Tp * a() const;
        Tp * a();
        
        
        const Tp * b() const;
        Tp * b();
        void coef(Tp a0, Tp a1, Tp a2, Tp b1, Tp b2);

        void freq(Tp v);					///< Set center frequency
        void res(Tp v);						///< Set resonance (Q); gain at center frequency
        void level(Tp v);					///< Set level (PEAKING, LOW_SHELF, HIGH_SHELF types only)
        void set(Tp frq, Tp res);			///< Set filter center frequency and resonance
        void set(Tp frq, Tp res, FilterType type);	///< Set all filter params
        void type(FilterType type);			///< Set type of filter
        void zero();						///< Zero internal delays

        Tv operator()(Tv in);				///< Filter next sample
        Tv nextBP(Tv in);					///< Optimized for band-pass types
        
        Tp freq() const;					///< Get center frequency
        Tp res() const;						///< Get resonance (Q)
        Tp level() const;					///< Get level
        FilterType type() const;			///< Get filter type
        
        void onDomainChange(double r);
    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class BlockDC {
    public:

        
        BlockDC(Tp bwidth = Tp(35));

        /// Filter sample
        Tv operator()(Tv in){
            Tv t = in + d1*mB1;
            Tv o0 = t - d1;
            d1 = t;
            return o0;
        }

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
        
        void width(Tp v);
        void zero();
        void onDomainChange(double /*r*/);        
    };


    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class BlockNyq : public BlockDC<Tv,Tp,Td>{
    public:

        BlockNyq(Tp bwidth = Tp(35));

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }

        void width(Tp v);
    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class Filter2 {
    public:

        
        void freq(Tp v);
        void width(Tp v);
        
        void zero();

        void onDomainChange(double r);    
    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class AllPass2 : public Filter2<Tv,Tp,Td>{
    public:
        
        AllPass2(Tp frq = Tp(1000), Tp wid = Tp(100));

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }

    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class Notch : public Filter2<Tv,Tp,Td>{
    public:
              
        Notch(Tp frq = Tp(1000), Tp wid = Tp(100));
        void freq(Tp v);
        void width(Tp v);

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
    };

    template <class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class Reson : public Filter2<Tv,Tp,Td>{
    public:

      
        Reson(Tp frq = Tp(1000), Tp wid = Tp(100));
        void freq(Tp v);
        void width(Tp v);
        void set(Tp frq, Tp wid);

        /// Filter sample
        Tv operator()(Tv in){
            Tv t = in * gain() + d1*mC[1] + d2*mC[2];
            this->delay(t);
            return t;
        }

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }    
    };

    template <class Tv=gam::real, class Tp=gam::real>
    class Hilbert {
    public:
        
        Hilbert();

        %extend {
            gam::Complex<Tv> Tick(Tv in) { return (*$self)(in); }
        }
        void zero();
    };

    template <class Tv=double, class Tp=double>
    class Integrator{
    public:

        Integrator(Tp leakCoef = Tp(1), Tv initVal = Tv(0));
        
        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }

        Integrator& leak(Tp v);
        Integrator& zero();    
    };

    template <class T=gam::real>
    class Differencer{
    public:        
        %extend {
            T Tick(T in) { return (*$self)(in); }
        }    
    };

    template <class Tv=gam::real>
    class MovingAvg : public DelayN<Tv>{
    public:

        MovingAvg();
        
        explicit MovingAvg(unsigned size);

        MovingAvg& operator=(const Tv& v);
        
        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
        virtual void onResize();
    };


    template<class Tv=gam::real, class Tp=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class OnePole {
    public:

        
        OnePole(Tp freq = Tp(1000), FilterType type = LOW_PASS, const Tv& stored = Tv(0));        
        //OnePole(Tp freq, const Tv& stored);


        const Tp& freq() const;

        void type(FilterType type);			///< Set type of filter (gam::LOW_PASS or gam::HIGH_PASS)
        void freq(Tp val);					///< Set cutoff frequency (-3 dB bandwidth of pole)

        void lag(Tp length, Tp thresh=Tp(0.001));

        void smooth(Tp val);				///< Set smoothing coefficient directly
        void zero();
        void reset(Tv v = Tv(0));

        %extend 
        {
            const Tv& Tick() { return (*$self)(); }
            const Tv& Process(Tv in) { return (*$self)(in); }
        }
        void operator  = (Tv val);			///< Stores input value for operator()
        void operator *= (Tv val);			///< Multiplies stored value by value

        const Tv& last() const;				///< Returns last output
        const Tv& stored() const;			///< Returns stored value
        Tv& stored();						///< Returns stored value

        bool zeroing(Tv eps=0.0001) const;	///< Returns whether the filter is outputting zeros
        
        void onDomainChange(double r);
    };

}

%template(AllPass1) gam::AllPass1<SampleType,SampleType>;
%template(Biquad) gam::Biquad<SampleType,SampleType>;
%template(BlockDC) gam::BlockDC<SampleType,SampleType>;
%template(BlockNyq) gam::BlockNyq<SampleType,SampleType>;
%template(AllPass2) gam::AllPass2<SampleType,SampleType>;
%template(Notch) gam::Notch<SampleType,SampleType>;
%template(Reson) gam::Reson<SampleType,SampleType>;
%template(Hilbert) gam::Hilbert<SampleType,SampleType>;
%template(Integrator) gam::Integrator<SampleType,SampleType>;
%template(Differencer) gam::Differencer<SampleType>;
%template(MovingAvg) gam::MovingAvg<SampleType>;
%template(OnePole) gam::OnePole<SampleType,SampleType>;