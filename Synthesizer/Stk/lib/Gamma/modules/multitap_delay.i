%{
#include "Gamma/Delay.h"
#include "Gamma/Containers.h"
%}


namespace gam {
   template <
        class Tv = gam::real,
        template <class> class Si = ipl::Linear,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Multitap : public Delay<Tv,Si,Td> {
    public:

        Multitap(float delay, unsigned numTaps);
        unsigned taps() const;
        
        Tv   read(unsigned tap) const;
        void delay(float length, unsigned tap);
        void freq(float v, unsigned tap);
        void taps(unsigned numTaps);


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