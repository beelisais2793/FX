%{
#include "Gamma/Envelope.h"
%}

namespace gam
{
    template <class Tv=real, class Tp=real>
    class Curve{
    public:

        Curve();
        Curve(Tp length, Tp curve, Tv start=Tv(1), Tv end=Tv(0));

        bool done() const;				///< Returns whether curve has gone past end value
        Tv end() const;
        Tv value() const;				///< Get current value

        Curve& reset(Tv start=Tv(0));	///< Reset envelope
        Curve& value(const Tv& v);		///< Set value
        
        Curve& set(Tp length, Tp curve, Tv start=Tv(0), Tv end=Tv(1));

        %extend {
            Tv Tick() { return (*$self)(); }
        }
    };

    template <int N, class Tv=real, class Tp=real, class Td=GAM_DEFAULT_DOMAIN>
    class Env 
    public:

        Env();
        Env(Tp lvl1, Tp len1, Tp lvl2);
        Env(Tp lvl1, Tp len1, Tp lvl2, Tp len2, Tp lvl3);
        Env(Tp lvl1, Tp len1, Tp lvl2, Tp len2, Tp lvl3, Tp len3, Tp lvl4);

        int size() const;
        unsigned position() const;
        int sustainPoint() const;
        int stage() const;
        Tv value() const;
        bool done() const;
        bool released() const;
        bool sustained() const;
        Tv operator()();
        void release();

        Env& loop(bool v);
        Env& sustainPoint(int v);
        Env& sustainDisable();
        void reset();
        void resetSoft();
        void finish();
        Tp * lengths();
        const Tp * lengths() const;
        
        Env& lengths(const SampleType* vals, int len);        
        Env& lengths(Tp a, Tp b);
        Env& lengths(Tp a, Tp b, Tp c);
        Env& lengths(Tp a, Tp b, Tp c, Tp d);
        Env& lengths(Tp a, Tp b, Tp c, Tp d, Tp e);
    
        Tp totalLength() const;    
        Env& totalLength(Tp length, int modSegment);
        Env& totalLength(Tp length);
        Tp * curves();
        const Tp * curves() const;
        Env& curve(Tp v);        
        Env& curves(const Tp* vals, int len);        
        Env& curves(Tp ca, Tp cb);
        Env& curves(Tp ca, Tp cb, Tp cc);
        Env& curves(Tp ca, Tp cb, Tp cc, Tp cd);        
        Env& curve(Tp v);
        Env& segment(int i, Tp len, Tp crv);                    
        Env& segments(const Tv* lens, const Tv* crvs, int len, int begin=0);
        Env& segments(Tp la, Tp ca, Tp lb, Tp cb);
        Env& segments(Tp la, Tp ca, Tp lb, Tp cb, Tp lc, Tp cc);
        Env& segments(Tp la, Tp ca, Tp lb, Tp cb, Tp lc, Tp cc, Tp ld, Tp cd);
        Tv * levels();
        const Tv * levels() const;
        Env& levels(const Tv* vals, int len);
        Env& levels(Tv a, Tv b);
        Env& levels(Tv a, Tv b, Tv c);
        Env& levels(Tv a, Tv b, Tv c, Tv d);
        Env& levels(Tv a, Tv b, Tv c, Tv d, Tv e);        
        Env& maxLevel(Tv v);    
    };

    template <class Tv=real, class Tp=real, class Td=GAM_DEFAULT_DOMAIN>
    class AD : public Env<2,Tv,Tp,Td>{
    public:
        using Env<2,Tv,Tp,Td>::release;
        
        AD(Tp att =Tp(0.01), Tp dec =Tp(0.1), Tv amp = Tv(1), Tp crv =Tp(-4));
        AD& attack(Tp len);
        AD& decay(Tp len);
        AD& amp(Tv v);

        Tp attack() const;
        Tp decay() const;
        Tv amp() const;
    };


    template <class Tv=real, class Tp=real, class Td=GAM_DEFAULT_DOMAIN>
    class ADSR : public Env<3,Tv,Tp,Td>{
    public:
        using Env<3,Tv,Tp,Td>::release;
      
        ADSR(
            Tp att =Tp(0.01), Tp dec =Tp(0.1), Tv sus =Tv(0.7), Tp rel =Tp(1.),
            Tv amp =Tv( 1),
            Tp crv =Tp(-4)
        );
        
        ADSR& attack(Tp len);
        ADSR& decay(Tp len);

        ADSR& sustain(Tv val);
        ADSR& release(Tp len);
        ADSR& amp(Tv v);

        Tp attack() const;
        Tp decay() const;
        Tv sustain() const;
        Tp release() const;
        Tv amp() const;
    };


    template <class T=real, class Td=GAM_DEFAULT_DOMAIN>
    class Decay 
    public:

        Decay(T decay=T(1), T val=T(1));

        T decay() const;		///< Returns -60 dB decay length
        bool done(T thresh=T(0.001)) const; ///< Returns whether value is below threshold
        T value() const;		///< Returns current value

        %extend {
            T Tick() { return (*$self)(); }
        }
        void decay(T v);		///< Set number of units for curve to decay -60 dB

        void value(T v);		///< Set current value

        void reset(T amp=T(1));	///< Reset envelope and assign amplitude
        void finish(T amp=T(0.001)); ///< Jump to end of envelope

        void onDomainChange(double r);    
    };

    template <class T=real, class Td=GAM_DEFAULT_DOMAIN>
    class Gate : public Td{
    public:

        Gate(double closingDelay=0, double threshold=0.001);
        bool done() const;

        %extend {
            T Tick(const T& v) { return (*$self)(v); }
        }

        Gate& delay(double v);
    };


    template <
        class Tv=real,
        template <class> class Si=iplSeq::Linear,
        class Tp=real,
        class Td=GAM_DEFAULT_DOMAIN
    >
    class Seg 
    public:

        Seg(Tp len=0.5, Tv start=1, Tv end=0, Tp phase=0);

        bool done() const;
        Tv val() const;
        
        void operator= (Tv v);

        %extend {
            Tv Tick() { return (*$self)(); }
        }

        /*
        template <class Gen>
        Tv operator()(Gen& g){
            if(done()){
                mIpl.push(g());
                mAcc.val = mAcc.val - Tp(1); // wrap phase
            }
            Tp f = mAcc.val;
            mAcc();
            return mIpl(f);
        }
        */

       
        void freq(Tp v);
        void length(Tp v);
        void period(Tp v);
        void phase(Tp v);
        void reset();

        Si<Tv>& ipol();
        void onDomainChange(double /*r*/);
    };


    template <class T=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class SegExp {
    public:

        SegExp(T len, T crv=-3, T start=1, T end=0);
        bool done() const;

        %extend {
            T Tick() { return (*$self)(); }
        }

        void operator= (T v);
        void curve(T v);
        void period(T v);
        void reset();
        void set(T len, T crv)
        void onDomainChange(double r);
    };

}

%template<AD> gam::AD<SampleType,SampleType>;
%template<ADSR> gam::ADSR<SampleType,SampleType>;
%template<Decay> gam::Decay<SampleType>;
%template<Gate> gam::Gate<SampleType>;
%template<Seg> gam::Set<SampleType>;
%template<SegExp> gam::SetExp<SampleType>;