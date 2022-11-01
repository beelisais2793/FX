
%{
#include "Gamma/DFT.h"
%}

namespace gam {

    /// Spectral data types
    enum SpectralType{
        COMPLEX,	/**< Complex number */
        MAG_PHASE,	/**< Magnitude and phase */
        MAG_FREQ	/**< Magnitude and frequency */
    };

    template <class T=gam::real>
    class SlidingWindow{
    public:
        SlidingWindow(unsigned winSize, unsigned hopSize);
        ~SlidingWindow();
        void resize(unsigned winSize, unsigned hopSize);
        void sizeHop(unsigned size);
        void sizeWin(unsigned size);
        unsigned sizeHop() const;
        unsigned sizeWin() const;
        const T * window();
        
        %extend {
            const T* Get() { return (*$self)(); }
            bool  Tick(T input) { return (*$self)(input); }
            bool  Process(T * dst, T input) { return (*$self)(dst,input); }
        }            
    };

    template <class T=gam::real, class Td=GAM_DEFAULT_DOMAIN>
    class DFTBase {
    public:
        DFTBase();
        virtual ~DFTBase();
        
        T * aux(unsigned num);
        Complex<T> * bins();
        const Complex<T> * bins() const;
        Complex<T> * begin();
        const Complex<T> * begin() const;
        Complex<T> * end();
        const Complex<T> * end() const;

        Complex<T>& bin(unsigned k);        
        const Complex<T>& bin(unsigned k) const;
        T * bufferInverse();
        const T * bufferInverse() const;
        double binFreq() const;		///< Get width of frequency bins
        unsigned numAux() const;	///< Get number of auxiliary buffers
        unsigned numBins() const;	///< Get number of frequency bins
        unsigned sizeDFT() const;	///< Get size of transform, in samples
        Domain& domainFreq();		///< Get frequency domain
        void numAux(unsigned num);
        void copyBinsToAux(unsigned binComp, unsigned auxNum);
        void copyAuxToBins(unsigned auxNum, unsigned binComp);
        void zero();				///< Zeroes internal frequency bins
        void zeroEnds();			///< Zeroes DC and Nyquist bins
        void zeroAux();				///< Zeroes all auxiliary buffers
        void zeroAux(unsigned num);	///< Zeroes an auxiliary buffer
        void onDomainChange(double r);
   };

    // needs dftbase, it won't recognize the class up there..
    class DFT : public DFTBase<float>{
    public:
        DFT(
            unsigned winSize=1024, unsigned padSize=0,
            SpectralType specType=COMPLEX,
            unsigned numAux=0
        );
        virtual ~DFT();

        DFT& spectrumType(SpectralType v);
        DFT& precise(bool whether);
        void resize(unsigned windowSize, unsigned padSize);
        float freqRes() const;
        float overlap() const;				///< Get transform overlap factor
        bool overlapping() const;			///< Whether the transform is overlapping
        unsigned sizeHop() const;			///< Get size of hop
        unsigned sizePad() const;			///< Get size of zero-padding
        unsigned sizeWin() const;			///< Get size of window
        Domain& domainHop();				///< Hop domain

        %extend {
            bool  Process(float input) { return (*$self)(input); }
            float Tick() { return (*$self)(); }
        }
        void forward(const float * src=0);	
        virtual void inverse(float * dst=0);
        bool inverseOnNext();
        void spctToRect();		// convert spectrum to rectangle format
        void spctToPolar();		// convert spectrum to polar format
        void onDomainChange(double r);
        void print(FILE * fp=stdout, const char * append="\n");
   };

    class STFT : public DFT {
    public:

        STFT(unsigned winSize=1024, unsigned hopSize=256, unsigned padSize=0,
            WindowType winType = RECTANGLE,
            SpectralType specType = COMPLEX,
            unsigned numAux=0
        );
       
        virtual ~STFT();

        bool operator()(float input);

        %extend {
            bool Tick(float input) { return (*$self)(input); }
        }

        void forward(const float * src=0);
        virtual void inverse(float * dst=0);
        void resize(unsigned winSize, unsigned padSize);
        STFT& inverseWindowing(bool v);        
        STFT& rotateForward(bool v);
        STFT& sizeHop(unsigned size);
        STFT& windowType(WindowType type);
        double unitsHop();
        float * phases();
        double * accumPhases();
        STFT& resetPhases();       
        void print(FILE * fp=stdout, const char * append="\n");	
    };

    template <class T>
    class SlidingDFT : public DFTBase<T> {
    public:

        SlidingDFT(unsigned sizeDFT, unsigned binLo, unsigned binHi);
        void forward(T input);
        SlidingDFT& interval(unsigned binLo, unsigned binHi);
        void resize(unsigned sizeDFT, unsigned binLo, unsigned binHi);
    };
}


%template(SlidingWindow) gam::SlidingWindow<SampleType>;
%template(SlidingDFT) gam::SlidingDFT<SampleType>;