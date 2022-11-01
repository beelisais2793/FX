%{
#include "Gamma/Oscillator.h"
%}

namespace gam
{
    template <class Sp = phsInc::Loop, class Td = GAM_DEFAULT_DOMAIN>
    class Accum  {
    public:

        Accum(float frq=440, float phs=0);

        void freq(float v);				///< Set frequency
        void freqI(uint32_t v);			///< Set fixed-point frequency
        void freqAdd(float v);			///< Add value to frequency for 1 sample
        void freqMul(float v);			///< Multiply frequency by value for 1 sample
        void phase(float v);			///< Set phase from [0, 1) of one period
        void phaseMax();				///< Set phase to maximum value
        void phaseAdd(float v);			///< Add value to phase [0, 1)
        void period(float v);			///< Set period length

        void reset();
        void finish();

        Sp& phsInc();

        bool done() const;				///< Returns true if done cycling
        bool cycled() const;			///< Returns whether phase cycled on last iteration

        float freq() const;				///< Get frequency
        uint32_t freqI() const;			///< Get fixed-point frequency
        float freqUnit() const;			///< Get frequency in [0, 1)
        float period() const;			///< Get period
        float phase() const;			///< Get phase in [0, 1)
        uint32_t phaseI() const;		///< Get fixed-point phase

    
        %extend {
            bool Tick() { return (*$self)(); }
        }
        uint32_t nextPhase();			///< Increment phase and return updated phase
        uint32_t nextPhase(float freqOffset);
        uint32_t cycles();				///< Get 1 to 0 transitions of all accumulator bits
        bool cycle();
        bool once();
        bool seq(uint32_t pattern);

        void onDomainChange(double r);
    };


    template <class Sp = phsInc::Loop, class Td = GAM_DEFAULT_DOMAIN>
    class Sweep : public Accum<Sp, Td> {
    public:
      
        Sweep(float frq=440, float phs=0);

        %extend {
            float Tick() { return (*$self)(); }
        }
    };   

    template <class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    class AccumPhase {
    public:
        
        AccumPhase(Tv frq=440, Tv phs=0, Tv rad=M_PI);

        Tv nextPhase();
        Tv nextPhase(Tv frqOffset);

        void freq(Tv v);		///< Set frequency
        void freqAdd(Tv v);		///< Add value to frequency for 1 sample
        void freqMul(Tv v);		///< Multiply frequency by value for 1 sample
        void period(Tv v);		///< Set period length
        void phase(Tv v);		///< Set phase from [0, 1) of one period
        void phaseAdd(Tv v);	///< Add value to unit phase
        void radius(Tv v);		///< Set output range to be in [-radius, radius)
        
        Tv freq() const;		///< Get frequency
        Tv freqUnit() const;	///< Get frequency in [0, 1)
        Tv period() const;		///< Get period
        Tv phase() const ;		///< Get normalized phase in [0, 1)
        Tv radius() const;		///< Get radius of output
        
        void onDomainChange(double r);
        void print(FILE * fp = stdout, const char * append = "\n");        
    };

    template<
        class Tv = gam::real,
        template<class> class Si = ipl::Linear,
        class Sp = phsInc::Loop,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Osc : public Accum<Sp,Td>, public ArrayPow2<Tv>{
    public:

        /// Default constructor does not allocate table memory
        Osc();
        Osc(float frq, float phs=0, uint32_t size=512);
        Osc(float frq, float phs, ArrayPow2<Tv>& src);
        
        %extend {
            Tv Tick() { return (*$self)(); }
        }

        
        Tv val() const;
        Tv atPhaseI(uint32_t v) const;

        Osc& addSine(double cycles, double amp=1, double phs=0);        
        gam::ArrayPow2<Tv>& table();
        const gam::ArrayPow2<Tv>& table() const;
    };

    template<class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    class CSine {
    public:

        typedef Complex<Tv> complex;

        CSine(Tv frq=440, Tv amp=1, Tv dcy=-1, Tv phs=0);
        complex val;				///< Current value
        
        %extend {
            complex Tick() { return (*$self)(); }
        }

        void amp(Tv v);				///< Set amplitude
        void decay(Tv v);			///< Set -60 dB decay length (negative for no decay)
        void freq(Tv v);			///< Set frequency
        void freq(complex v){ mInc=v; }
        void phase(Tv v);			///< Set unit phase, in [0,1]
        void reset();				///< Reset amplitude and set phase to 0
        void set(Tv frq, Tv phs, Tv amp, Tv dcy);

        Tv amp() const;
        Tv decay() const;
        Tv freq() const;

        void onDomainChange(double r);
    };

    template<class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    class Sine : public AccumPhase<Tv,Td> {
    public:
      
        Sine(Tv frq=440, Tv phs=0);
      
        %extend {
            Tv Tick(Tv frq0 = Tv(0)) { return (*$self)(frq0); }
        }
    };

    template <class Tv = double, class Td = GAM_DEFAULT_DOMAIN>
    class SineR : public gen::RSin<Tv>
    {
    public:

        SineR(Tv frq=440, Tv amp=1, Tv phs=0);

        
        Tv freq() const;        
        void ampPhase(Tv a=1, Tv p=0);
        void freq(Tv v);
        void period(Tv v);
        void set(Tv frq, Tv amp, Tv phs=0);
        void onDomainChange(double ratio);
    };

    template <class Tv = double, class Td = GAM_DEFAULT_DOMAIN>
    class SineRs : public Array<SineR<Tv, Domain1> >, Td{
    public:

        SineRs();
        SineRs(unsigned num);

        %extend {
            Tv Tick() { return (*$self)(); }
        }
        Tv last(unsigned i) const;
        void set(unsigned i, Tv frq, Tv amp=1, Tv phs=0);
        void onDomainChange(double ratio);
    };

    template <class Tv = double, class Td = GAM_DEFAULT_DOMAIN>
    class SineD : public gen::RSin2<Tv>
    {
    public:

        SineD(Tv frq=440, Tv amp=1, Tv dcy=-1, Tv phs=0);

        Tv freq() const;
        void ampPhase(Tv a=1, Tv p=0);
        void decay(Tv v);
        void freq(Tv v);
        void period(Tv v);
        void set(Tv frq, Tv amp, Tv dcy, Tv phs=0);
        void onDomainChange(double ratio);
    };

    template <class Tv = double, class Td = GAM_DEFAULT_DOMAIN>
    class SineDs : public Array<SineD<Tv, Domain1> > {
    public:

        SineDs(){}        
        SineDs(unsigned num): Base(num);

        Tv operator()(){
            Tv r=Tv(0);
            for(unsigned j=0; j<this->size(); ++j) r+=(*this)[j]();
            return r;
        }

        %extend {
            Tv Tick() { return (*$self)(); }
        }
        
        Tv last(unsigned i) const;
        void set(unsigned i, Tv frq, Tv amp, Tv dcy, Tv phs=0);
        void onDomainChange(double ratio);
    };

    template<class Tv = double, class Td = GAM_DEFAULT_DOMAIN>
    class Chirplet {
    public:

        typedef Complex<Tv> complex;

        Chirplet(Tv frq1=440, Tv frq2=880, Tv amp=1, Tv len=1, Tv phs=0);
        Chirplet& freq(double start, double end);        
        Chirplet& freq(double v);
        Chirplet& length(double v, const complex& offset=complex(0.01));
        Chirplet& amp(double v);


        %extend {
            complex Tick() { return (*$self)(); }
        }
        
        
        Chirplet& reset();
        bool done(float thresh=0.001) const;
        Tv length() const;
    };

    template <class Sp = phsInc::Loop, class Td = GAM_DEFAULT_DOMAIN>
    class LFO : public Accum<Sp,Td>{
    public:

        LFO();
        LFO(double frq, double phase=0, double mod=0.5);
        
        LFO& set(float f, float p, float m);

        LFO& mod(double n);		///< Set modifier from unit value
        LFO& modI(uint32_t v);	///< Set modifier from integer
        
        uint32_t modI() const;
        double mod() const;

        float cos();		///< Cosine-like wave based on 3rd order polynomial
        float down();		///< Downward ramp (1 to -1); S1 Clausen function
        float even3();		///< Even harmonic sine-like wave (3rd order); S3 Clausen function
        float even5();		///< Even harmonic sine-like wave (5th order)
        float imp();		///< Impulse train with aliasing reduction
        float line2();		///< 2-segment line; 'mod' changes wave from down to tri to up
        float para();		///< Parabolic wave; C2 Clausen function
        float pulse();		///< Pulse (up + down). 'mod' controls pulse width
        float pulseRange(); ///< Pulse (up + down). 'mod' controls pulse width. amplitude doesn't change with mod.
        float saw(){ return down(); } ///< Saw wave (downward ramp)
        float sinPara();	///< Sine-like wave constructed from parabolas; integral of triangle
        float stair();		///< Stair (square + square). 'mod' controls pulse width
        float sqr();		///< Square (-1 to 1)
        float tri();		///< Triangle (starts at 1 goes down to -1 then up to 1)
        float up();			///< Upward ramp
        float up2();		///< Dual upward ramp (up + up). 'mod' controls pulse width.

        float S1();			///< S1 Clausen function; sum_k sin(kt)/k^1
        float C2();			///< C2 Clausen function; sum_k cos(kt)/k^2
        float S3();			///< S3 Clausen function; sum_k sin(kt)/k^3
        float C4();			///< C4 Clausen function; sum_k cos(kt)/k^4
        float S5();			///< S5 Clausen function; sum_k sin(kt)/k^5

        float cosU();		///< Unipolar cosine-like wave based on 3rd order polynomial
        float downU();		///< Unipolar downward ramp
        float hann();		///< Hann-like window based on 3rd order polynomial
        float impU();		///< Unipolar impulse train
        float line2U();		///< Unipolar line2
        float paraU();		///< Unipolar parabolic wave
        float pulseU();		///< Unipolar pulse
        float stairU();		///< Unipolar stair
        float sqrU();		///< Unipolar square
        float triU();		///< Unipolar triangle (starts at 1 going down then up)
        float upU();		///< Unipolar upward ramp
        float up2U();		///< Unipolar upward ramp2

        float patU();
        float patU(uint32_t mul);
        float sineT9();
        float sineP9();        
    };

    template <class Sp = phsInc::Loop, class Td = GAM_DEFAULT_DOMAIN>
    class DWO : public Accum<Sp,Td>{
    public:

        DWO();
        DWO(float frq, float phase=0, float mod=0.5);

        DWO& mod(double n);		///< Set modifier from unit value
        DWO& modI(uint32_t v);	///< Set modifier from integer

        uint32_t modI() const;
        double mod() const;        
        void freq(float v);
        void period(float v);

        float up();				///< Upward saw
        float down();			///< Downward saw
        float saw();
        float sqr();			///< Square
        float para();			///< Parabolic
        float tri();			///< Triangle
        float pulse();			///< Pulse
        float imp();			///< Impulse train
        void onDomainChange(double r);

    };


    template<class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    class Buzz : public AccumPhase<Tv,Td> {
    public:

        Buzz(Tv frq=440, Tv phase=0, Tv harmonics=8);
        virtual ~Buzz();

        void antialias();			///< Adjust number of harmonics to prevent aliasing
        void harmonics(Tv num);		///< Set number of harmonics
        void harmonicsMax();		///< Set number of harmonics to fill Nyquist range
        void normalize(bool v);		///< Whether to normalize amplitude

        Tv operator()();			///< Returns next sample of all harmonic impulse
        Tv odd();					///< Returns next sample of odd harmonic impulse
        Tv saw(Tv intg=0.999);		///< Returns next sample of saw waveform
        Tv square(Tv intg=0.999);	///< Returns next sample of square waveform
        
        Tv maxHarmonics() const;	///< Get number of harmonics below Nyquist based on current settings

        void onDomainChange(double r);
    };


    template <class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    struct Impulse : public Buzz<Tv,Td>{
    public:

        Impulse(Tv frq=440, Tv phs=0);
        void freq(Tv v);
        void onDomainChange(double r);

        using Buzz<Tv,Td>::freq; // needed for getter    
    };

    template <class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    struct Saw : public Impulse<Tv,Td> {
        Saw(Tv frq=440, Tv phs=0);
        
        %extend {
            Tv Tick(Tv itg=0.999) { return (*$self)(itg); }
        }
    };

    template <class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    struct Square : public Impulse<Tv,Td> {

        Square(Tv frq=440, Tv phs=0) : Impulse<Tv,Td>(frq, phs){}
        %extend {
            Tv Tick(Tv itg=0.999) { return (*$self)(itg); }
        }
    };

    template<class Tv = gam::real, class Td = GAM_DEFAULT_DOMAIN>
    class DSF : public AccumPhase<Tv,Td> {
    public:

        DSF(Tv frq=440, Tv freqRatio=1, Tv ampRatio=0.5, Tv harmonics=8);
        
        Tv operator()();			///< Generate next sample

        %extend {
            Tv Tick() { return (*$self)(); }
        }

        void ampRatio(Tv v);		///< Set amplitude ratio of partials
        void antialias();			///< Adjust harmonics so partials do not alias
        void freq(Tv v);			///< Set frequency of fundamental
        void freqRatio(Tv v);		///< Set frequency ratio of partials
        void harmonics(Tv v);		///< Set number of harmonics
        void harmonicsMax();		///< Set number of harmonics to fill Nyquist range

        Tv ampRatio() const;		///< Get amplitude ratio
        Tv freqRatio() const;		///< Get frequency ratio
        Tv harmonics() const;		///< Get current number of harmonics
        Tv maxHarmonics() const;	///< Get maximum number of harmonics for current settings
        
        void onDomainChange(double r);
    };

    template <
        class Gen = gen::Default<>,
        template <typename> class Si = iplSeq::Linear,
        class Sp = phsInc::Loop,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Upsample : public Accum<Sp,Td>{
    public:

        typedef typename Gen::value_type value_type;

      
        Upsample(float frq=440);

        /*
        template <class SampleGen>
        value_type operator()(SampleGen& gen){
            if(this->cycle()) mIpl.push(gen());
            return mIpl(this->phase());
        }
        
        // Upsample an external function
        value_type operator()(const std::function<value_type(void)>& onCycle){
            if(this->cycle()) mIpl.push(onCycle());
            return mIpl(this->phase());
        }
       */
      
        %extend 
        {
            SampleType Process(SampleType in) { return (*$self)(in); }
            SampleType Tick() { return (*$self)(); }
        }
        /// Get internal generator
        Gen& gen();

    };    
}

%template (Accum) gam::Accum<>;
%template (Sweep) gam::Sweep<>;
%template (AccumPhase) gam::AccumPhase<SampleType>;
%template (Osc) gam::Osc<SampleType>;
%template (CSine) gam::CSine<SampleType>;
%template (Sine) gam::Sine<SampleType>;
%template (SineR) gam::SineR<SampleType>;
%template (SineRs) gam::SineRs<SampleType>;
%template (SineD) gam::SineD<SampleType>;
%template (SineDs) gam::SineDs<SampleType>;
%template (SineR) gam::SineR<SampleType>;
%template (Chirplet) gam::Chirplet<SampleType>;
%template (LFO) gam::LFO<>;
%template (DWO) gam::DWO<>;
%template (Buzz) gam::Buzz<SampleType>;
%template (Saw) gam::Saw<SampleType>;
%template (Square) gam::Square<SampleType>;
%template (DSF) gam::DSF<SampleType>;
