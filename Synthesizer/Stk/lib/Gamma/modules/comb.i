%{
#include "Gamma/Delay.h"
#include "Gamma/Containers.h"
%}


namespace gam 
{
    template<
        class Tv = gam::real,
        template <class> class Si = ipl::Linear,
        class Tp = gam::real,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Comb 
    {
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
        Tv operator()();
        Tv operator()(const Tv& i0);				///< Returns next filtered value
        Tv operator()(const Tv& i0, const Tv& oN);	///< Circulate filter with ffd & fbk

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

        void delay(float v);						///< Set delay length
        void delaySamples(uint32_t v);				///< Set delay length in samples
        void delaySamplesR(float v);				///< Set delay length in samples (real-valued)
        void delayUnit(float u);					///< Set delay as (0, 1) of buffer size
        void freq(float v);							///< Set natural frequency (1/delay())
        void ipolType(ipl::Type v);
        void maxDelay(float v, bool setDelay=true);	///< Set maximum delay length

        %extern {
            Tv Process(const Tv & v) { return (*$self)(v); }
            Tv Tick() { return (*$self)(); }
        }
        Tv read(float ago) const;					///< Returns element 'ago' units ago        
        void write(const Tv& v);					///< Writes new element into buffer
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
}