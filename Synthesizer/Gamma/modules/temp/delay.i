%{
#include "Gamma/Delay.h"
#include "Gamma/Containers.h"
%}

namespace gam {

    template<
        class Tv = gam::real,
        template<class> class Si = ipl::Linear,
        class Td = GAM_DEFAULT_DOMAIN
    >
    // needs arraypow2 interface
    class Delay
    {
    public:

        Delay();
        Delay(float delay);
        Delay(float maxDelay, float delay);

        void delay(float v);						///< Set delay length
        void delaySamples(uint32_t v);				///< Set delay length in samples
        void delaySamplesR(float v);				///< Set delay length in samples (real-valued)
        void delayUnit(float u);					///< Set delay as (0, 1) of buffer size
        void freq(float v);							///< Set natural frequency (1/delay())
        void ipolType(ipl::Type v);
        void maxDelay(float v, bool setDelay=true);	///< Set maximum delay length

        //Tv operator()(const Tv& v);					///< Returns next filtered value
        //Tv operator()() const;						///< Reads delayed element from buffer
        %extern {
            Tv Process(const Tv & v) { return (*$self)(v); }
            Tv Tick() { return (*$self)(v); }
        }
        Tv read(float ago) const;					///< Returns element 'ago' units ago        
        void write(const Tv& v);					///< Writes new element into buffer


        //template <class V>
        //void read(V * dst, unsigned len, unsigned end=0) const;

        float delay() const;						///< Get current delay length
        uint32_t delaySamples() const;				///< Get current delay length in samples
        float delaySamplesR() const;				///< Get current delay length in samples (real-valued)
        float delayUnit() const;					///< Get unit delay (relative to max delay)
        uint32_t delayIndex(uint32_t delay) const;	///< Get index of delayed element	
        float freq() const;
        uint32_t indexBack() const;					///< Get index of backmost element
        float maxDelay() const;						///< Get maximum delay length units

        virtual void onResize();
        void onDomainChange(double r);

        void print();

   };

   template <
        class Tv = gam::real,
        template <class> class Si = ipl::Linear,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Multitap : public Delay<Tv,Si,Td> {
    public:

        Multitap(float delay, unsigned numTaps);
        unsigned taps() const;
        
        Tv read(unsigned tap) const;
        
        void delay(float length, unsigned tap);
        void freq(float v, unsigned tap);
        void taps(unsigned numTaps);
    };

    template <unsigned N, class T>
    class DelayShift{
    public:


        DelayShift(const T& v=T());
        T& operator[](unsigned i);        
        const T& operator[](unsigned i) const;
        T * elems();
        const T * elems() const;        
        void write(const T& v);

        %extend {
            T Tick(const T& v ) { return (*$self)(v); }
        }
        /// Get size of delay
        static unsigned size();
    };


    template<class T = gam::real>
    class Delay1: public DelayShift<1,T>
    {
    public:        
        Delay1(const T& v=T());
    };

    template<class T = gam::real>
    class Delay2 : public DelayShift<2,T>{
    public:

        Delay2(const T& v=T());
        Delay2(const T& v2, const T& v1);
    };

    template<
        class Tv = gam::real,
        template <class> class Si = ipl::Linear,
        class Tp = gam::real,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Comb : public Delay<Tv,Si,Td> {
    public:

        Comb();
        Comb(float delay, const Tp& ffd = Tp(0), const Tp& fbk = Tp(0));
        Comb(float maxDelay, float delay, const Tp& ffd, const Tp& fbk);

        void decay(float units, float end = 0.001f);
        void allPass(const Tp& v);
        void fbk(const Tp& v);					///< Set feedback amount, in (-1, 1)
        void ffd(const Tp& v);					///< Set feedforward amount [-1, 1]
        void feeds(const Tp& fwd, const Tp& bwd);
        void set(float delay, const Tp& ffd, const Tp& fbk); ///< Set several parameters

        
        %extend {
            Tv Tick() { return (*$self)(); }
            Tv flter(const Tv &i0, const Tv &oN) { return (*$self)(i0,oN); }
            Tv filter(const Tv& i0) { return (*$self)(i0); }
        }

        Tv circulateFbk(const Tv& i0, const Tv& oN);
        Tv nextFbk(const Tv& i0);        
        float norm() const;				///< Get unity gain scale factor
        float normFbk() const;			///< Get unity gain scale factor due to feedback
        float normFfd() const;			///< Get unity gain scale factor due to feedforward
        Tp ffd() const;					///< Get feedforward amount
        Tp fbk() const;					///< Get feedback amount
    };
}

%template(Delay) gam::Delay<SampleType>;
%template(Multitap) gam::Multitap<SampleType>;
//%template(DelayShift) gam::DelayShift<SampleType>;
%template(Delay1) gam::Delay1<SampleType>;
%template(Delay2) gam::Delay2<SampleType>;
%template(Comb) gam::Comb<SampleType>;