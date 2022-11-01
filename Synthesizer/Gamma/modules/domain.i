%{
#include "Gamma/Domain.h"
%}

namespace gam
{

    class Domain1{
    public:

        double spu() const;
        double ups() const;
        const Domain1 * domain() const;
        bool hasBeenSet() const;
        void onDomainChange(double /*ratioSPU*/);
        void spu(double /*val*/);
        void ups(double /*val*/);
    };

    class DomainObserver : public Node2<DomainObserver> {
    public:

        DomainObserver();
        DomainObserver(const DomainObserver& rhs);
        virtual ~DomainObserver();
        double spu() const;				///< Get samples/unit
        double ups() const;				///< Get units/sample
        const Domain * domain() const;	///< Get pointer to my subject domain
        virtual void onDomainChange(double /*ratioSPU*/);
        void domain(Domain& src);
        DomainObserver& operator= (const DomainObserver& rhs);
    };

    class Domain
    {
    public:

        Domain();
        Domain(double spu);
        ~Domain();

        //Domain& operator<< (DomainObserver& obs);	///< Attach observer
        void notifyObservers(double r);		///< Notify observers (\sa DomainObserver::onDomainChange)
        void spu(double v);					///< Set samples/unit and notify observers
        void ups(double v);					///< Set units/sample and notify observers

        bool hasBeenSet() const;			///< Returns true if spu has been set at least once
        double spu() const;					///< Returns samples/unit, i.e. sample rate
        double ups() const;					///< Returns units/sample, i.e. sample interval

        void print() const;        
        static Domain& master();
    };

    void sampleRate(double samplesPerSecond);    
    double sampleRate();
}