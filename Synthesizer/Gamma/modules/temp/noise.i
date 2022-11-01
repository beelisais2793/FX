%{
#include "Gamma/Noise.h"
%}

namespace gam
{
    template <class RNG>
    class NoiseBase{
    public:
        typedef float value_type;

        NoiseBase();        
        NoiseBase(uint32_t seed);
        void seed(uint32_t v){ rng = v; }
        
        RNG rng;    
    };


    template <class RNG = RNGLinCon>
    class NoiseBrown : public NoiseBase<RNG> {
    public:

        NoiseBrown(float val=0, float step=0.04, float min=-1, float max=1, uint32_t seedi=0);
    
        %extend {
            float Tick() { return (*$self)(); }
        }

        float val, step, min, max;
    };


    /// Pink Noise

    /// Pink noise has a power spectrum of 1/f. This corresponds to an amplitude
    /// spectrum with a -3 dB/octave slope.
    /// In this implementation, it is produced by summing together 12 octaves of
    /// downsampled white noise.
    /// \ingroup Noise
    template <class RNG = RNGLinCon>
    class NoisePink : public NoiseBase<RNG> {
    public:

        NoisePink();    
        NoisePink(uint32_t seed);        
        %extend {
            float Tick() { return (*$self)(); }
        }
    };


    /// White noise

    /// White noise has a uniform power spectrum.
    /// \ingroup Noise
    template <class RNG = RNGLinCon>
    class NoiseWhite : public NoiseBase<RNG> {
    public:
        %extend {
            float Tick() { return (*$self)(); }
        }
    };


    /// Violet noise

    /// Violet noise has a power spectrum of f^2. This corresponds to an amplitude
    /// spectrum with a +6 dB/octave slope. Here it is produced by differentiating
    /// white noise.
    /// \ingroup Noise
    template <class RNG = RNGLinCon>
    class NoiseViolet : public NoiseBase<RNG> {
    public:
        %extend {
            float Tick() { return (*$self)(); }
        }
    };


    /// Binary noise

    /// This noise flips randomly between -A and A. When A is very small, say 1e-20,
    /// then this can be added to the input of filters to prevent denormals.
    /// \ingroup Noise
    template <class RNG = RNGLinCon>
    class NoiseBinary : public NoiseBase<RNG> {
    public:

        /// \param[in] ampi		peak-to-peak amplitude of noise
        /// \param[in] seedi	random number generator seedi; 0 generates a random seed
        NoiseBinary(float ampi = 1.f, uint32_t seedi = 0);
        
        %extend {
            float Tick() { return (*$self)(); }
        }
        float amp;
    };
}

%template(NoiseBrown) gam::NoiseBrown<>;
%template(NoisePink) gam::NoisePink<>;
%template(NoiseWhite) gam::NoiseWhite<>;
%template(NoiseBinary) gam::NoiseBinary<>;
%template(NoiseViolet) gam::NoiseViolet<>;