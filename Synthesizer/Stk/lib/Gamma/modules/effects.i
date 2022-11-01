%{
#include "Gamma/Effects.h"
%}

namespace gam
{
    template <class Tp=real>
    class AM{
    public:

        AM(Tp modDepth = Tp(1));        
        void depth(Tp v);

        %extend {
            Tp Tick(Tp car, Tp mod) { return (*$self)(car,mod); }
        }
    };

    class Biquad3{
    public:
      
        Biquad3(float f0, float f1, float f2, float q=8, FilterType type=BAND_PASS);

        void freq(float f0, float f1, float f2);
            
        %extend {
            float Tick(float i0) { return (*$self)(i0); }
            gam::Biquad<>& get_bq0() { return $self->bq0; }
            gam::Biquad<>& get_bq1() { return $self->bq1; }
            gam::Biquad<>& get_bq2() { return $self->bq2; }
        }            

        Biquad<> bq0, bq1, bq2;
    };

    class Burst
    {
    public:
        Burst(float frq1=20000, float frq2=4000, float dec=0.1, float res=2);
        
        %extend {
            float bang() { return (*$self)(); }
            void  exec(float frq1, float frq2, float dec, float rst) { (*$self)(frq1,frq2,dec,rst); }
            gam::NoiseWhite<gam::RNGMulCon>& get_src() { return $self->src; }
            gam::Biquad<>& get_fil() { return $self->fil; }
            gam::Decay<float>& get_env() { return $self->env; }
        }
        void reset();

        float freq1, freq2, amp;
        NoiseWhite<RNGMulCon> src;
	    Biquad<> fil;
	    Decay<float> env;
    };

    template <class T=gam::real>
    class Chirp{
    public:

        Chirp(T freq1=220, T freq2=0, T decay60=0.2);
        
        T operator()(){
            if(env.value() > 0.0001f){
                T e = env();
                osc.freq(freq2 + (freq1 - freq2) * e);
                return osc() * e;
            }
            return 0.f;
        }
        
        %extend {
            T Tick() { return (*$self)(); }
            void Process(T f1, T f2, T d, bool doReset=false) { return (*$self)(f1,f2,d,doReset); }

            gam::Sine<T>& get_osc() { return $self->osc; }
            gam::Decay<T>& get_env() { return $self->env;}
            
        }

        void decay(T v);
        void freq(T start, T end);    
        void reset();
            
        Sine<T> osc;		///< Sine oscillator
	    Decay<T> env;		///< Envelope
        T freq1;			///< Start frequency
        T freq2;			///< End frequency
    };

    template <unsigned N, class T=gam::real> 
    class ChebyN{
    public:
        T c[N];			///< Harmonic coefficients
        ChebyN(const T& fundAmp = T(1));
        T operator()(T i0) const { return i0*c[0] + wet(i0); }
        %extend {
            T __getitem__(T i0) const { return (*$self)(i0); }
        }
        T wet(T i0) const;        
        unsigned size() const;
        T& coef(int i);
        ChebyN& set(const SampleType* weights);
        ChebyN& zero();
    };

    template <class T=gam::real>
    class Chorus{
    public:
        Chorus(float delay=0.0021, float depth=0.002, float freq=1, float ffd=0.9, float fbk=0.1);
        Chorus& maxDelay(float v);
        Chorus& delay(float v);
        Chorus& fbk(float v);
        Chorus& ffd(float v);
        Chorus& freq(float v);
        Chorus& depth(float v);

        %extend {
            T    Tick(const T &v) { return (*$self)(v); }
            void Process(const T & in, T& o1, T& o2) { return (*$self)(in,o1,o2); }
            void filter_stereo(const gam::Vec<2,SampleType> & v) { (*$self)(v); }
            void filter_samples(const T& i1, const T& i2, T & o1, T& o2) { (*$self)(i1,i2,o1,o2); }
            gam::Comb<T, ipl::Cubic>& get_comb1() { return $self->comb1; }
            gam::Comb<T, ipl::Cubic>& get_comb2() { return $self->comb2; }
            gam::CSine<double>& get_mod() { return $self->mod; }
        }                
        void modulate();   

        //Comb<T, ipl::Cubic> comb1, comb2;		///< Comb filters
	    CSine<double> mod;		     
    };

    template <class T=gam::real>
    class FreqShift{
    public:
        
        FreqShift(float shift=1);

        %extend {
            T Tick(T in) { return (*$self)(in); }
            gam::CSine<T> get_mod() { return $self->mod; }
            gam::Hilbert<T> get_hil() { return $self->hil; }
        }        
        /// Set frequency shift amount
        FreqShift& freq(T v);

        CSine<T> mod;
	    Hilbert<T> hil;
    };

    class MonoSynth{
    public:
        MonoSynth(float freq=440, float dur=0.8, float ctf1=1000, float ctf2=100, float res=3);
        
        %extend {
            float Tick() { return (*$self)(); }
            gam::Saw<float>& get_osc() { return $self->osc; }
            gam::Decay<float>& get_env() { return $self->env; }
            gam::OnePole<float>& get_opEnv() { return $self->opEnv; }
            gam::Biquad<>& get_filter() { return $self->filter; }
        }
        void freq(float v);    
        void reset();

        Saw<float> osc;
        Biquad<> filter;
        Decay<float> env;
        OnePole<float> opEnv;
        float ctf1, ctf2;
    };

    template <class T=gam::real>
    class Pan{
    public:
      
        Pan(T pos=0);

        %extend {
            gam::Vec<2,T> Tick(T in) { return (*$self)(in); }
            void filter(T in, SampleType & out1, SampleType & out2) { (*$self)(in,out1,out2); }
            void filter_sample(T in1, T in2, SampleType &out1, SampleType &out2) { (*$self)(in1,in2,out1,out2); }
        }
        
        void pos(T v);
        void pos(T v, T gain);
        void posL(T v);
    };

    class Pluck{
    public:
        Pluck(double freq=440, double decay=0.99);
        
        %extend {
            float Tick() { return (*$self)(); }
            float filter(float in) { return (*$self)(in); }
        }
        void reset(){ env.reset(); }
        void freq(float v){ comb.freq(v); }
        
        NoiseWhite<> noise;
        Decay<> env;
        Biquad<> fil;
        //Comb<> comb;
    };


    template <class T=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class Quantizer : public Td{
    public:
      
        Quantizer(double freq=2000, float stepAmt=0);
        
        void freq(double v);        
        void period(double v);
        void step(float v);
        
        %extend {
            T Tick(T in) { return (*$self)(in); }
        }
        virtual void onDomainChange(double r);
    };
}

%template(AM) gam::AM<SampleType>;
%template(Chirp) gam::Chirp<SampleType>;
%template(Cheby1) gam::ChebyN<1,SampleType>;
%template(Cheby2) gam::ChebyN<2,SampleType>;
%template(Cheby3) gam::ChebyN<3,SampleType>;
%template(Cheby4) gam::ChebyN<4,SampleType>;
%template(Cheby5) gam::ChebyN<5,SampleType>;
%template(Cheby6) gam::ChebyN<6,SampleType>;
%template(Cheby7) gam::ChebyN<7,SampleType>;
%template(Cheby8) gam::ChebyN<8,SampleType>;
%template(Cheby9) gam::ChebyN<9,SampleType>;
%template(Chorus) gam::Chorus<SampleType>;
%template(FreqShift) gam::FreqShift<SampleType>;
%template(Pan) gam::Pan<SampleType>;
%template(Quantizer) gam::Quantizer<SampleType>;