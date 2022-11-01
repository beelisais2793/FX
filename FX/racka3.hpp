#pragma once
#include <cmath>
#include <sndfile.h>
#include <samplerate.h>
#include <fftw3.h>

#define MAX_FILTER_STAGES 5
extern DspFloatType sampleRate;
extern DspFloatType invSampleRate;

const char * DATADIR = "presets/";
int error_num;

//globals
static const DspFloatType p2 = M_PI/2.0f;
static const DspFloatType fact3 = 0.148148148148148f; //can multiply by 1/fact3

#define D_PI 6.283185f
#define PI 3.141598f
#define LOG_10 2.302585f
#define LOG_2  0.693147f
#define LN2R 1.442695041f
#define CNST_E  2.71828182845905f
#define AMPLITUDE_INTERPOLATION_THRESHOLD 0.0001f
#define FF_MAX_VOWELS 6
#define FF_MAX_FORMANTS 12
#define FF_MAX_SEQUENCE 8

#define RND (rand()/(RAND_MAX+1.0))
#define RND1 (((DspFloatType) rand())/(((DspFloatType) RAND_MAX)+1.0f))
#define F2I(f,i) (i)=((f>0) ? ( (int)(f) ) :( (int)(f-1.0f) ))
#define dB2rap(dB) (DspFloatType)((expf((dB)*LOG_10/20.0f)))
#define rap2dB(rap) (DspFloatType)((20*log(rap)/LOG_10))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define INTERPOLATE_AMPLITUDE(a,b,x,size) ( (a) + ( (b) - (a) ) * (DspFloatType)(x) / (DspFloatType) (size) )
#define ABOVE_AMPLITUDE_THRESHOLD(a,b) ( ( 2.0f*fabs( (b) - (a) ) / ( fabs( (b) + (a) + 0.0000000001f) ) ) > AMPLITUDE_INTERPOLATION_THRESHOLD )
#define POLY 8
#define DENORMAL_GUARD 1e-18f	// Make it smaller until CPU problem re-appears
#define SwapFourBytes(data) ( (((data) >> 24) & 0x000000ff) | (((data) >> 8) & 0x0000ff00) | (((data) << 8) & 0x00ff0000) | (((data) << 24) & 0xff000000) )
#define D_NOTE          1.059463f
#define LOG_D_NOTE      0.057762f
#define D_NOTE_SQRT     1.029302f
#define MAX_PEAKS 8
#define MAX_ALIENWAH_DELAY 100
#define ATTACK  0.175f  //crossover time for reverse delay
#define MAX_DELAY 2	// Number of Seconds
#define MAXHARMS  8    // max number of harmonics available
#define MAX_PHASER_STAGES 12
#define MAX_CHORUS_DELAY 250.0f	//ms
#define LN2                       (1.0f)  //Uncomment for att/rel to behave more like a capacitor.
#define MUG_CORR_FACT  0.4f
//Crunch waveshaping constants
#define Thi		0.67f			//High threshold for limiting onset
#define Tlo		-0.65f			//Low threshold for limiting onset
#define Tlc		-0.6139445f		//Tlo + sqrt(Tlo/500)
#define Thc		0.6365834f	        //Thi - sqrt(Thi/600)
#define CRUNCH_GAIN	100.0f			//Typical voltage gain for most OD stompboxes
#define DIV_TLC_CONST   0.002f			// 1/300
#define DIV_THC_CONST	0.0016666f		// 1/600 (approximately)
//End waveshaping constants
#define D_FLANGE_MAX_DELAY	0.055f			// Number of Seconds  - 50ms corresponds to fdepth = 20 (Hz). Added some extra for padding
#define LFO_CONSTANT		9.765625e-04		// 1/(2^LOG_FMAX - 1)
#define LOG_FMAX		10.0f			//  -- This optimizes LFO sweep for useful range.
#define MINDEPTH		20.0f			// won't allow filter lower than 20Hz
#define MAXDEPTH		15000.0f		// Keeps delay greater than 2 samples at 44kHz SR
#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define CLOSING 4
#define ENV_TR 0.0001f
#define HARMONICS 11
#define REV_COMBS 8
#define REV_APS 4
#define MAX_SFILTER_STAGES 12

/*
static inline DspFloatType f_pow2(DspFloatType x)
{
        ls_pcast32 *px, tx, lx;
        DspFloatType dx;
        px = (ls_pcast32 *)&x; // store address of DspFloatType as long pointer
        tx.f = (x-0.5f) + (3<<22); // temporary value for truncation
        lx.i = tx.i - 0x4b400000; // integer power of 2
        dx = x - (DspFloatType)lx.i; // DspFloatType remainder of power of 2
        x = 1.0f + dx * (0.6960656421638072f + // cubic apporoximation of 2^x
                   dx * (0.224494337302845f +  // for x in the range [0, 1]
                   dx * (0.07944023841053369f)));
        (*px).i += (lx.i << 23); // add integer power of 2 to exponent
        return (*px).f;
}
*/
/*
#define P2a0  1.00000534060469
#define P2a1   0.693057900547259
#define P2a2   0.239411678986933
#define P2a3   0.0532229404911678
#define P2a4   0.00686649174914722
#include <math.h>
static inline DspFloatType f_pow2(DspFloatType x)
{
DspFloatType y,xx, intpow;
long xint = (int) fabs(ceil(x));
xx = x - ceil(x);
xint = xint<<xint;
if(x>0) intpow = (DspFloatType) xint;
else intpow = 1.0f;
y = intpow*(xx*(xx*(xx*(xx*P2a4 + P2a3) + P2a2) + P2a1) + P2a0);
return y;
}
*/


namespace RackFX {



    class Filter_
    {
    public:
        virtual ~ Filter_ ()  {};
        virtual void filterout (DspFloatType * smp) { };
        virtual void setfreq (DspFloatType frequency) {};
        virtual void setfreq_and_q (DspFloatType frequency, DspFloatType q_) { };
        virtual void setq (DspFloatType q_) { };
        virtual void setgain (DspFloatType dBgain) {};

        DspFloatType outgain;
    };

    class FilterParams
    {
    public:
        FilterParams (unsigned char Ptype_, unsigned char Pfreq, unsigned char Pq_);
        ~FilterParams ();

        void defaults ();

        void getfromFilterParams (FilterParams * pars);

        DspFloatType getfreq ();
        DspFloatType getq ();
        DspFloatType getfreqtracking (DspFloatType notefreq);
        DspFloatType getgain ();

        unsigned char Pcategory;	//Filter category (Analog/Formant/StVar)
        unsigned char Ptype;		// Filter type  (for analog lpf,hpf,bpf..)
        unsigned char Pfreq;		// Frequency (64-central frequency)
        unsigned char Pq;		// Q parameters (resonance or bandwidth)
        unsigned char Pstages;	//filter stages+1
        unsigned char Pfreqtrack;	//how the filter frequency is changing according the note frequency
        unsigned char Pgain;		//filter's output gain

        //Formant filter parameters
        unsigned char Pnumformants;	//how many formants are used
        unsigned char Pformantslowness;	//how slow varies the formants
        unsigned char Pvowelclearness;	//how vowels are kept clean (how much try to avoid "mixed" vowels)
        unsigned char Pcenterfreq, Poctavesfreq;	//the center frequency of the res. func., and the number of octaves

        struct {
            struct {
                unsigned char freq, amp, q;	//frequency,amplitude,Q
            } formants[FF_MAX_FORMANTS];
        } Pvowels[FF_MAX_VOWELS];


        unsigned char Psequencesize;	//how many vowels are in the sequence
        unsigned char Psequencestretch;	//how the sequence is stretched (how the input from filter envelopes/LFOs/etc. is "stretched")
        unsigned char Psequencereversed;	//if the input from filter envelopes/LFOs/etc. is reversed(negated)
        struct {
            unsigned char nvowel;	//the vowel from the position
        } Psequence[FF_MAX_SEQUENCE];


        void formantfilterH (int nvowel, int nfreqs, DspFloatType * freqs);	//used by UI


        bool changed;


        DspFloatType getcenterfreq ();
        DspFloatType getoctavesfreq ();
        DspFloatType getfreqpos (DspFloatType freq);
        DspFloatType getfreqx (DspFloatType x);
        DspFloatType getformantfreq (unsigned char freq);
        DspFloatType getformantamp (unsigned char amp);
        DspFloatType getformantq (unsigned char q);


    private:
        void defaults (int n);

        //stored default parameters
        unsigned char Dtype;
        unsigned char Dfreq;
        unsigned char Dq;
    };


    class Filter
    {
    public:
        Filter (FilterParams * pars);
        ~Filter ();
        void filterout (DspFloatType * smp);
        void setfreq (DspFloatType frequency);
        void setfreq_and_q (DspFloatType frequency, DspFloatType q_);
        void setq (DspFloatType q_);
        DspFloatType getrealfreq (DspFloatType freqpitch);

    private:
        unsigned char category;

        Filter_ * filter;
    };



    class EffectLFO
    {
    public:
        EffectLFO ();
        ~EffectLFO ();
        void effectlfoout (DspFloatType * outl, DspFloatType * outr);
        void updateparams ();
        int Pfreq;
        int Prandomness;
        int PLFOtype;
        int Pstereo;	//"64"=0
    private:
        DspFloatType getlfoshape (DspFloatType x);

        DspFloatType xl, xr;
        DspFloatType incx;
        DspFloatType ampl1, ampl2, ampr1, ampr2;	//necesar pentru "randomness"
        DspFloatType lfointensity;
        DspFloatType lfornd;
        int lfotype;

        //Lorenz Fractal parameters
        DspFloatType x0,y0,z0,x1,y1,z1,radius;
        DspFloatType h;
        DspFloatType a;
        DspFloatType b;
        DspFloatType c;
        DspFloatType scale;
        DspFloatType iperiod;
        DspFloatType ratediv;

        //Sample/Hold
        int holdflag;  //toggle left/right channel changes
        DspFloatType tca, tcb, maxrate;
        DspFloatType rreg, lreg, xlreg,xrreg, oldrreg, oldlreg;
    };


    // Just another RBJ Filter but has a few cool additional things

    class AnalogFilter:public Filter_
    {
    public:
        AnalogFilter (unsigned char Ftype, DspFloatType Ffreq, DspFloatType Fq,
                    unsigned char Fstages);
        ~AnalogFilter ();
        void filterout (DspFloatType * smp);
        DspFloatType filterout_s (DspFloatType smp);

        void setfreq (DspFloatType frequency);
        void setfreq_and_q (DspFloatType frequency, DspFloatType q_);
        void setq (DspFloatType q_);
        void setSR(unsigned int SR);

        void settype (int type_);
        void setgain (DspFloatType dBgain);
        void setstages (int stages_);
        void cleanup ();
        void reversecoeffs();

        DspFloatType H (DspFloatType freq);	//Obtains the response for a given frequency


    private:
        struct fstage {
            DspFloatType c1, c2;
        } x[MAX_FILTER_STAGES + 1], y[MAX_FILTER_STAGES + 1],
        oldx[MAX_FILTER_STAGES + 1], oldy[MAX_FILTER_STAGES + 1];

        void singlefilterout (DspFloatType * smp, fstage & x, fstage & y, DspFloatType * c,
                            DspFloatType * d);
        DspFloatType singlefilterout_s (DspFloatType smp, fstage & x, fstage & y, DspFloatType * c,
                                DspFloatType * d);

        void computefiltercoefs ();
        int type;			//The type of the filter (LPF1,HPF1,LPF2,HPF2...)
        int stages;			//how many times the filter is applied (0->1,1->2,etc.)
        int order;			//the order of the filter (number of poles)
        int needsinterpolation, firsttime;
        int abovenq;			//this is 1 if the frequency is above the nyquist
        int oldabovenq;		//if the last time was above nyquist (used to see if it needs interpolation)

        unsigned int iSAMPLE_RATE;


        DspFloatType freq;		//Frequency given in Hz
        DspFloatType q;			//Q factor (resonance or Q factor)
        DspFloatType gain;		//the gain of the filter (if are shelf/peak) filters


        DspFloatType c[3], d[3];		//coefficients

        DspFloatType oldc[3], oldd[3];	//old coefficients(used only if some filter paremeters changes very fast, and it needs interpolation)

        DspFloatType xd[3], yd[3];	//used if the filter is applied more times

        DspFloatType ifSAMPLE_RATE;

    };

    class FPreset
    {
    public:

        FPreset();
        ~FPreset();
        void ReadPreset(int eff, int num);
    };

    struct COMPLEXTYPE {
        DspFloatType a, b;
    };

    class delayline
    {
    public:
        delayline(DspFloatType maxdelay, int maxtaps_);	//construct the object with intended maximum delay time
        ~delayline();
        void cleanup();
        void set_averaging(DspFloatType tc_);	//use this if you want the time change averaging longer or shorter
        void set_mix(DspFloatType mix_);
        DspFloatType envelope();

        //Delay line simple use case is this:
        // mydelayed_sample = mydelayline->delay(input, delay_time, 0, 1, 0)
        DspFloatType delay(DspFloatType smps, DspFloatType time, int tap_, int touch, int reverse);	//interpolating delay
        DspFloatType delay_simple(DspFloatType smps, DspFloatType time, int tap_, int touch, int reverse);	//simple ring buffer
        //smps  - The current input sample
        //time  - amount of delay you want
        //mix   - for chorus or flanger how much of original to mix
        //tap_  - if multi-tap delay, this is the tap you want to access. Usually set touch=0
        //when accessing multiple taps after input.
        //touch  -set to zero if you want smps written to the delay line.  Set nonzero if you only want to read out of delay line
        //reverse -set to nonzero if you want to play the samples in the delay line backward.
        //Typically you want to apply an envelope to eliminate the click at wraparound from old to recent.
        //in this case, multiply the sample by the envelope:
        // myreversedelayedsample = mydelayline->delay(input, delay_time, 0, 1, 1) * mydelayline->envelope;

        DspFloatType get_phaser(DspFloatType smps, DspFloatType lfo, int tap_, int stg);	//Allows you to use phaser directly without delay line
        //smps  - input sample
        //lfo   - ranges from 0 to 1
        //tap   - allows multiple separate phasers with the same object
        //stg   - number of phase stages to process

    private:
        int zero_index;
        int tap, maxtaps;
        DspFloatType maxtime;
        long maxdelaysmps;
        int rvptr, distance;

        DspFloatType *avgtime, *time;	//keeping it from changing too quickly
        DspFloatType tconst, alpha, beta, mix, imix;	//don't allow change in delay time exceed 1 sample at a time

        int *newtime;
        int *oldtime;
        int *crossfade;
        DspFloatType *xfade, fadetime;
        DspFloatType *cur_smps;

        struct phasevars {
            DspFloatType yn1[4];
            DspFloatType xn1[4];
            DspFloatType gain[4];
            int stages;
        } *pstruct;

        DspFloatType phaser(DspFloatType fxn);
        DspFloatType lagrange(DspFloatType p0, DspFloatType p1, DspFloatType p2, DspFloatType p3, DspFloatType x_);
        DspFloatType spline(DspFloatType p0, DspFloatType p1, DspFloatType p2, DspFloatType p3, DspFloatType x_);

        struct tapvars {
            DspFloatType lvars[4];
            DspFloatType ivars[4];
            DspFloatType fracts[4];
        } *tapstruct;

        DspFloatType *ringbuffer;


    };

    //You can tweak how fast this routine executes here with a little sacrifice
    #define GOOD_TRIG

    #ifdef GOOD_TRIG
    #else
    #define FAST_TRIG
    #endif

    #if defined(GOOD_TRIG)
    #define FHT_SWAP(a,b,t) {(t)=(a);(a)=(b);(b)=(t);}
    #define TRIG_VARS                                                \
        int t_lam=0;
    #define TRIG_INIT(k,c,s)                                         \
        {                                                           \
        int i;                                                     \
        for (i=2 ; i<=k ; i++)                                     \
            {coswrk[i]=costab[i];sinwrk[i]=sintab[i];}             \
        t_lam = 0;                                                 \
        c = 1;                                                     \
        s = 0;                                                     \
        }
    #define TRIG_NEXT(k,c,s)                                         \
        {                                                           \
            int i,j;                                                \
            (t_lam)++;                                              \
            for (i=0 ; !((1<<i)&t_lam) ; i++);                      \
            i = k-i;                                                \
            s = sinwrk[i];                                          \
            c = coswrk[i];                                          \
            if (i>1)                                                \
                {                                                    \
                for (j=k-i+2 ; (1<<j)&t_lam ; j++);                 \
                j         = k - j;                                  \
                sinwrk[i] = halsec[i] * (sinwrk[i-1] + sinwrk[j]);  \
                coswrk[i] = halsec[i] * (coswrk[i-1] + coswrk[j]);  \
                }                                                    \
        }
    #define TRIG_RESET(k,c,s)
    #endif

    #if defined(FAST_TRIG)
    #define TRIG_VARS                                        \
        DspFloatType t_c,t_s;
    #define TRIG_INIT(k,c,s)                                 \
        {                                                    \
        t_c  = costab[k];                                   \
        t_s  = sintab[k];                                   \
        c    = 1;                                           \
        s    = 0;                                           \
        }
    #define TRIG_NEXT(k,c,s)                                 \
        {                                                    \
        DspFloatType t = c;                                         \
        c   = t*t_c - s*t_s;                                \
        s   = t*t_s + s*t_c;                                \
        }
    #define TRIG_RESET(k,c,s)
    #endif

    #define M_SQRT2_2   0.70710678118654752440084436210484
    #define M_SQRT22   2.0*0.70710678118654752440084436210484

    class fft_filter
    {
    public:
        //fft_filter (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType size);
        fft_filter();
        ~fft_filter ();
        void out (DspFloatType * smpsl, DspFloatType * smpr);
        void make_window(int n, DspFloatType* window);
        void load_impulse(int size, char* filename);
        void run_filter(DspFloatType* smps);
        void resample_impulse(int size, DspFloatType* ir);
        void cleanup ();
        DspFloatType outvolume;

        DspFloatType *efxoutl;
        DspFloatType *efxoutr;

        //makes linear indexing easier
        void realifft(int n, DspFloatType *real);
        void realfft(int n, DspFloatType *real);

    //  void interleaved_real_fft(int n, DspFloatType *real);
    //  void interleaved_real_ifft(int n, DspFloatType *real);

    private:
        void mayer_fft(int n, DspFloatType *real, DspFloatType *imag);
        void mayer_ifft(int n, DspFloatType *real, DspFloatType *imag);
        void mayer_fht(DspFloatType *fz, int n);
        void mayer_realifft(int n, DspFloatType *real);
        void mayer_realfft(int n, DspFloatType *real);

        static DspFloatType halsec[20];
        static DspFloatType costab[20];
        static DspFloatType sintab[20];
        static DspFloatType coswrk[20];
        static DspFloatType sinwrk[20];

    };



} // RackFX








