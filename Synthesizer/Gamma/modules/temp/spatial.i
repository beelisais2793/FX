%{
#include "Gamma/Spatial.h"
%}

namespace gam {
    
    template <typename T>
    class LoopGain{
    public:

        void gain(float v);
        void damping(float v);

        %extend {
            T Tick(T in) { return (*$self)(in); }
        }

        float gain() const;
    };


    
    template <typename T>
    class Loop1P{
    public:
    
        void gain(float v);
        void damping(float v);

        %extend {
            T Tick(T in) { return (*$self)(in); }
        }

        float gain() const;
    };


    /// One-pole/one-zero loop filter (low-pass)
    template <typename T>
    class Loop1P1Z{
    public:
        
        void gain(float v);
        void damping(float v);

        %extend {
            T Tick(T in) { return (*$self)(in); }
        }

        float gain() const;
    };



    template<
        typename Tv = gam::real,
        template<typename> class Si = ipl::Linear,
        template<typename> class LoopFilter = LoopGain,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class Echo : public Delay<Tv,Si,Td> {
    public:

        typedef Delay<Tv,Si,Td> Base;

        Echo();
        Echo(double delay);

        Echo& decay(float v);
        Echo& fbk(float v);
        Echo& damping(float v);

        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
        
        float decay() const;        
        LoopFilter<Tv>& loopFilter();
    };


    /// Echo with damped complex sinusoidal response
    template<
        typename Tv = gam::real,
        template<typename> class Si = ipl::Linear,
        template<typename> class LoopFilter = LoopGain,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class EchoCSine : public Delay<Complex<Tv>, Si, Td> {
    public:

        typedef Delay<Complex<Tv>, Si, Td> Base;

        EchoCSine();
        EchoCSine(double delay);
        EchoCSine& fbk(float amt, float ang=0);
        EchoCSine& gain(float amt, float ang=0);
        EchoCSine& decay(float units);
        EchoCSine& fbkFreq(float frq, float addCycle=0);
        
        %extend {

            Complex<Tv> Process(Tv real, Tv imag) { return (*$self)(real,imag); }
            Complex<Tv> Tick(Tv in) { return (*$self)(in); }
        }

        
        Complex<float> fbk() const;
        Complex<float> gain() const;
    };



    enum ReverbFlavor{
        JCREVERB,	// John Chowning (4-comb, 3-allpass)
        FREEVERB,	// Jezar's Freeverb (8-comb, 4-allpass)
    };

    template<
        typename Tv = gam::real,
        template<typename> class LoopFilter = Loop1P,
        template<typename> class Si = ipl::Trunc,
        class Td = GAM_DEFAULT_DOMAIN
    >
    class ReverbMS  {
    public:

        typedef std::vector<Echo<Tv, Si, LoopFilter, Domain1>> Combs;
        typedef std::vector<Comb<Tv, Si, float, Domain1>> Allpasses;

        ReverbMS();
        ReverbMS(ReverbFlavor flavor, unsigned offset=0);
        ReverbMS& resize(ReverbFlavor flavor, unsigned offset=0);

        ReverbMS& decay(float v);
        ReverbMS& damping(float v);


        %extend {
            Tv Tick(Tv in) { return (*$self)(in); }
        }
        
        float decay() const;

        Combs& combs();
        Allpasses& allpasses();
        void print() const;

    };

    template <int Ndest=2, class T = gam::real>
    class Dist
    {
    public:
        Dist(float maxDelay=0.2, float near=0.1, float far=10);
        Dist& near(float v);
        float near() const;

        Dist& far(float v);
        float far() const;
        
        Dist& speedOfSound(float v);
        float speedOfSound() const;

        
        Dist& dist(int dest, float d);
        Dist& dist(int dest, float x, float y, float z=0);
        
        
        Dist& dist(const Vec<Ndest,T>& d);        
        const Vec<Ndest,float>& dist() const;        
        Vec<Ndest, T> operator()(T in);

        %extend {
            Vec<Ndest,T> Tick(T in) { return (*$self)(in); }
        }

        
        const Delay<T>& delayLine() const;
    };

}

%template(LoopGain) gam::LoopGain<SampleType>;
%template(Loop1p) gam::Loop1P<SampleType>;
%template(Loop1p1z) gam::Loop1P1Z<SampleType>;
%template(Echo) gam::Echo<SampleType>;
%template(EchoCSine) gam::EchoCSine<SampleType>;
%template(ReverbMS) gam::ReverbMS<SampleType>;
%template(Dist) gam::Dist<2,SampleType>;