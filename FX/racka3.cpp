//////////////////////////////////////////////////////////////////////////////
// RackA3
//////////////////////////////////////////////////////////////////////////////

#include "racka3.hpp"
#include "Undenormal.hpp"
#include <cstdio>
#include <cstring>

using namespace RackFX;

int Wave_res_amount;
int Wave_up_q;
int Wave_down_q;

// I dont like this at all and will change it sometime later.
const DspFloatType SAMPLE_RATE = sampleRate;
const DspFloatType fSAMPLE_RATE = sampleRate;
const DspFloatType cSAMPLE_RATE = invSampleRate;

extern const int BufferSize;
const int   PERIOD	= BufferSize;
const DspFloatType fPERIOD = (DspFloatType)BufferSize;

static DspFloatType r__ratio[12];
static int pdata[50];

int reconota;
DspFloatType freqs[12];
DspFloatType lfreqs[12];
DspFloatType aFreq;

int note_active[POLY];
int rnote[POLY];
int gate[POLY];

//The below pow function really works & is good to 16 bits, but is it faster than math lib powf()???
//globals

static const DspFloatType a[5] = { 1.00000534060469, 0.693057900547259, 0.239411678986933, 0.0532229404911678, 0.00686649174914722 };
//lookup for positive powers of 2
static const DspFloatType pw2[25] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f, 65536.0f, 131072.0f, 262144.0f, 524288.0f, 1048576.0f, 2097152.0f, 4194304.0f, 8388608.0f, 16777216.0f};
//negative powers of 2, notice ipw2[0] will never be indexed.
static const DspFloatType ipw2[25] = {1.0, 5.0e-01, 2.5e-01, 1.25e-01, 6.25e-02, 3.125e-02, 1.5625e-02, 7.8125e-03, 3.90625e-03, 1.953125e-03, 9.765625e-04, 4.8828125e-04, 2.44140625e-04, 1.220703125e-04, 6.103515625e-05, 3.0517578125e-05, 1.52587890625e-05, 7.62939453125e-06, 3.814697265625e-06, 1.9073486328125e-06, 9.5367431640625e-07, 4.76837158203125e-07, 2.38418579101562e-07, 1.19209289550781e-07, 5.96046447753906e-08};

static inline DspFloatType
f_sin(DspFloatType x)
{

    DspFloatType y;  //function output
    DspFloatType tmp;
    bool sign;
    if ((x>D_PI) || (x<-D_PI)) x = fmod(x,D_PI);
    if (x < 0.0f) x+=D_PI;
    sign = 0;
    if(x>M_PI) {
        x = D_PI - x;
        sign = 1;
    }

    if (x <= p2) y = x - x*x*x*fact3;
    else {
        tmp = x - M_PI;
        y = -tmp + tmp*tmp*tmp*fact3;
    }

    if (sign) y = -y;

    return y;
}

static inline DspFloatType
f_cos(DspFloatType x_)
{
    return f_sin(p2 + x_);
}


static inline DspFloatType f_pow2(DspFloatType x)
{
    DspFloatType y = 0.0f;

    if(x >=24) return pw2[24];
    else if (x <= -24.0f) return ipw2[24];
    else {
        DspFloatType whole =  ceilf(x);
        int xint = (int) whole;
        x = x - whole;

        if (xint>=0) {
            y = pw2[xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        } else  {

            y = ipw2[-xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        }

        return y;
    }

}

#define f_exp(x) f_pow2(x * LN2R)


///////////////////////////////////////////////////////
// Filter Params
///////////////////////////////////////////////////////

FilterParams::FilterParams (unsigned char Ptype_, unsigned char Pfreq_,
                            unsigned char Pq_)
{

    // setpresettype("Pfilter");
    Dtype = Ptype_;
    Dfreq = Pfreq_;
    Dq = Pq_;

    changed = false;
    defaults ();
};

FilterParams::~FilterParams ()
{
};


void
FilterParams::defaults ()
{
    Ptype = Dtype;
    Pfreq = Dfreq;
    Pq = Dq;

    Pstages = 0;
    Pfreqtrack = 64;
    Pgain = 64;
    Pcategory = 0;

    Pnumformants = 3;
    Pformantslowness = 64;
    for (int j = 0; j < FF_MAX_VOWELS; j++) {
        defaults (j);
    };

    Psequencesize = 3;
    for (int i = 0; i < FF_MAX_SEQUENCE; i++)
        Psequence[i].nvowel = (unsigned char) i % FF_MAX_VOWELS;

    Psequencestretch = 40;
    Psequencereversed = 0;
    Pcenterfreq = 64;		//1 kHz
    Poctavesfreq = 64;
    Pvowelclearness = 64;
};

void
FilterParams::defaults (int n)
{
    int j = n;
    for (int i = 0; i < FF_MAX_FORMANTS; i++) {
        Pvowels[j].formants[i].freq = (unsigned char) (RND*127.0);	//some random freqs
        Pvowels[j].formants[i].q = 64;
        Pvowels[j].formants[i].amp = 127;
    };
};


/*
 * Get the parameters from other FilterParams
 */

void
FilterParams::getfromFilterParams (FilterParams * pars)
{
    defaults ();

    if (pars == NULL)
        return;

    Ptype = pars->Ptype;
    Pfreq = pars->Pfreq;
    Pq = pars->Pq;

    Pstages = pars->Pstages;
    Pfreqtrack = pars->Pfreqtrack;
    Pgain = pars->Pgain;
    Pcategory = pars->Pcategory;

    Pnumformants = pars->Pnumformants;
    Pformantslowness = pars->Pformantslowness;
    for (int j = 0; j < FF_MAX_VOWELS; j++) {
        for (int i = 0; i < FF_MAX_FORMANTS; i++) {
            Pvowels[j].formants[i].freq = pars->Pvowels[j].formants[i].freq;
            Pvowels[j].formants[i].q = pars->Pvowels[j].formants[i].q;
            Pvowels[j].formants[i].amp = pars->Pvowels[j].formants[i].amp;
        };
    };

    Psequencesize = pars->Psequencesize;
    for (int i = 0; i < FF_MAX_SEQUENCE; i++)
        Psequence[i].nvowel = pars->Psequence[i].nvowel;

    Psequencestretch = pars->Psequencestretch;
    Psequencereversed = pars->Psequencereversed;
    Pcenterfreq = pars->Pcenterfreq;
    Poctavesfreq = pars->Poctavesfreq;
    Pvowelclearness = pars->Pvowelclearness;
};



/*
 * Parameter control
 */
DspFloatType FilterParams::getfreq ()
{
    return (((DspFloatType)Pfreq / 64.0f - 1.0f) * 5.0f);
};

DspFloatType FilterParams::getq ()
{
    return (expf (powf ((DspFloatType) Pq / 127.0f, 2) * logf (1000.0f)) - 0.9f);
};

DspFloatType FilterParams::getfreqtracking (DspFloatType notefreq)
{
    return (logf (notefreq / 440.0f) * ((DspFloatType)Pfreqtrack - 64.0f) / (64.0f * LOG_2));
};

DspFloatType FilterParams::getgain ()
{
    return (((DspFloatType)Pgain / 64.0f - 1.0f) * 30.0f);	//-30..30dB
};

/*
 * Get the center frequency of the formant's graph
 */
DspFloatType FilterParams::getcenterfreq ()
{
    return (10000.0f * powf (10.0f, -(1.0f - (DspFloatType)Pcenterfreq / 127.0f) * 2.0f));
};

/*
 * Get the number of octave that the formant functions applies to
 */
DspFloatType FilterParams::getoctavesfreq ()
{
    return (0.25f + 10.0f * (DspFloatType)Poctavesfreq / 127.0f);
};

/*
 * Get the frequency from x, where x is [0..1]
 */
DspFloatType FilterParams::getfreqx (DspFloatType x)
{
    if (x > 1.0)
        x = 1.0f;
    DspFloatType
    octf = powf (2.0f, getoctavesfreq ());
    return (getcenterfreq () / sqrtf (octf) * powf (octf, x));
};

/*
 * Get the x coordinate from frequency (used by the UI)
 */
DspFloatType FilterParams::getfreqpos (DspFloatType freq)
{
    return ((logf (freq) -
             logf (getfreqx (0.0))) / logf (2.0f) / getoctavesfreq ());
};


/*
 * Get the freq. response of the formant filter
 */
void
FilterParams::formantfilterH (int nvowel, int nfreqs, DspFloatType * freqs)
{
    DspFloatType c[3], d[3];
    DspFloatType filter_freq, filter_q, filter_amp;
    DspFloatType omega, sn, cs, alpha;

    for (int i = 0; i < nfreqs; i++)
        freqs[i] = 0.0;

    //for each formant...
    for (int nformant = 0; nformant < Pnumformants; nformant++) {
        //compute formant parameters(frequency,amplitude,etc.)
        filter_freq = getformantfreq (Pvowels[nvowel].formants[nformant].freq);
        filter_q = getformantq (Pvowels[nvowel].formants[nformant].q) * getq ();
        if (Pstages > 0)
            filter_q =
                (filter_q > 1.0 ? powf (filter_q, 1.0f / ((DspFloatType)Pstages + 1)) : filter_q);

        filter_amp = getformantamp (Pvowels[nvowel].formants[nformant].amp);


        if (filter_freq <= (sampleRate / 2 - 100.0)) {
            omega = 2.0f * PI * filter_freq / sampleRate;
            sn = sinf (omega);
            cs = cosf (omega);
            alpha = sn / (2.0f * filter_q);
            DspFloatType tmp = 1.0f + alpha;
            c[0] = alpha / tmp * sqrtf (filter_q + 1.0f);
            c[1] = 0;
            c[2] = -alpha / tmp * sqrtf (filter_q + 1.0f);
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha) / tmp * (-1.0f);
        } else
            continue;


        for (int i = 0; i < nfreqs; i++) {
            DspFloatType freq = getfreqx ((DspFloatType)i / (DspFloatType) nfreqs);
            if (freq > sampleRate / 2) {
                for (int tmp = i; tmp < nfreqs; tmp++)
                    freqs[tmp] = 0.0;
                break;
            };
            DspFloatType fr = freq / sampleRate * PI * 2.0f;
            DspFloatType x = c[0], y = 0.0f;
            for (int n = 1; n < 3; n++) {
                x += cosf ((DspFloatType)n * fr) * c[n];
                y -= sinf ((DspFloatType)n * fr) * c[n];
            };
            DspFloatType h = x * x + y * y;
            x = 1.0f;
            y = 0.0;
            for (int n = 1; n < 3; n++) {
                x -= cosf ((DspFloatType)n * fr) * d[n];
                y += sinf ((DspFloatType)n * fr) * d[n];
            };
            h = h / (x * x + y * y);

            freqs[i] += powf (h, ((DspFloatType)Pstages + 1.0f) / 2.0f) * filter_amp;
        };
    };
    for (int i = 0; i < nfreqs; i++) {
        if (freqs[i] > 0.000000001f)
            freqs[i] = rap2dB (freqs[i]) + getgain ();
        else
            freqs[i] = -90.0f;
    };

};

/*
 * Transforms a parameter to the real value
 */
DspFloatType FilterParams::getformantfreq (unsigned char freq)
{
    DspFloatType
    result = getfreqx ((DspFloatType)freq / 127.0f);
    return (result);
};

DspFloatType FilterParams::getformantamp (unsigned char amp)
{
    DspFloatType
    result = powf (0.1f, (1.0f - (DspFloatType)amp / 127.0f) * 4.0f);
    return (result);
};

DspFloatType FilterParams::getformantq (unsigned char q)
{
    //temp
    DspFloatType
    result = powf (25.0f, ((DspFloatType)q - 32.0f) / 64.0f);
    return (result);
};

///////////////////////////////////////////////////////
// Filter Kaka
///////////////////////////////////////////////////////

Filter::Filter (FilterParams * pars)
{
    unsigned char Ftype = pars->Ptype;
    unsigned char Fstages = pars->Pstages;

    category = pars->Pcategory;

    switch (category) {
    case 1:
        filter = new FormantFilter (pars);
        break;
    case 2:
        filter = new SVFilter(Ftype, 1000.0f, pars->getq (), Fstages);
        filter->outgain = dB2rap (pars->getgain ());
        if (filter->outgain > 1.0f)
            filter->outgain = sqrtf (filter->outgain);
        break;
    default:
        filter = new AnalogFilter (Ftype, 1000.0f, pars->getq (), Fstages);
        if ((Ftype >= 6) && (Ftype <= 8))
            filter->setgain (pars->getgain ());
        else
            filter->outgain = dB2rap (pars->getgain ());
        break;
    };
};

Filter::~Filter ()
{
};

void
Filter::filterout (DspFloatType * smp)
{
    filter->filterout (smp);
};

void
Filter::setfreq (DspFloatType frequency)
{
    filter->setfreq (frequency);
};

void
Filter::setfreq_and_q (DspFloatType frequency, DspFloatType q_)
{
    filter->setfreq_and_q (frequency, q_);
};

void
Filter::setq (DspFloatType q_)
{
    filter->setq (q_);
};

DspFloatType Filter::getrealfreq (DspFloatType freqpitch)
{
    if ((category == 0) || (category == 2))
        return (powf (2.0f, freqpitch + 9.96578428f));	//log2(1000)=9.95748
    else
        return (freqpitch);
};

///////////////////////////////////////////////////////
// Analog Filter (RBJ)
///////////////////////////////////////////////////////
AnalogFilter::AnalogFilter (unsigned char Ftype, DspFloatType Ffreq, DspFloatType Fq,
                            unsigned char Fstages)
{

    iSAMPLE_RATE=SAMPLE_RATE;
    ifSAMPLE_RATE=fSAMPLE_RATE;


    stages = Fstages;
    for (int i = 0; i < 3; i++) {
        oldc[i] = 0.0;
        oldd[i] = 0.0;
        c[i] = 0.0;
        d[i] = 0.0;
    };
    type = Ftype;
    freq = Ffreq;
    q = Fq;
    gain = 1.0;
    if (stages >= MAX_FILTER_STAGES)
        stages = MAX_FILTER_STAGES;
    cleanup ();
    firsttime = 0;
    abovenq = 0;
    oldabovenq = 0;
    setfreq_and_q (Ffreq, Fq);
    firsttime = 1;
    d[0] = 0;			//this is not used
    outgain = 1.0;


};

AnalogFilter::~AnalogFilter ()
{
};

void
AnalogFilter::cleanup ()
{
    for (int i = 0; i < MAX_FILTER_STAGES + 1; i++) {
        x[i].c1 = 0.0;
        x[i].c2 = 0.0;
        y[i].c1 = 0.0;
        y[i].c2 = 0.0;
        oldx[i] = x[i];
        oldy[i] = y[i];
    };
    needsinterpolation = 0;


};

void
AnalogFilter::computefiltercoefs ()
{
    int zerocoefs = 0;		//this is used if the freq is too high
    DspFloatType tmp;
    DspFloatType omega, sn, cs, alpha, beta;

    //do not allow frequencies bigger than samplerate/2
    DspFloatType freq = this->freq;
    if (freq > (iSAMPLE_RATE / 2 - 500.0)) {
        freq = ifSAMPLE_RATE * .5f - 500.0f;
        zerocoefs = 1;
    };
    if (freq < 0.1)
        freq = 0.1f;
    //do not allow bogus Q
    if (q < 0.0)
        q = 0.0;
    DspFloatType tmpq, tmpgain;
    if (stages == 0) {
        tmpq = q;
        tmpgain = gain;
    } else {
        tmpq = (q > 1.0 ? powf (q, 1.0f /(DspFloatType) (stages + 1)) : q);
        tmpgain = powf (gain, 1.0f / (DspFloatType)(stages + 1));
    };

    //most of theese are implementations of
    //the "Cookbook formulae for audio EQ" by Robert Bristow-Johnson
    //The original location of the Cookbook is:
    //http://www.harmony-central.com/Computer/Programming/Audio-EQ-Cookbook.txt
    switch (type) {
    case 0:			//LPF 1 pole
        if (zerocoefs == 0)
            tmp = expf (-D_PI * freq / ifSAMPLE_RATE);
        else
            tmp = 0.0f;
        c[0] = 1.0f - tmp;
        c[1] = 0.0;
        c[2] = 0.0;
        d[1] = tmp;
        d[2] = 0.0;
        order = 1;
        break;
    case 1:			//HPF 1 pole
        if (zerocoefs == 0)
            tmp = expf (-D_PI * freq / ifSAMPLE_RATE);
        else
            tmp = 0.0f;
        c[0] = (1.0f + tmp) * .5f;
        c[1] = -(1.0f + tmp) * .5f;
        c[2] = 0.0f;
        d[1] = tmp;
        d[2] = 0.0f;
        order = 1;
        break;
    case 2:			//LPF 2 poles
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            alpha = sn / (2.0f * tmpq);
            tmp = 1 + alpha;
            c[0] = (1.0f - cs) * .5f / tmp;
            c[1] = (1.0f - cs) / tmp;
            c[2] = (1.0f - cs) * .5f / tmp;
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha) / tmp * (-1.0f);
        } else {
            c[0] = 1.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 3:			//HPF 2 poles
        if (zerocoefs == 0) {
            omega =  D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            alpha = sn / (2.0f * tmpq);
            tmp = 1.0f + alpha;
            c[0] = (1.0f + cs) / 2.0f / tmp;
            c[1] = -(1.0f + cs) / tmp;
            c[2] = (1.0f + cs) / 2.0f / tmp;
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha) / tmp * (-1.0f);
        } else {
            c[0] = 0.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 4:			//BPF 2 poles
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            alpha = sn / (2.0f * tmpq);
            tmp = 1.0f + alpha;
            c[0] = alpha / tmp * sqrtf (tmpq + 1.0f);
            c[1] = 0.0f;
            c[2] = -alpha / tmp * sqrtf (tmpq + 1.0f);
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha) / tmp * (-1.0f);
        } else {
            c[0] = 0.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 5:			//NOTCH 2 poles
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            alpha = sn / (2.0f * sqrtf (tmpq));
            tmp = 1.0f + alpha;
            c[0] = 1.0f / tmp;
            c[1] = -2.0f * cs / tmp;
            c[2] = 1.0f / tmp;
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha) / tmp * (-1.0f);
        } else {
            c[0] = 1.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 6:			//PEAK (2 poles)
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            tmpq *= 3.0f;
            alpha = sn / (2.0f * tmpq);
            tmp = 1.0f + alpha / tmpgain;
            c[0] = (1.0f + alpha * tmpgain) / tmp;
            c[1] = (-2.0f * cs) / tmp;
            c[2] = (1.0f - alpha * tmpgain) / tmp;
            d[1] = -2.0f * cs / tmp * (-1.0f);
            d[2] = (1.0f - alpha / tmpgain) / tmp * (-1.0f);
        } else {
            c[0] = 1.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 7:			//Low Shelf - 2 poles
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            tmpq = sqrtf (tmpq);
            alpha = sn / (2.0f * tmpq);
            beta = sqrtf (tmpgain) / tmpq;
            tmp = (tmpgain + 1.0f) + (tmpgain - 1.0f) * cs + beta * sn;

            c[0] =
                tmpgain * ((tmpgain + 1.0f) - (tmpgain - 1.0f) * cs +
                           beta * sn) / tmp;
            c[1] =
                2.0f * tmpgain * ((tmpgain - 1.0f) - (tmpgain + 1.0f) * cs) / tmp;
            c[2] =
                tmpgain * ((tmpgain + 1.0f) - (tmpgain - 1.0f) * cs -
                           beta * sn) / tmp;
            d[1] = -2.0f * ((tmpgain - 1.0f) + (tmpgain + 1.0f) * cs) / tmp * (-1.0f);
            d[2] =
                ((tmpgain + 1.0f) + (tmpgain - 1.0f) * cs - beta * sn) / tmp * (-1.0f);
        } else {
            c[0] = tmpgain;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    case 8:			//High Shelf - 2 poles
        if (zerocoefs == 0) {
            omega = D_PI * freq / ifSAMPLE_RATE;
            sn = sinf (omega);
            cs = cosf (omega);
            tmpq = sqrtf (tmpq);
            alpha = sn / (2.0f * tmpq);
            beta = sqrtf (tmpgain) / tmpq;
            tmp = (tmpgain + 1.0f) - (tmpgain - 1.0f) * cs + beta * sn;

            c[0] =
                tmpgain * ((tmpgain + 1.0f) + (tmpgain - 1.0f) * cs +
                           beta * sn) / tmp;
            c[1] =
                -2.0f * tmpgain * ((tmpgain - 1.0f) + (tmpgain + 1.0f) * cs) / tmp;
            c[2] =
                tmpgain * ((tmpgain + 1.0f) + (tmpgain - 1.0f) * cs -
                           beta * sn) / tmp;
            d[1] = 2.0f * ((tmpgain - 1.0f) - (tmpgain + 1.0f) * cs) / tmp * (-1.0f);
            d[2] =
                ((tmpgain + 1.0f) - (tmpgain - 1.0f) * cs - beta * sn) / tmp * (-1.0f);
        } else {
            c[0] = 1.0;
            c[1] = 0.0;
            c[2] = 0.0;
            d[1] = 0.0;
            d[2] = 0.0;
        };
        order = 2;
        break;
    default:			//wrong type
        type = 0;
        computefiltercoefs ();
        break;
    };
};


void
AnalogFilter::setfreq (DspFloatType frequency)
{
    if (frequency < 0.1)
        frequency = 0.1f;
    DspFloatType rap = freq / frequency;
    if (rap < 1.0)
        rap = 1.0f / rap;

    oldabovenq = abovenq;
    abovenq = frequency > (iSAMPLE_RATE / 2 - 500.0);

    int nyquistthresh = (abovenq ^ oldabovenq);


    if ((rap > 3.0) || (nyquistthresh != 0)) {
        //if the frequency is changed fast, it needs interpolation (now, filter and coeficients backup)
        for (int i = 0; i < 3; i++) {
            oldc[i] = c[i];
            oldd[i] = d[i];
        };
        for (int i = 0; i < MAX_FILTER_STAGES + 1; i++) {
            oldx[i] = x[i];
            oldy[i] = y[i];
        };
        if (firsttime == 0)
            needsinterpolation = 1;
    };
    freq = frequency;
    computefiltercoefs ();
    firsttime = 0;

};


void
AnalogFilter::setSR(unsigned int value)
{

    iSAMPLE_RATE = value;
    ifSAMPLE_RATE=(DspFloatType) iSAMPLE_RATE;
    computefiltercoefs ();

}


void
AnalogFilter::setfreq_and_q (DspFloatType frequency, DspFloatType q_)
{
    q = q_;
    setfreq (frequency);
};

void
AnalogFilter::setq (DspFloatType q_)
{
    q = q_;
    computefiltercoefs ();
};

void
AnalogFilter::settype (int type_)
{
    type = type_;
    computefiltercoefs ();
};

void
AnalogFilter::setgain (DspFloatType dBgain)
{
    gain = dB2rap(dBgain);
    computefiltercoefs ();
};

void
AnalogFilter::setstages (int stages_)
{
    if (stages_ >= MAX_FILTER_STAGES)
        stages_ = MAX_FILTER_STAGES - 1;
    stages = stages_;
    cleanup ();
    computefiltercoefs ();
};

void
AnalogFilter::singlefilterout (DspFloatType * smp, fstage & x, fstage & y,
                               DspFloatType * c, DspFloatType * d)
{
    int i;
    DspFloatType y0;
    if (order == 1) {
        //First order filter
        for (i = 0; i < PERIOD; i++) {

            y0 = smp[i] * c[0] + x.c1 * c[1] + y.c1 * d[1];
            y.c1 = y0 + DENORMAL_GUARD;
            x.c1 = smp[i];
            //output
            smp[i] = y0;
        };
    };
    if (order == 2) {
        //Second order filter
        for (i = 0; i < PERIOD; i++) {
            y0 =
                (smp[i] * c[0]) + (x.c1 * c[1]) + (x.c2 * c[2]) + (y.c1 * d[1]) +
                (y.c2 * d[2]);
            y.c2 = y.c1;
            y.c1 = y0 + DENORMAL_GUARD;
            x.c2 = x.c1;
            x.c1 = smp[i];
            //output
            smp[i] = y0;
        };
    };
};

void
AnalogFilter::filterout (DspFloatType * smp)
{
    int i;
    DspFloatType *ismp = NULL;	//used if it needs interpolation
    if (needsinterpolation != 0) {
        ismp = new DspFloatType[PERIOD];
        for (i = 0; i < PERIOD; i++)
            ismp[i] = smp[i];
        for (i = 0; i < stages + 1; i++)
            singlefilterout (ismp, oldx[i], oldy[i], oldc, oldd);
    };

    for (i = 0; i < stages + 1; i++)
        singlefilterout (smp, x[i], y[i], c, d);

    if (needsinterpolation != 0) {
        for (i = 0; i < PERIOD; i++) {
            DspFloatType x = (DspFloatType) i / fPERIOD;
            smp[i] = ismp[i] * (1.0f - x) + smp[i] * x;
        };
        delete (ismp);
        needsinterpolation = 0;
    };

};


DspFloatType
AnalogFilter::filterout_s(DspFloatType smp)
{
    int i;
    if (needsinterpolation != 0) {
        for (i = 0; i < stages + 1; i++)
            smp=singlefilterout_s(smp, oldx[i], oldy[i], oldc, oldd);
    }

    for (i = 0; i < stages + 1; i++)
        smp=singlefilterout_s(smp, x[i], y[i], c, d);

    return(smp);

};



DspFloatType
AnalogFilter::singlefilterout_s (DspFloatType smp, fstage & x, fstage & y,
                                 DspFloatType * c, DspFloatType * d)
{
    DspFloatType y0;
    if (order == 1) {
        //First order filter
        y0 = smp* c[0] + x.c1 * c[1] + y.c1 * d[1];
        y.c1 = y0;
        x.c1 = smp + DENORMAL_GUARD;
        //output
        smp = y0;

    };
    if (order == 2) {
        //Second order filter
        y0 =
            (smp* c[0]) + (x.c1 * c[1]) + (x.c2 * c[2]) + (y.c1 * d[1]) +
            (y.c2 * d[2]);
        y.c2 = y.c1;
        y.c1 = y0 + DENORMAL_GUARD;
        x.c2 = x.c1;
        x.c1 = smp;
        //output
        smp = y0;

    };

    return(smp);

};






void  AnalogFilter::reversecoeffs()
{
    DspFloatType tmpd1, tmpd2, tmpc0;
    tmpd1 = -1.0f * d[1];
    tmpd2 = -1.0f * d[2];

    tmpc0 = 10.0f * c[0];

    c[0] = tmpc0;
    d[1] = -1.0f * c[1]*tmpc0;
    d[2] = -1.0f * c[2]*tmpc0;
    c[1] = tmpd1*tmpc0;
    c[2] = tmpd2*tmpc0;


};

DspFloatType AnalogFilter::H (DspFloatType freq)
{
    DspFloatType
    fr = freq / ifSAMPLE_RATE * D_PI;
    DspFloatType
    x = c[0], y = 0.0;
    for (int n = 1; n < 3; n++) {
        x += cosf ((DspFloatType)n * fr) * c[n];
        y -= sinf ((DspFloatType)n * fr) * c[n];
    };
    DspFloatType
    h = x * x + y * y;
    x = 1.0;
    y = 0.0;
    for (int n = 1; n < 3; n++) {
        x -= cosf ((DspFloatType)n * fr) * d[n];
        y += sinf ((DspFloatType)n * fr) * d[n];
    };
    h = h / (x * x + y * y);
    return (powf (h, (DspFloatType)(stages + 1) / 2.0f));
};

///////////////////////////////////////////////////////
// LFO
///////////////////////////////////////////////////////


EffectLFO::EffectLFO ()
{
    xl = 0.0;
    xr = 0.0;
    Pfreq = 40;
    Prandomness = 0;
    PLFOtype = 0;
    Pstereo = 96;

    iperiod = fPERIOD/sampleRate;
    h = iperiod;
    a = 10.0f;
    b = 28.0f;
    c = 8.0f / 5.0f;
    scale = 1.0f/36.0f;
    ratediv = 0.1f;
    holdflag = 0;
    tca = iperiod/(iperiod + 0.02);  //20ms default
    tcb = 1.0f - tca;
    rreg = lreg = oldrreg = oldlreg = 0.0f;
    updateparams ();

    ampl1 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;
    ampl2 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;
    ampr1 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;
    ampr2 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;


};

EffectLFO::~EffectLFO ()
{
};


/*
 * Update the changed parameters
 */
void
EffectLFO::updateparams ()
{

    incx = (DspFloatType)Pfreq * fPERIOD / (sampleRate * 60.0f);

    if (incx > 0.49999999)
        incx = 0.499999999f;		//Limit the Frequency

    lfornd = (DspFloatType)Prandomness / 127.0f;
    if (lfornd < 0.0)
        lfornd = 0.0;
    else if (lfornd > 1.0)
        lfornd = 1.0;

    if (PLFOtype > 11)   //this has to be updated if more lfo's are added
        PLFOtype = 0;		
    lfotype = PLFOtype;

    xr = fmodf (xl + ((DspFloatType)Pstereo - 64.0f) / 127.0f + 1.0f, 1.0f);

    if ((h = incx*ratediv) > 0.02) h = 0.02;  //keeps it stable

    a = 10.0f + (((DspFloatType) RND) - 0.5f)*8.0f;
    b = 28.0f + (((DspFloatType) RND) - 0.5f)*12.0f;
    c = 1.25f + 3.0f * ((DspFloatType) RND);

// printf("incx %f x0 %f y0 %f z0 %f out %f c %f b %f a %f\n",incx,x0,y0,z0, (2.0f * radius - 1.0f), c, b, a);
    x0 = 0.1f + 0.1f * ((DspFloatType) RND);
    y0 = 0.0f;
    z0 = 0.2f;
    x1 = y1 = z1 = radius = 0.0f;

    DspFloatType tmp = 6.0f/((DspFloatType) Pfreq);  //S/H time attack  0.2*60=12.0
    tca = iperiod/(iperiod + tmp);  //
    tcb = 1.0f - tca;
    maxrate = 4.0f*iperiod;
};


/*
 * Compute the shape of the LFO
 */
DspFloatType EffectLFO::getlfoshape (DspFloatType x)
{
    DspFloatType tmpv;
    DspFloatType out=0.0;
    int iterations = 1;  //make fractal go faster
    switch (lfotype) {
    case 1:			//EffectLFO_TRIANGLE
        if ((x > 0.0) && (x < 0.25))
            out = 4.0f * x;
        else if ((x > 0.25) && (x < 0.75))
            out = 2.0f - 4.0f * x;
        else
            out = 4.0f * x - 4.0f;
        break;
    case 2:			//EffectLFO_RAMP Ramp+
        out = 2.0f * x - 1.0f;
        break;
    case 3:			//EffectLFO_RAMP Ramp-
        out = - 2.0f * x + 1.0f;
        break;
    case 4:                     //ZigZag
        x = x * 2.0f - 1.0f;
        tmpv = 0.33f * f_sin(x);
        out = f_sin(f_sin(x*D_PI)*x/tmpv);
        break;
    case 5:                     //Modulated Square ?? ;-)
        tmpv = x * D_PI;
        out=f_sin(tmpv+f_sin(2.0f*tmpv));
        break;
    case 6:                     // Modulated Saw
        tmpv = x * D_PI;
        out=f_sin(tmpv+f_sin(tmpv));
        break;
    case 8:                       //Lorenz Fractal, faster, using X,Y outputs
        iterations = 4;
    case 7:			// Lorenz Fractal
        for(int j=0; j<iterations; j++) {
            x1 = x0 + h * a * (y0 - x0);
            y1 = y0 + h * (x0 * (b - z0) - y0);
            z1 = z0 + h * (x0 * y0 - c * z0);
            x0 = x1;
            y0 = y1;
            z0 = z1;
        }
        if(lfotype==7) {
            if((radius = (sqrtf(x0*x0 + y0*y0 + z0*z0) * scale) - 0.25f)  > 1.0f) radius = 1.0f;
            if(radius < 0.0) radius = 0.0;
            out = 2.0f * radius - 1.0f;
        }

        break;
    case 9:                  //Sample/Hold Random
        if(fmod(x,0.5f)<=(2.0f*incx)) {          //this function is called by left, then right...so must toggle each time called
            rreg = lreg;
            lreg = RND1;

        }

        if(xlreg<lreg) xlreg += maxrate;
        else xlreg -= maxrate;
        if(xrreg<rreg) xrreg += maxrate;
        else xrreg -= maxrate;
        oldlreg = xlreg*tca + oldlreg*tcb;
        oldrreg = xrreg*tca + oldrreg*tcb;

        if(holdflag) {
            out = 2.0f*oldlreg -1.0f;
            holdflag = (1 + holdflag)%2;
        } else {
            out = 2.0f*oldrreg - 1.0f;
        }


        break;
    case 10:   //Tri-top
        if(x<=0.5f) out = -f_sin(x*D_PI);
        else if ((x > 0.5f) && (x < 0.75f))
        out = 6 * (x-0.5);
        else
        out = 1.5 - 6.0f *( x - 0.75f);
        out-=0.25f;
        out*=0.88888889f;
    break;
    case 11:  //Tri-Bottom
        if(x<=0.5f) out = -f_sin(x*D_PI);
        else if ((x > 0.5f) && (x < 0.75f))
        out = 6 * (x-0.5);
        else
        out = 1.5 - 6.0f *( x - 0.75f);
        out-=0.25f;
        out*=-0.88888889f;
    break; 
        //more to be added here; also ::updateparams() need to be updated (to allow more lfotypes)
    default:
        out = f_cos (x * D_PI);	//EffectLFO_SINE
    };
    return (out);
};

/*
 * LFO output
 */
void
EffectLFO::effectlfoout (DspFloatType * outl, DspFloatType * outr)
{
    DspFloatType out;

    out = getlfoshape (xl);
    //if ((lfotype == 0) || (lfotype == 1))         //What was that for?
    out *= (ampl1 + xl * (ampl2 - ampl1));
    xl += incx;
    if (xl > 1.0) {
        xl -= 1.0f;
        ampl1 = ampl2;
        ampl2 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;
    };
    if(lfotype==8) out = scale*x0;  //fractal parameter
    *outl = (out + 1.0f) * 0.5f;

    if(lfotype==8) out = scale*y0;  //fractal parameter
    else out = getlfoshape (xr);

    //if ((lfotype == 0) || (lfotype == 1))
    out *= (ampr1 + xr * (ampr2 - ampr1));
    xr += incx;
    if (xr > 1.0) {
        xr -= 1.0f;
        ampr1 = ampr2;
        ampr2 = (1.0f - lfornd) + lfornd * (DspFloatType)RND;
    };
    *outr = (out + 1.0f) * 0.5f;
};



///////////////////////////////////////////////////////
// Analog Phaser
///////////////////////////////////////////////////////

#define PHASER_LFO_SHAPE 2
#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Analog_Phaser::Analog_Phaser (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lxn1 = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES);

    lyn1 = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES);

    rxn1 = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES);

    ryn1 = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES);

    offset = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES);	//model mismatch between JFET devices
    offset[0] = -0.2509303f;
    offset[1] = 0.9408924f;
    offset[2] = 0.998f;
    offset[3] = -0.3486182f;
    offset[4] = -0.2762545f;
    offset[5] = -0.5215785f;
    offset[6] = 0.2509303f;
    offset[7] = -0.9408924f;
    offset[8] = -0.998f;
    offset[9] = 0.3486182f;
    offset[10] = 0.2762545f;
    offset[11] = 0.5215785f;

    barber = 0;  //Deactivate barber pole phasing by default

    mis = 1.0f;
    Rmin = 625.0f;	// 2N5457 typical on resistance at Vgs = 0
    Rmax = 22000.0f;	// Resistor parallel to FET
    Rmx = Rmin/Rmax;
    Rconst = 1.0f + Rmx;  // Handle parallel resistor relationship
    C = 0.00000005f;	     // 50 nF
    CFs = 2.0f*sampleRate*C;
    invperiod = 1.0f / fPERIOD;


    Ppreset = 0;
    setpreset (Ppreset);
    cleanup ();
};

Analog_Phaser::~Analog_Phaser ()
{
};


/*
 * Effect output
 */
void
Analog_Phaser::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j;
    DspFloatType lfol, lfor, lgain, rgain, bl, br, gl, gr, rmod, lmod, d, hpfr, hpfl;
    lgain = 0.0;
    rgain = 0.0;

    //initialize hpf
    hpfl = 0.0;
    hpfr = 0.0;

    lfo.effectlfoout (&lfol, &lfor);
    lmod = lfol*width + depth;
    rmod = lfor*width + depth;

    if (lmod > ONE_)
        lmod = ONE_;
    else if (lmod < ZERO_)
        lmod = ZERO_;
    if (rmod > ONE_)
        rmod = ONE_;
    else if (rmod < ZERO_)
        rmod = ZERO_;

    if (Phyper != 0) {
        lmod *= lmod;  //Triangle wave squared is approximately sin on bottom, tri on top
        rmod *= rmod;  //Result is exponential sweep more akin to filter in synth with exponential generator circuitry.
    };

    lmod = sqrtf(1.0f - lmod);  //gl,gr is Vp - Vgs. Typical FET drain-source resistance follows constant/[1-sqrt(Vp - Vgs)]
    rmod = sqrtf(1.0f - rmod);

    rdiff = (rmod - oldrgain) * invperiod;
    ldiff = (lmod - oldlgain) * invperiod;

    gl = oldlgain;
    gr = oldrgain;

    oldlgain = lmod;
    oldrgain = rmod;

    for (i = 0; i < PERIOD; i++) {

        gl += ldiff;	// Linear interpolation between LFO samples
        gr += rdiff;

        DspFloatType lxn = smpsl[i];
        DspFloatType rxn = smpsr[i];


        if (barber) {
            gl = fmodf((gl + 0.25f) , ONE_);
            gr = fmodf((gr + 0.25f) , ONE_);
        };


        //Left channel
        for (j = 0; j < Pstages; j++) {
            //Phasing routine
            mis = 1.0f + offsetpct*offset[j];
            d = (1.0f + 2.0f*(0.25f + gl)*hpfl*hpfl*distortion) * mis;  //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.
            Rconst =  1.0f + mis*Rmx;
            bl = (Rconst - gl )/ (d*Rmin);  // This is 1/R. R is being modulated to control filter fc.
            lgain = (CFs - bl)/(CFs + bl);

            lyn1[j] = lgain * (lxn + lyn1[j]) - lxn1[j];
            lyn1[j] += DENORMAL_GUARD;
            hpfl = lyn1[j] + (1.0f-lgain)*lxn1[j];  //high pass filter -- Distortion depends on the high-pass part of the AP stage.

            lxn1[j] = lxn;
            lxn = lyn1[j];
            if (j==1) lxn += fbl;  //Insert feedback after first phase stage
        };

        //Right channel
        for (j = 0; j < Pstages; j++) {
            //Phasing routine
            mis = 1.0f + offsetpct*offset[j];
            d = (1.0f + 2.0f*(0.25f + gr)*hpfr*hpfr*distortion) * mis;   // distortion
            Rconst =  1.0f + mis*Rmx;
            br = (Rconst - gr )/ (d*Rmin);
            rgain = (CFs - br)/(CFs + br);

            ryn1[j] = rgain * (rxn + ryn1[j]) - rxn1[j];
            ryn1[j] += DENORMAL_GUARD;
            hpfr = ryn1[j] + (1.0f-rgain)*rxn1[j];  //high pass filter

            rxn1[j] = rxn;
            rxn = ryn1[j];
            if (j==1) rxn += fbr;  //Insert feedback after first phase stage
        };

        fbl = lxn * fb;
        fbr = rxn * fb;
        efxoutl[i] = lxn;
        efxoutr[i] = rxn;

    };

    if (Poutsub != 0)
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] *= -1.0f;
            efxoutr[i] *= -1.0f;
        };

};

/*
 * Cleanup the effect
 */
void
Analog_Phaser::cleanup ()
{
    fbl = 0.0;
    fbr = 0.0;
    oldlgain = 0.0;
    oldrgain = 0.0;
    for (int i = 0; i < Pstages; i++) {
        lxn1[i] = 0.0;

        lyn1[i] = 0.0;

        rxn1[i] = 0.0;

        ryn1[i] = 0.0;

    };
};

/*
 * Parameter control
 */
void
Analog_Phaser::setwidth (int Pwidth)
{
    this->Pwidth = Pwidth;
    width = ((DspFloatType)Pwidth / 127.0f);
};


void
Analog_Phaser::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = (DspFloatType) (Pfb - 64) / 64.2f;
};

void
Analog_Phaser::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    // outvolume is needed in calling program
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
Analog_Phaser::setdistortion (int Pdistortion)
{
    this->Pdistortion = Pdistortion;
    distortion = (DspFloatType)Pdistortion / 127.0f;
};

void
Analog_Phaser::setoffset (int Poffset)
{
    this->Poffset = Poffset;
    offsetpct = (DspFloatType)Poffset / 127.0f;
};

void
Analog_Phaser::setstages (int Pstages)
{

    if (Pstages >= MAX_PHASER_STAGES)
        Pstages = MAX_PHASER_STAGES ;
    this->Pstages = Pstages;

    cleanup ();
};

void
Analog_Phaser::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = (DspFloatType)(Pdepth - 64) / 127.0f;  //Pdepth input should be 0-127.  depth shall range 0-0.5 since we don't need to shift the full spectrum.
};


void
Analog_Phaser::setpreset (int npreset)
{
    const int PRESET_SIZE = 13;
    const int NUM_PRESETS = 6;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Phaser1
        {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1},
        //Phaser2
        {64, 20, 14, 5, 1, 64, 110, 40, 6, 10, 0, 70, 1},
        //Phaser3
        {64, 20, 9, 0, 0, 64, 40, 40, 8, 10, 0, 60, 0},
        //Phaser4
        {64, 20, 14, 10, 0, 64, 110, 80, 7, 10, 1, 45, 1},
        //Phaser5
        {25, 20, 240, 10, 0, 64, 25, 16, 8, 100, 0, 25, 0},
        //Phaser6
        {64, 20, 1, 10, 1, 64, 110, 40, 12, 10, 0, 70, 1}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(18,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;
};


void
Analog_Phaser::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setdistortion (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        barber = 0;
        if (value == 2) barber = 1;
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setwidth (value);
        break;
    case 7:
        setfb (value);
        break;
    case 8:
        setstages (value);
        break;
    case 9:
        setoffset (value);
        break;
    case 10:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case 11:
        setdepth (value);
        break;
    case 12:
        if (value > 1)
            value = 1;
        Phyper = value;
        break;
    };
};

int
Analog_Phaser::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pdistortion);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pwidth);
        break;
    case 7:
        return (Pfb);
        break;
    case 8:
        return (Pstages);
        break;
    case 9:
        return (Poffset);
        break;
    case 10:
        return (Poutsub);
        break;
    case 11:
        return (Pdepth);
        break;
    case 12:
        return (Phyper);
        break;

    default:
        return (0);
    };

};


///////////////////////////////////////////////////////
// Alienwah
///////////////////////////////////////////////////////


Alienwah::Alienwah (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    Ppreset = 0;
    setpreset (Ppreset);
    cleanup ();
    oldclfol.a = fb;
    oldclfol.b = 0.0;
    oldclfor.a = fb;
    oldclfor.b = 0.0;

};

Alienwah::~Alienwah ()
{
};


/*
 * Apply the effect
 */
void
Alienwah::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType lfol, lfor;
    COMPLEXTYPE clfol, clfor, out, tmp;

    lfo.effectlfoout (&lfol, &lfor);
    lfol *= depth * D_PI;
    lfor *= depth * D_PI;
    clfol.a = cosf (lfol + phase) * fb;
    clfol.b = sinf (lfol + phase) * fb;
    clfor.a = cosf (lfor + phase) * fb;
    clfor.b = sinf (lfor + phase) * fb;

    for (i = 0; i < PERIOD; i++) {
        DspFloatType x = (DspFloatType)i / fPERIOD;
        DspFloatType x1 = 1.0f - x;
        //left
        tmp.a = clfol.a * x + oldclfol.a * x1;
        tmp.b = clfol.b * x + oldclfol.b * x1;

        out.a = tmp.a * oldl[oldk].a - tmp.b * oldl[oldk].b
                + (1.0f - fabsf (fb)) * smpsl[i] * panning;
        out.b = tmp.a * oldl[oldk].b + tmp.b * oldl[oldk].a;
        oldl[oldk].a = out.a;
        oldl[oldk].b = out.b;
        DspFloatType l = out.a * 10.0f * (fb + 0.1f);

        //right
        tmp.a = clfor.a * x + oldclfor.a * x1;
        tmp.b = clfor.b * x + oldclfor.b * x1;

        out.a = tmp.a * oldr[oldk].a - tmp.b * oldr[oldk].b
                + (1.0f - fabsf (fb)) * smpsr[i] * (1.0f - panning);
        out.b = tmp.a * oldr[oldk].b + tmp.b * oldr[oldk].a;
        oldr[oldk].a = out.a;
        oldr[oldk].b = out.b;
        DspFloatType r = out.a * 10.0f * (fb + 0.1f);


        if (++oldk >= Pdelay)
            oldk = 0;
        //LRcross
        efxoutl[i] = l * (1.0f - lrcross) + r * lrcross;
        efxoutr[i] = r * (1.0f - lrcross) + l * lrcross;
    };

    oldclfol.a = clfol.a;
    oldclfol.b = clfol.b;
    oldclfor.a = clfor.a;
    oldclfor.b = clfor.b;

};

/*
 * Cleanup the effect
 */
void
Alienwah::cleanup ()
{
    for (int i = oldpdelay; i < MAX_ALIENWAH_DELAY; i++) {
        oldl[i].a = 0.0f;
        oldl[i].b = 0.0f;
        oldr[i].a = 0.0f;
        oldr[i].b = 0.0f;

    };
    oldk = 0;
};


/*
 * Parameter control
 */

void
Alienwah::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = ((DspFloatType)Pdepth / 127.0f);
};

void
Alienwah::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = fabsf (((DspFloatType)Pfb - 64.0f) / 64.1f);
    fb = sqrtf (fb);
    if (fb < 0.4f)
        fb = 0.4f;
    if (Pfb < 64)
        fb = -fb;
};

void
Alienwah::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
Alienwah::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + .5f)/ 127.0f;
};

void
Alienwah::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f;
};

void
Alienwah::setphase (int Pphase)
{
    this->Pphase = Pphase;
    phase = ((DspFloatType)Pphase - 64.0f) / 64.0f * PI;
};

void
Alienwah::setdelay (int Pdelay)
{
    if (Pdelay > MAX_ALIENWAH_DELAY)
        Pdelay = MAX_ALIENWAH_DELAY;
    this->Pdelay = Pdelay;
    if(Pdelay>oldpdelay)  cleanup ();
    oldpdelay = Pdelay;
};

void
Alienwah::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 4;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //AlienWah1
        {64, 64, 80, 0, 0, 62, 60, 105, 25, 0, 64},
        //AlienWah2
        {64, 64, 95, 106, 0, 101, 60, 105, 17, 0, 64},
        //AlienWah3
        {64, 64, 55, 0, 1, 100, 112, 105, 31, 0, 42},
        //AlienWah4
        {64, 64, 1, 0, 1, 66, 101, 11, 47, 0, 86}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(11,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {


        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }


    Ppreset = npreset;
};


void
Alienwah::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setdepth (value);
        break;
    case 7:
        setfb (value);
        break;
    case 8:
        setdelay (value);
        break;
    case 9:
        setlrcross (value);
        break;
    case 10:
        setphase (value);
        break;
    };
};

int
Alienwah::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pdepth);
        break;
    case 7:
        return (Pfb);
        break;
    case 8:
        return (Pdelay);
        break;
    case 9:
        return (Plrcross);
        break;
    case 10:
        return (Pphase);
        break;
    default:
        return (0);
    };

};



///////////////////////////////////////////////////////
// Formant Filter
///////////////////////////////////////////////////////

FormantFilter::FormantFilter (FilterParams * pars)
{
    numformants = pars->Pnumformants;
    for (int i = 0; i < numformants; i++)
        formant[i] = new AnalogFilter (4 /*BPF*/, 1000.0f, 10.0f, pars->Pstages);
    cleanup ();
    inbuffer = new DspFloatType[PERIOD];
    tmpbuf = new DspFloatType[PERIOD];

    for (int j = 0; j < FF_MAX_VOWELS; j++)
        for (int i = 0; i < numformants; i++) {
            formantpar[j][i].freq =
                pars->getformantfreq (pars->Pvowels[j].formants[i].freq);
            formantpar[j][i].amp =
                pars->getformantamp (pars->Pvowels[j].formants[i].amp);
            formantpar[j][i].q =
                pars->getformantq (pars->Pvowels[j].formants[i].q);
        };
    for (int i = 0; i < FF_MAX_FORMANTS; i++)
        oldformantamp[i] = 1.0;
    for (int i = 0; i < numformants; i++) {
        currentformants[i].freq = 1000.0f;
        currentformants[i].amp = 1.0f;
        currentformants[i].q = 2.0f;
    };

    formantslowness = powf (1.0f - ((DspFloatType)pars->Pformantslowness / 128.0f), 3.0f);

    sequencesize = pars->Psequencesize;
    if (sequencesize == 0)
        sequencesize = 1;
    for (int k = 0; k < sequencesize; k++)
        sequence[k].nvowel = pars->Psequence[k].nvowel;

    vowelclearness = powf (10.0f, ((DspFloatType)pars->Pvowelclearness - 32.0f) / 48.0f);

    sequencestretch = powf (0.1f, ((DspFloatType)pars->Psequencestretch - 32.0f) / 48.0f);
    if (pars->Psequencereversed)
        sequencestretch *= -1.0f;

    outgain = dB2rap (pars->getgain ());

    oldinput = -1.0f;
    Qfactor = 1.0f;
    oldQfactor = Qfactor;
    firsttime = 1;
};

FormantFilter::~FormantFilter ()
{
    for (int i = 0; i < numformants; i++)
        delete (formant[i]);
    delete (inbuffer);
    delete (tmpbuf);
};




void
FormantFilter::cleanup ()
{
    for (int i = 0; i < numformants; i++)
        formant[i]->cleanup ();
};

void
FormantFilter::setpos (DspFloatType input)
{
    int p1, p2;

    if (firsttime != 0)
        slowinput = input;
    else
        slowinput = slowinput * (1.0f - formantslowness) + input * formantslowness;

    if ((fabsf (oldinput - input) < 0.001)
            && (fabsf (slowinput - input) < 0.001)
            && (fabsf (Qfactor - oldQfactor) < 0.001)) {
//      oldinput=input; daca setez asta, o sa faca probleme la schimbari foarte lente
        firsttime = 0;
        return;
    } else
        oldinput = input;


    DspFloatType pos = fmodf (input * sequencestretch, 1.0f);
    if (pos < 0.0)
        pos += 1.0f;

    F2I (pos * (DspFloatType)sequencesize, p2);
    p1 = p2 - 1;
    if (p1 < 0)
        p1 += sequencesize;

    pos = fmodf (pos * (DspFloatType)sequencesize, 1.0f);
    if (pos < 0.0)
        pos = 0.0f;
    else if (pos > 1.0)
        pos = 1.0f;
    pos =
        (atanf ((pos * 2.0f - 1.0f) * vowelclearness) / atanf (vowelclearness) +
         1.0f) * 0.5f;

    p1 = sequence[p1].nvowel;
    p2 = sequence[p2].nvowel;

    if (firsttime != 0) {
        for (int i = 0; i < numformants; i++) {
            currentformants[i].freq =
                formantpar[p1][i].freq * (1.0f - pos) +
                formantpar[p2][i].freq * pos;
            currentformants[i].amp =
                formantpar[p1][i].amp * (1.0f - pos) + formantpar[p2][i].amp * pos;
            currentformants[i].q =
                formantpar[p1][i].q * (1.0f - pos) + formantpar[p2][i].q * pos;
            formant[i]->setfreq_and_q (currentformants[i].freq,
                                       currentformants[i].q * Qfactor);
            oldformantamp[i] = currentformants[i].amp;
        };
        firsttime = 0;
    } else {
        for (int i = 0; i < numformants; i++) {
            currentformants[i].freq =
                currentformants[i].freq * (1.0f - formantslowness) +
                (formantpar[p1][i].freq * (1.0f - pos) +
                 formantpar[p2][i].freq * pos) * formantslowness;

            currentformants[i].amp =
                currentformants[i].amp * (1.0f - formantslowness) +
                (formantpar[p1][i].amp * (1.0f - pos) +
                 formantpar[p2][i].amp * pos) * formantslowness;

            currentformants[i].q =
                currentformants[i].q * (1.0f - formantslowness) +
                (formantpar[p1][i].q * (1.0f - pos) +
                 formantpar[p2][i].q * pos) * formantslowness;

            formant[i]->setfreq_and_q (currentformants[i].freq,
                                       currentformants[i].q * Qfactor);
        };
    };

    oldQfactor = Qfactor;
};

void
FormantFilter::setfreq (DspFloatType frequency)
{
    setpos (frequency);
};

void
FormantFilter::setq (DspFloatType q_)
{
    Qfactor = q_;
    for (int i = 0; i < numformants; i++)
        formant[i]->setq (Qfactor * currentformants[i].q);
};

void
FormantFilter::setfreq_and_q (DspFloatType frequency, DspFloatType q_)
{
    Qfactor = q_;
    setpos (frequency);
};


void
FormantFilter::filterout (DspFloatType * smp)
{
    int i, j;
    for (i = 0; i < PERIOD; i++) {
        inbuffer[i] = smp[i];
        smp[i] = 0.0;
    };

    for (j = 0; j < numformants; j++) {
        for (i = 0; i < PERIOD; i++)
            tmpbuf[i] = inbuffer[i] * outgain;
        formant[j]->filterout (tmpbuf);

        if (ABOVE_AMPLITUDE_THRESHOLD
                (oldformantamp[j], currentformants[j].amp))
            for (i = 0; i < PERIOD; i++)
                smp[i] +=
                    tmpbuf[i] * INTERPOLATE_AMPLITUDE (oldformantamp[j],
                                                       currentformants[j].amp, i,
                                                       PERIOD);
        else
            for (i = 0; i < PERIOD; i++)
                smp[i] += tmpbuf[i] * currentformants[j].amp;
        oldformantamp[j] = currentformants[j].amp;
    };
};


///////////////////////////////////////////////////////
// Filter Params
///////////////////////////////////////////////////////

SVFilter::SVFilter (unsigned char Ftype, DspFloatType Ffreq, DspFloatType Fq,
                    unsigned char Fstages)
{
    stages = Fstages;
    type = Ftype;
    freq = Ffreq;
    q = Fq;
    gain = 1.0f;
    outgain = 1.0f;
    needsinterpolation = 0;
    firsttime = 1;
    if (stages >= MAX_FILTER_STAGES)
        stages = MAX_FILTER_STAGES;
    cleanup ();
    setfreq_and_q (Ffreq, Fq);
};

SVFilter::~SVFilter ()
{
};

void
SVFilter::cleanup ()
{
    for (int i = 0; i < MAX_FILTER_STAGES + 1; i++) {
        st[i].low = 0.0;
        st[i].high = 0.0;
        st[i].band = 0.0;
        st[i].notch = 0.0;
    };
    oldabovenq = 0;
    abovenq = 0;
};

void
SVFilter::computefiltercoefs ()
{
    par.f = freq / fSAMPLE_RATE * 4.0f;
    if (par.f > 0.99999)
        par.f = 0.99999f;
    par.q = 1.0f - atanf (sqrtf (q)) * 2.0f / PI;
    par.q = powf (par.q, 1.0f / (DspFloatType)(stages + 1));
    par.q_sqrt = sqrtf (par.q);
};


void
SVFilter::setfreq (DspFloatType frequency)
{
    if (frequency < 0.1)
        frequency = 0.1f;
    DspFloatType rap = freq / frequency;
    if (rap < 1.0)
        rap = 1.0f / rap;

    oldabovenq = abovenq;
    abovenq = frequency > (fSAMPLE_RATE / 2.0f - 500.0f);

    int nyquistthresh = (abovenq ^ oldabovenq);


    if ((rap > 3.0) || (nyquistthresh != 0)) {
        //if the frequency is changed fast, it needs interpolation (now, filter and coeficients backup)
        if (firsttime == 0)
            needsinterpolation = 1;
        ipar = par;
    };
    freq = frequency;
    computefiltercoefs ();
    firsttime = 0;

};

void
SVFilter::setfreq_and_q (DspFloatType frequency, DspFloatType q_)
{
    q = q_;
    setfreq (frequency);
};

void
SVFilter::setq (DspFloatType q_)
{
    q = q_;
    computefiltercoefs ();
};

void
SVFilter::settype (int type_)
{
    type = type_;
    computefiltercoefs ();
};

void
SVFilter::setgain (DspFloatType dBgain)
{
    gain = dB2rap (dBgain);
    computefiltercoefs ();
};

void
SVFilter::setstages (int stages_)
{
    if (stages_ >= MAX_FILTER_STAGES)
        stages_ = MAX_FILTER_STAGES - 1;
    stages = stages_;
    cleanup ();
    computefiltercoefs ();
};

void
SVFilter::singlefilterout (DspFloatType * smp, fstage & x, parameters & par)
{
    int i;
    DspFloatType *out = NULL;
    switch (type) {
    case 0:
        out = &x.low;
        break;
    case 1:
        out = &x.high;
        break;
    case 2:
        out = &x.band;
        break;
    case 3:
        out = &x.notch;
        break;
    };

    for (i = 0; i < PERIOD; i++) {
        x.low = x.low + par.f * x.band;
        x.high = par.q_sqrt * smp[i] - x.low - par.q * x.band;
        x.band = par.f * x.high + x.band;
        x.notch = x.high + x.low;

        smp[i] = *out;
    };
};

void
SVFilter::filterout (DspFloatType * smp)
{
    int i;
    DspFloatType *ismp = NULL;

    if (needsinterpolation != 0) {
        ismp = new DspFloatType[PERIOD];
        for (i = 0; i < PERIOD; i++)
            ismp[i] = smp[i];
        for (i = 0; i < stages + 1; i++)
            singlefilterout (ismp, st[i], ipar);
    };

    for (i = 0; i < stages + 1; i++)
        singlefilterout (smp, st[i], par);

    if (needsinterpolation != 0) {
        for (i = 0; i < PERIOD; i++) {
            DspFloatType x = (DspFloatType) i / fPERIOD;
            smp[i] = ismp[i] * (1.0f - x) + smp[i] * x;
        };
        delete (ismp);
        needsinterpolation = 0;
    };

    for (i = 0; i < PERIOD; i++)
        smp[i] *= outgain;

};




FPreset::FPreset()
{
};

FPreset::~FPreset()
{
};

void
FPreset::ReadPreset(int eff, int num)
{

    FILE *fn;
    char tempfile[256];
    char buf[256];
    char *sbuf;
    int k=0;
    int reff=0;
    memset(tempfile,0,sizeof(tempfile));
    memset(pdata,0,sizeof(pdata));
    sprintf (tempfile, "%s%s", getenv ("HOME"), "/.rkrintpreset");
    if (( fn = fopen (tempfile, "r")) != NULL) {
        while (fgets (buf, sizeof buf, fn) != NULL)

        {
            sbuf = buf;
            sscanf(buf,"%d",&reff);
            if(reff==eff) k++;
            if(k==num) {
                strsep(&sbuf,",");
                strsep(&sbuf,",");
                sscanf(sbuf,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d.%d.%d\n",
                       &pdata[0],&pdata[1],&pdata[2],&pdata[3],&pdata[4],&pdata[5],&pdata[6],&pdata[7],&pdata[8],&pdata[9],
                       &pdata[10],&pdata[11],&pdata[12],&pdata[13],&pdata[14],&pdata[15],&pdata[16],&pdata[17],&pdata[18],&pdata[19],
                       &pdata[20],&pdata[21],&pdata[22],&pdata[23],&pdata[24],&pdata[25],&pdata[26],&pdata[27],&pdata[28],&pdata[29]);
                break;
            }
        }


        fclose(fn);
    }
}


Arpie::Arpie (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Pdelay = 60;
    Plrdelay = 100;
    Plrcross = 100;
    Pfb = 40;
    Phidamp = 60;
    Pharms = 3;
    Psubdiv = 1;

    ldelay = NULL;
    rdelay = NULL;
    lrdelay = 0;
    harmonic = 1;
    Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
    invattack = SAMPLE_RATE/15;
    envattack = 1.0f/(DspFloatType)invattack;
    maxx_delay = SAMPLE_RATE * MAX_DELAY;
    fade = SAMPLE_RATE / 10;    //200ms fade time available

    ldelay = new DspFloatType[maxx_delay];
    rdelay = new DspFloatType[maxx_delay];
    pattern = new int[MAXHARMS];

    setpreset (Ppreset);
    setpattern (0);
    cleanup ();
};

Arpie::~Arpie ()
{
};

/*
 * Cleanup the effect
 */
void
Arpie::cleanup ()
{
    memset(ldelay, 0 ,sizeof(DspFloatType)* maxx_delay);
    memset(rdelay, 0 ,sizeof(DspFloatType)* maxx_delay);
    oldl = 0.0;
    oldr = 0.0;
    rvkl = 0;
    rvkr = 0;
    kl = 0;
    kr = 0;
    harmonic = 1;
};


/*
 * Initialize the delays
 */
void
Arpie::initdelays ()
{
    int i;

    kl = 0;
    kr = 0;

    dl = delay - lrdelay;
    if (dl < 1)
        dl = 1;
    dr = delay + lrdelay;
    if (dr < 1)
        dr = 1;

    rvkl = 0;
    rvkr = 0;
    Srate_Attack_Coeff = 15.0f / (dl + dr);   // Set swell time to 1/10th of average delay time
    fade = (dl+dr)/5;

    for (i = dl; i < maxx_delay; i++)
        ldelay[i] = 0.0;
    for (i = dr; i < maxx_delay; i++)
        rdelay[i] = 0.0;
    oldl = 0.0;
    oldr = 0.0;

};

/*
 * Effect output
 */
void
Arpie::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType l, r, ldl, rdl, rswell, lswell;

    for (i = 0; i < PERIOD; i++) {
        ldl = ldelay[kl];
        rdl = rdelay[kr];
        l = ldl * (1.0f - lrcross) + rdl * lrcross;
        r = rdl * (1.0f - lrcross) + ldl * lrcross;
        ldl = l;
        rdl = r;


        ldl = smpsl[i] * panning - ldl * fb;
        rdl = smpsr[i] * (1.0f - panning) - rdl * fb;

        if(reverse > 0.0) {

            lswell =	(DspFloatType)(abs(kl - rvkl)) * Srate_Attack_Coeff;
            envswell = (1.0f - cosf(PI * envcnt*envattack));
            if (envswell > 1.0f) envswell = 1.0f;
            if (lswell <= PI) {
                lswell = 0.5f * (1.0f - cosf(lswell));  //Clickless transition
                efxoutl[i] = envswell * (reverse * (ldelay[rvkl] * lswell + ldelay[rvfl] * (1.0f - lswell))  + (ldl * (1-reverse)));   //Volume ducking near zero crossing.
            } else {
                efxoutl[i] = ((ldelay[rvkl] * reverse)  + (ldl * (1-reverse))) * envswell;
            }

            rswell = 	(DspFloatType)(abs(kr - rvkr)) * Srate_Attack_Coeff;
            if (rswell <= PI) {
                rswell = 0.5f * (1.0f - cosf(rswell));   //Clickless transition
                efxoutr[i] = envswell * (reverse * (rdelay[rvkr] * rswell + rdelay[rvfr] * (1.0f - rswell))  + (rdl * (1-reverse)));  //Volume ducking near zero crossing.
            } else {
                efxoutr[i] = envswell * ((rdelay[rvkr] * reverse)  + (rdl * (1-reverse)));
            }


        } else {
            efxoutl[i]= ldl;
            efxoutr[i]= rdl;
        }


        //LowPass Filter
        ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
        rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
        oldl = ldl + DENORMAL_GUARD;
        oldr = rdl + DENORMAL_GUARD;

        if (++envcnt >= invattack) envcnt = invattack;
        if (kl > (dl - invattack)) envcnt -=2;
        if (envcnt < 0) envcnt = 0;

        if (++kl >= dl) {
            kl = 0;
            envcnt = 0;
            if (++harmonic >= Pharms) harmonic = 0;
        }
        if (++kr >= dr)
            kr = 0;
        rvkl += pattern[harmonic];
        if (rvkl >= (dl )) rvkl = rvkl%(dl);
        rvkr += pattern[harmonic];
        if (rvkr >= (dr )) rvkr = rvkr%(dr);

        rvfl = rvkl + fade;
        if (rvfl >= (dl ))  rvfl = rvfl%(dl);
        rvfr = rvkr + fade;
        if (rvfr >= (dr )) rvfr = rvfr%(dr);
    };

};


/*
 * Parameter control
 */
void
Arpie::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Arpie::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
};

void
Arpie::setreverse (int Preverse)
{
    this->Preverse = Preverse;
    reverse = (DspFloatType) Preverse / 127.0f;
};

void
Arpie::setdelay (int Pdelay)
{
    this->Pdelay = Pdelay;
    if (Pdelay < 2) Pdelay = 2;
    if (Pdelay > 600) Pdelay = 600;	//100ms .. 2 sec constraint
    delay = 1 + lrintf ( (60.0f/((DspFloatType)(subdiv*Pdelay))) * fSAMPLE_RATE );	//quarter notes
    initdelays ();

};

void
Arpie::setlrdelay (int Plrdelay)
{
    DspFloatType tmp;
    this->Plrdelay = Plrdelay;
    tmp =
        (powf (2.0, fabsf ((DspFloatType)Plrdelay - 64.0f) / 64.0f * 9.0f) -
         1.0f) / 1000.0f * fSAMPLE_RATE;
    if (Plrdelay < 64.0)
        tmp = -tmp;
    lrdelay = lrintf(tmp);
    initdelays ();
};

void
Arpie::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f * 1.0f;
};

void
Arpie::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = (DspFloatType)Pfb / 128.0f;
};

void
Arpie::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 0.5f - (DspFloatType)Phidamp / 254.0f;
};


void
Arpie::setpattern (int Ppattern)
{
    this->Ppattern = Ppattern;

    const int PATTERN_SIZE = MAXHARMS;
    const int NUM_PATTERNS = 7;
    int setpatterns[NUM_PATTERNS][PATTERN_SIZE] = {
        {2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 6, 5, 4, 3, 2},
        {2, 4, 3, 5, 4, 6, 5, 7},
        {2, 2, 4, 3, 6, 2, 5, 3},
        {3, 2, 4, 3, 5, 4, 6, 5},
        {4, 3, 2, 7, 5, 3, 4, 2},
        {2, 3, 4, 5, 6, 7, 8, 9}
    };


    if (Ppattern >= PATTERN_SIZE)
        Ppattern = PATTERN_SIZE - 1;
    for (int ii = 0; ii < PATTERN_SIZE; ii++)
        pattern[ii] = setpatterns[Ppattern][ii];

};

void
Arpie::setpreset (int npreset)
{
    const int PRESET_SIZE = 9;
    const int NUM_PRESETS = 9;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Arpie 1
        {67, 64, 35, 64, 30, 59, 0, 127, 4},
        //Arpie 2
        {67, 64, 21, 64, 30, 59, 0, 64, 4},
        //Arpie 3
        {67, 75, 60, 64, 30, 59, 10, 0, 4},
        //Simple Arpie
        {67, 60, 44, 64, 30, 0, 0, 0, 4},
        //Canyon
        {67, 60, 102, 50, 30, 82, 48, 0, 4},
        //Panning Arpie 1
        {67, 64, 44, 17, 0, 82, 24, 0, 4},
        //Panning Arpie 2
        {81, 60, 46, 118, 100, 68, 18, 0, 4},
        //Panning Arpie 3
        {81, 60, 26, 100, 127, 67, 36, 0, 4},
        //Feedback Arpie
        {62, 64, 28, 64, 100, 90, 55, 0, 4}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(24,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Arpie::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setdelay (value);
        break;
    case 3:
        setlrdelay (value);
        break;
    case 4:
        setlrcross (value);
        break;
    case 5:
        setfb (value);
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        setreverse (value);
        break;
    case 8:
        Pharms = value;
        if ( (Pharms < 2) && (Pharms >= MAXHARMS)) {
            Pharms = 2;
        }
        break;
    case 9:
        setpattern(value);
        break;
    case 10:
        Psubdiv = value;
        subdiv = Psubdiv+1;
        setdelay (Pdelay);
        break;

    };
};

int
Arpie::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Pdelay);
        break;
    case 3:
        return (Plrdelay);
        break;
    case 4:
        return (Plrcross);
        break;
    case 5:
        return (Pfb);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return (Preverse);
        break;
    case 8:
        return (Pharms);
        break;
    case 9:
        return(Ppattern);
        break;
    case 10:
        return (Psubdiv);
        break;
    };
    return (0);			//in case of bogus parameter number
};



Chorus::Chorus ()
{

    dlk = 0;
    drk = 0;
    maxdelay = lrintf (MAX_CHORUS_DELAY / 1000.0 * SAMPLE_RATE);
    delayl = new DspFloatType[maxdelay];
    delayr = new DspFloatType[maxdelay];

    //int Ppreset = 0;
    //setpreset (0,Ppreset);

    DspFloatType tmp = 0.08f;
    ldelay = new delayline(tmp, 2);
    rdelay = new delayline(tmp, 2);
    ldelay -> set_averaging(0.005f);
    rdelay -> set_averaging(0.005f);
    ldelay->set_mix( 0.5f );
    rdelay->set_mix( 0.5f );

    oldr = 0.0f;
    oldl = 0.0f;
    awesome_mode = 0;

    lfo.effectlfoout (&lfol, &lfor);
    dl2 = getdelay (lfol);
    dr2 = getdelay (lfor);
    cleanup ();
};

Chorus::~Chorus ()
{
};

/*
 * get the delay value in samples; xlfo is the current lfo value
 */
DspFloatType Chorus::getdelay (DspFloatType xlfo)
{
    DspFloatType
    result;
    if (Pflangemode == 0) {
        result = (delay + xlfo * depth) * fSAMPLE_RATE;
    } else
        result = 0;

    //check if it is too big delay(caused bu errornous setdelay() and setdepth()
    if ((result + 0.5) >= maxdelay) {
        fprintf (stderr, "%s",
                 "WARNING: Chorus.C::getdelay(..) too big delay (see setdelay and setdepth funcs.)\n");
        printf ("%f %d\n", result, maxdelay);
        result = (DspFloatType) maxdelay - 1.0f;
    };
    return (result);
};

/*
 * Apply the effect
 */
void
Chorus::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType tmp;
    dl1 = dl2;
    dr1 = dr2;
    lfo.effectlfoout (&lfol, &lfor);

    if(awesome_mode) { //use interpolated delay line for better sound
        DspFloatType tmpsub;

        dl2 = delay + lfol * depth;
        dr2 = delay + lfor * depth;
        if (Poutsub != 0) tmpsub = -1.0f;
        else tmpsub = 1.0f;

        for (i = 0; i < PERIOD; i++) {
            //Left
            mdel = (dl1 * (DspFloatType)(PERIOD - i) + dl2 * (DspFloatType)i) / fPERIOD;
            tmp = smpsl[i] + oldl*fb;
            efxoutl[i] = tmpsub*ldelay->delay(tmp, mdel, 0, 1, 0);
            oldl = efxoutl[i];

            //Right
            mdel = (dr1 * (DspFloatType)(PERIOD - i) + dr2 * (DspFloatType)i) / fPERIOD;
            tmp = smpsr[i] + oldr*fb;
            efxoutr[i] = tmpsub*rdelay->delay(tmp, mdel, 0, 1, 0);
            oldr =  efxoutr[i];
        }

    } else {

        dl2 = getdelay (lfol);
        dr2 = getdelay (lfor);
        for (i = 0; i < PERIOD; i++) {
            DspFloatType inl = smpsl[i];
            DspFloatType inr = smpsr[i];
            //LRcross
            DspFloatType l = inl;
            DspFloatType r = inr;
            inl = l * (1.0f - lrcross) + r * lrcross;
            inr = r * (1.0f - lrcross) + l * lrcross;

            //Left channel

            //compute the delay in samples using linear interpolation between the lfo delays
            mdel = (dl1 * (DspFloatType)(PERIOD - i) + dl2 * (DspFloatType)i) / fPERIOD;
            if (++dlk >= maxdelay)
                dlk = 0;
            DspFloatType tmp = (DspFloatType) dlk - mdel + (DspFloatType)maxdelay * 2.0f;	//where should I get the sample from

            F2I (tmp, dlhi);
            dlhi %= maxdelay;

            dlhi2 = (dlhi - 1 + maxdelay) % maxdelay;
            dllo = 1.0f - fmodf (tmp, 1.0f);
            efxoutl[i] = delayl[dlhi2] * dllo + delayl[dlhi] * (1.0f - dllo);
            delayl[dlk] = inl + efxoutl[i] * fb;

            //Right channel

            //compute the delay in samples using linear interpolation between the lfo delays
            mdel = (dr1 * (DspFloatType)(PERIOD - i) + dr2 * (DspFloatType)i) / fPERIOD;
            if (++drk >= maxdelay)
                drk = 0;
            tmp = (DspFloatType)drk - mdel + (DspFloatType)maxdelay * 2.0f;	//where should I get the sample from

            F2I (tmp, dlhi);
            dlhi %= maxdelay;

            dlhi2 = (dlhi - 1 + maxdelay) % maxdelay;
            dllo = 1.0f - fmodf (tmp, 1.0f);
            efxoutr[i] = delayr[dlhi2] * dllo + delayr[dlhi] * (1.0f - dllo);
            delayr[dlk] = inr + efxoutr[i] * fb;

        };


        if (Poutsub != 0)
            for (i = 0; i < PERIOD; i++) {
                efxoutl[i] *= -1.0f;
                efxoutr[i] *= -1.0f;
            };


        for (int i = 0; i < PERIOD; i++) {
            efxoutl[i] *= panning;
            efxoutr[i] *= (1.0f - panning);
        };

    } //end awesome_mode test
};

/*
 * Cleanup the effect
 */
void
Chorus::cleanup ()
{
    for (int i = 0; i < maxdelay; i++) {
        delayl[i] = 0.0;
        delayr[i] = 0.0;
    };

};

/*
 * Parameter control
 */
void
Chorus::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = (powf (8.0f, ((DspFloatType)Pdepth / 127.0f) * 2.0f) - 1.0f) / 1000.0f;	//seconds
};

void
Chorus::setdelay (int Pdelay)
{
    this->Pdelay = Pdelay;
    delay = (powf (10.0f, ((DspFloatType)Pdelay / 127.0f) * 2.0f) - 1.0f) / 1000.0f;	//seconds
};

void
Chorus::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = ((DspFloatType)Pfb - 64.0f) / 64.1f;
};

void
Chorus::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    if(awesome_mode) { //use interpolated delay line for better sound
        outvolume = 0.0f;
        ldelay->set_mix( ((DspFloatType)Pvolume / 128.0f) );
        rdelay->set_mix( ((DspFloatType)Pvolume / 128.0f) );
    } else   outvolume = (DspFloatType)Pvolume / 127.0f;

};

void
Chorus::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning +.5f) / 127.0f;
};

void
Chorus::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f;
};

void Chorus::setpreset (int dgui, int npreset)
{
    const int PRESET_SIZE = 12;
    const int NUM_PRESETS = 10;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Chorus1
        {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0},
        //Chorus2
        {64, 64, 19, 0, 0, 98, 56, 90, 64, 19, 0, 0},
        //Chorus3
        {64, 64, 7, 0, 1, 42, 97, 95, 90, 127, 0, 0},
        //Celeste1
        {64, 64, 1, 0, 0, 42, 115, 18, 90, 127, 0, 0},
        //Celeste2
        {64, 64, 7, 117, 0, 50, 115, 9, 31, 127, 0, 1},
        //Flange1
        {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0},
        //Flange2
        {64, 64, 9, 34, 1, 40, 35, 3, 109, 0, 0, 0},
        //Flange3
        {64, 64, 31, 34, 1, 94, 35, 3, 54, 0, 0, 1},
        //Flange4
        {64, 64, 14, 0, 1, 62, 12, 19, 97, 0, 0, 0},
        //Flange5
        {64, 64, 34, 105, 0, 24, 39, 19, 17, 0, 0, 1}
    };


    if((dgui==0) && (npreset>4)) {
        Fpre->ReadPreset(5,npreset-4);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);

    } else if((dgui==1) && (npreset>9)) {
        Fpre->ReadPreset(7,npreset-9);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;


};


void
Chorus::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setdepth (value);
        break;
    case 7:
        setdelay (value);
        break;
    case 8:
        setfb (value);
        break;
    case 9:
        setlrcross (value);
        break;
    case 10:
        if (value > 1)
            value = 1;
        Pflangemode = value;
        break;
    case 11:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case 12:
        awesome_mode = value;
        if(awesome_mode) {
            outvolume = 0.0f;
            ldelay->set_mix(((DspFloatType)Pvolume/128.0f) );
            rdelay->set_mix(((DspFloatType)Pvolume/128.0f) );
        } else outvolume = (DspFloatType)Pvolume / 127.0f;
        break;
    };
};

int
Chorus::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pdepth);
        break;
    case 7:
        return (Pdelay);
        break;
    case 8:
        return (Pfb);
        break;
    case 9:
        return (Plrcross);
        break;
    case 10:
        return (Pflangemode);
        break;
    case 11:
        return (Poutsub);
        break;
    case 12:
        return (awesome_mode);
        break;
    default:
        return (0);
    };

};




delayline::delayline(DspFloatType maxdelay, int maxtaps_)
{
    maxtaps = maxtaps_;
    maxtime = fSAMPLE_RATE * maxdelay;
    maxdelaysmps = SAMPLE_RATE * lrintf(ceilf(maxdelay));
    ringbuffer = (DspFloatType *) malloc(sizeof(DspFloatType) * maxdelaysmps);
    avgtime = (DspFloatType *) malloc(sizeof(DspFloatType) * maxtaps);
    time = (DspFloatType *) malloc(sizeof(DspFloatType) * maxtaps);
    xfade = (DspFloatType *) malloc(sizeof(DspFloatType) * maxtaps);
    cur_smps = (DspFloatType *) malloc(sizeof(DspFloatType) * maxtaps);
    oldtime = (int *) malloc(sizeof(int) * maxtaps);
    newtime = (int *) malloc(sizeof(int) * maxtaps);
    crossfade = (int *) malloc(sizeof(int) * maxtaps);

    pstruct = (phasevars *) malloc(sizeof(struct phasevars) * maxtaps);
    tapstruct = (tapvars *) malloc(sizeof(struct tapvars) * maxtaps);

    zero_index = 0;
    tap = 0;
    rvptr = 0;
    distance = 0;

    mix = 0.5f;
    imix = 0.5f;

    DspFloatType dt = 1.0f / fSAMPLE_RATE;
    alpha = dt / (0.15f + dt);
    beta = 1.0f - alpha;	//time change smoothing parameters

    cleanup();
};

delayline::~delayline()
{

}

void
delayline::cleanup()
{
    zero_index = 0;
    int i, k;
    for (i = 0; i < maxdelaysmps; i++)
        ringbuffer[i] = 0.0;
    for (i = 0; i < maxtaps; i++) {
        avgtime[i] = 0.0;
        time[i] = 0.0;
        for (k = 0; k < 4; k++) {
            pstruct[i].yn1[k] = 0.0f;
            pstruct[i].xn1[k] = 0.0f;
            pstruct[i].gain[k] = 0.0f;
            tapstruct[i].lvars[k] = 0.0f;
            tapstruct[i].ivars[k] = 0.0f;
            tapstruct[i].fracts[k] = 0.0f;

        }
    }

    for (i = 0; i < maxtaps; i++) {
        avgtime[i] = 0.0f;
        newtime[i] = 0;
        oldtime[i] = 0;
        xfade[i] = 0.0f;
        crossfade[i] = 0;
        cur_smps[i] = 0.0f;

    }

    set_averaging(0.25f);

};


DspFloatType
delayline::delay_simple(DspFloatType smps, DspFloatType time_, int tap_, int touch,
                        int reverse)
{
    int dlytime = 0;
    int bufptr = 0;

    if (tap_ >= maxtaps)
        tap = 0;
    else
        tap = tap_;

    time[tap] = fSAMPLE_RATE * time_;	//convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
    if (time[tap] > maxtime)
        time[tap] = maxtime;
    dlytime = lrintf(time[tap]);

    if (crossfade[tap]) {
        xfade[tap] += fadetime;
        if (xfade[tap] >= 1.0f) {
            xfade[tap] = 0.0f;
            crossfade[tap] = 0;
            oldtime[tap] = newtime[tap];
            newtime[tap] = dlytime;
        }
    }

    if (crossfade[tap] == 0) {
        if (dlytime != oldtime[tap]) {
            crossfade[tap] = 1;
            xfade[tap] = 0.0f;
            oldtime[tap] = newtime[tap];
            newtime[tap] = dlytime;
        }

    }





    dlytime = newtime[tap];

//now put in the sample
    if (touch) {		//make touch zero if you only want to pull samples off the delay line
        ringbuffer[zero_index] = smps;
        if (--zero_index < 0)
            zero_index = maxdelaysmps - 1;
    }
//if we want reverse delay
//you need to call this every time to keep the buffers up to date, and it's on a different tap
    if (reverse) {

        bufptr = (dlytime + zero_index);	//this points to the sample we want to get
        if (bufptr >= maxdelaysmps)
            bufptr -= maxdelaysmps;
        if (++rvptr > maxdelaysmps)
            rvptr = 0;

        if (bufptr > zero_index) {
            if (rvptr > bufptr) {
                rvptr = zero_index;
                distance = 0;
            } else
                distance = rvptr - zero_index;
        } else if ((bufptr < zero_index) && (rvptr < zero_index)) {
            if (rvptr > bufptr) {
                rvptr = zero_index;
                distance = 0;
            } else
                distance =
                    rvptr + maxdelaysmps - zero_index;
        } else
            distance = rvptr - zero_index;


        bufptr = rvptr;	//this points to the sample we want to get

    } else {
        bufptr = (dlytime + zero_index);	//this points to the sample we want to get
        if (bufptr >= maxdelaysmps)
            bufptr -= maxdelaysmps;
    }

    int oldnewdiff = newtime[tap] - oldtime[tap];
    int tmpptr = 0;
    if (crossfade[tap] != 0) {
        tmpptr = bufptr + oldnewdiff;
        if (tmpptr >= maxdelaysmps)
            tmpptr -= maxdelaysmps;
        else if (tmpptr <= 0)
            tmpptr += maxdelaysmps;
        return (xfade[tap] * ringbuffer[bufptr] + (1.0f - xfade[tap]) * ringbuffer[tmpptr]);	//fade nicely to new tap
    } else
        return (ringbuffer[bufptr]);

};

/*
*  Interpolated delay line
*/

DspFloatType
delayline::delay(DspFloatType smps, DspFloatType time_, int tap_, int touch,
                 int reverse)
{
    int dlytime = 0;
    int bufptr = 0;

    tap = fabs(tap_);
    if (tap >= maxtaps)
        tap = 0;

    if (reverse)  avgtime[tap] = alpha * 2.0*time_ + beta * avgtime[tap];	//smoothing the rate of time change
    else avgtime[tap] = alpha * time_ + beta * avgtime[tap];	//smoothing the rate of time change
    time[tap] = 1.0f + fSAMPLE_RATE * avgtime[tap];	//convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
    if (time[tap] > maxtime)
        time[tap] = maxtime;
    if (time[tap] < 0.0f)
        time[tap] = 0.0f;

    DspFloatType fract = (time[tap] - floorf(time[tap]));	//compute fractional delay
    dlytime = lrintf(floorf(time[tap]));

//now put in the sample
    if (touch) {		//make touch zero if you only want to pull samples off the delay line
        cur_smps[tap] = ringbuffer[zero_index] = smps;
        if (--zero_index < 0)
            zero_index = maxdelaysmps - 1;
    }
//if we want reverse delay
//you need to call this every time to keep the buffers up to date, and it's on a different tap
    if (reverse) {

        bufptr = (dlytime + zero_index);	//this points to the sample we want to get
        if (bufptr >= maxdelaysmps)
            bufptr -= maxdelaysmps;
        if (++rvptr > maxdelaysmps)
            rvptr = 0;

        if (bufptr > zero_index) {
            if (rvptr > bufptr) {
                rvptr = zero_index;
                distance = 0;
            } else
                distance = rvptr - zero_index;
        } else if ((bufptr < zero_index) && (rvptr < zero_index)) {
            if (rvptr > bufptr) {
                rvptr = zero_index;
                distance = 0;
            } else
                distance =
                    rvptr + maxdelaysmps - zero_index;
        } else
            distance = rvptr - zero_index;

        bufptr = rvptr;	//this points to the sample we want to get

    } else {
        bufptr = (dlytime + zero_index);	//this points to the sample we want to get
        if (bufptr >= maxdelaysmps)
            bufptr -= maxdelaysmps;
    }

    tapstruct[tap].lvars[3] = tapstruct[tap].lvars[2];
    tapstruct[tap].lvars[2] = tapstruct[tap].lvars[1];
    tapstruct[tap].lvars[1] = tapstruct[tap].lvars[0];
    tapstruct[tap].lvars[0] = ringbuffer[bufptr];

    tapstruct[tap].ivars[3] = tapstruct[tap].ivars[2];
    tapstruct[tap].ivars[2] = tapstruct[tap].ivars[1];
    tapstruct[tap].ivars[1] = tapstruct[tap].ivars[0];
    tapstruct[tap].ivars[0] = cur_smps[tap];

    tapstruct[tap].fracts[3] = tapstruct[tap].fracts[2];
    tapstruct[tap].fracts[2] = tapstruct[tap].fracts[1];
    tapstruct[tap].fracts[1] = tapstruct[tap].fracts[0];
    tapstruct[tap].fracts[0] = fract;

    DspFloatType tmpfrac =
        0.5f * (tapstruct[tap].fracts[1] + tapstruct[tap].fracts[2]);
    //DspFloatType itmpfrac = 1.0f - tmpfrac;
    DspFloatType itmpfrac = 0.5f;  //it was the original approximation 

    DspFloatType output =
        mix * lagrange(tapstruct[tap].ivars[0],
                       tapstruct[tap].ivars[1],
                       tapstruct[tap].ivars[2],
                       tapstruct[tap].ivars[3],
                       itmpfrac) + imix * lagrange(tapstruct[tap].lvars[0],
                               tapstruct[tap].lvars[1],
                               tapstruct[tap].lvars[2],
                               tapstruct[tap].lvars[3],
                               tmpfrac);

    return (output);

};


inline DspFloatType
delayline::get_phaser(DspFloatType smps, DspFloatType lfo, int tap_, int stg)
{
    DspFloatType delta = lfo;
    if (delta > 1.0f)
        delta = 1.0f;
    if (delta < 0.0f)
        delta = 0.0f;
    tap = tap_;

    pstruct[tap].gain[0] = (1.0f - delta) / (1.0f + delta);
    pstruct[tap].stages = stg;

    return (phaser(smps));
};


inline DspFloatType
delayline::phaser(DspFloatType fxn)	//All-pass interpolation
{

    DspFloatType xn = fxn;
    for (int st = 0; st < pstruct[tap].stages; st++) {
        pstruct[tap].yn1[st] =
            pstruct[tap].xn1[st] - pstruct[tap].gain[st] * (xn +
                    pstruct
                    [tap].
                    yn1
                    [st]);
        pstruct[tap].xn1[st] = xn;
        xn = pstruct[tap].yn1[st];
    }

    return xn;

};

/*  Unfactored WYSIWYG implementation of order=4 Lagrange interpolation polynomial
inline DspFloatType
delayline::lagrange(DspFloatType p0, DspFloatType p1, DspFloatType p2, DspFloatType p3, DspFloatType x_)
{
DspFloatType x = x_;
DspFloatType xm2xm1 = (x - 1.0f)*(x - 2.0f);
x = -p0*x*xm2xm1*0.16666666667f + p1*(x + 1.0f)*xm2xm1*0.5f - p2*x*(x + 1.0f)*(x - 2.0f)*0.5f + p3*x*(x + 1.0f)*(x - 1.0f)*0.16666666667f;
return x;
};
*/

inline DspFloatType
delayline::lagrange(DspFloatType p0, DspFloatType p1, DspFloatType p2, DspFloatType p3, DspFloatType x_)
{
//factored version for less multiplies
    DspFloatType x = x_;

    const DspFloatType c0p0 = -0.16666666667f * p0;
    const DspFloatType c1p1 = 0.5f * p1;
    const DspFloatType c2p2 = -0.5f * p2;
    const DspFloatType c3p3 = 0.16666666667f * p3;

    const DspFloatType a = c3p3 + c2p2 + c1p1 + c0p0;
    const DspFloatType b = -3.0f * c0p0 - p1 - c2p2;
    const DspFloatType c = 2.0f * c0p0 - c1p1 + p2 - c3p3;
    const DspFloatType d = p1;

    x = ((a * x + b) * x + c) * x + d;
    return x;
};

inline DspFloatType
delayline::spline(DspFloatType p0, DspFloatType p1, DspFloatType p2, DspFloatType p3, DspFloatType x_)
{
//does not produce any better results than lagrange(), but has less multiplies
//seems to produce discontinuities on a low level (-48dB), so not a preferred algorithm

    DspFloatType x = x_;

    DspFloatType const c0 = p1;
    DspFloatType const c1 = 0.5f * (p2 - p0);
    DspFloatType const c2 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    DspFloatType const c3 = 0.5f * (p3 - p0) + 1.5f * (p1 - p2);

    return (((c3 * x + c2) * x + c1) * x + c0);
}

void delayline::set_averaging(DspFloatType tc_)
{
    DspFloatType tc = tc_;
    DspFloatType dt = 1.0f / fSAMPLE_RATE;
    fadetime = dt * tc;
    alpha = dt / (tc + dt);
    beta = 1.0f - alpha;	//time change smoothing parameters
};

void delayline::set_mix(DspFloatType mix_)	//mix amount of dry w/ wet
{
    mix = fabs(mix_);
    imix = 1.0f - mix;
    if(mix_<0.0f) imix*=-1.0f;
}

DspFloatType delayline::envelope()
{
    DspFloatType fdist = ((DspFloatType) distance) / time[tap];
    if (fdist > 0.5f) {
        if (fdist <= 1.0f)
            fdist = 1.0f - fdist;
        else
            fdist = 0.0f;
    }

    if (fdist <= 0.125f) {
        fdist =
            1.0f - f_sin(M_PI * fdist * 4.0f + 1.5707963267949f);
    } else
        fdist = 1.0f;
    return fdist;

};

HarmEnhancer::HarmEnhancer(DspFloatType *Rmag, DspFloatType hfreq, DspFloatType lfreq, DspFloatType gain)
{

    inputl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    inputr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);

    set_vol(0,gain);
    realvol = gain;
    itm1l = 0.0f;
    itm1r = 0.0f;
    otm1l = 0.0f;
    otm1r = 0.0f;

    hpffreq = hfreq;
    lpffreq = lfreq;
    hpfl = new AnalogFilter(3, hfreq, 1, 0);
    hpfr = new AnalogFilter(3, hfreq, 1, 0);
    lpfl = new AnalogFilter(2, lfreq, 1, 0);
    lpfr = new AnalogFilter(2, lfreq, 1, 0);

    limiter = new Compressor (inputl, inputr);
    limiter->Compressor_Change_Preset(0,4);
    calcula_mag(Rmag);
}


HarmEnhancer::~HarmEnhancer()
{
};

void
HarmEnhancer::cleanup()
{
    lpfl->cleanup ();
    hpfl->cleanup ();
    lpfr->cleanup ();
    hpfr->cleanup ();
    limiter->cleanup();

};




void
HarmEnhancer::set_vol(int mode, DspFloatType gain)
{
    if(!mode) vol = gain;
    else
        vol = realvol + gain;

    vol*=2.0f;
}

void
HarmEnhancer::set_freqh(int mode, DspFloatType freq)
{
    if(!mode) {
        hpffreq = freq;
        freq = 0.0;
    }

    hpfl->setfreq(hpffreq+freq);
    hpfr->setfreq(hpffreq+freq);

}


void
HarmEnhancer::set_freql(int mode, DspFloatType freq)
{
    if(!mode) {
        lpffreq = freq;
        freq = 0.0;
    }

    lpfl->setfreq(lpffreq+freq);
    lpfr->setfreq(lpffreq+freq);

}




/* Calculate Chebychev coefficents from partial magnitudes, adapted from
 * example in Num. Rec. */
void
HarmEnhancer::chebpc (DspFloatType c[], DspFloatType d[])
{
    int j,k;

    DspFloatType sv, dd[HARMONICS];

    for (j = 0; j < HARMONICS; j++) {
        d[j] = dd[j] = 0.0;
    }

    d[0] = c[HARMONICS - 1];

    for (j = HARMONICS - 2; j >= 1; j--) {
        for (k = HARMONICS - j; k >= 1; k--) {
            sv = d[k];
            d[k] = 2.0 * d[k - 1] - dd[k];
            dd[k] = sv;
        }
        sv = d[0];
        d[0] = -dd[0] + c[j];
        dd[0] = sv;
    }

    for (j = HARMONICS - 1; j >= 1; j--) {
        d[j] = d[j - 1] - dd[j];
    }
    d[0] = -dd[0] + 0.5 * c[0];



}



void
HarmEnhancer::calcula_mag (DspFloatType *Rmag)
{

    int i;
    DspFloatType mag_fix = 0.0f;

    DspFloatType mag[HARMONICS] = {
        0.0f, Rmag[0], Rmag[1], Rmag[2], Rmag[3], Rmag[4], Rmag[5],
        Rmag[6], Rmag[7], Rmag[8], Rmag[9]
    };



    // Normalise magnitudes

    for (i = 0; i < 10; i++)
        mag_fix += fabs (Rmag[i]);


    if (mag_fix < 1.0f) {
        mag_fix = 1.0f;
    } else {
        mag_fix = 1.0f / mag_fix;
    }
    for (i = 0; i < HARMONICS; i++) {
        mag[i] *= mag_fix;
    }

    // Calculate polynomial coefficients, using Chebychev aproximation
    chebpc (mag, p);

}

void
HarmEnhancer::harm_out(DspFloatType *smpsl, DspFloatType *smpsr)
{

    int i,j;

    memcpy(inputl,smpsl, sizeof(DspFloatType)*PERIOD);
    memcpy(inputr,smpsr, sizeof(DspFloatType)*PERIOD);



    hpfl->filterout(inputl);
    hpfr->filterout(inputr);

    limiter->out(inputl,inputr);

    for (i=0; i<PERIOD; i++) {
        DspFloatType xl = inputl[i];
        DspFloatType xr = inputr[i];
        DspFloatType yl=0.0f;
        DspFloatType yr=0.0f;

        for(j=10; j>0; j--) {
            yl = (yl+p[j])*xl;
            yr = (yr+p[j])*xr;
        }
        yl+=p[0];
        yr+=p[0];


        otm1l = 0.999f * otm1l + yl - itm1l;
        itm1l = yl;
        otm1r = 0.999f * otm1r + yr - itm1r;
        itm1r = yr;

        otm1l = yl;
        otm1r = yr;

        inputl[i] = otm1l;
        inputr[i] = otm1r;

    }

    lpfl->filterout(inputl);
    lpfr->filterout(inputr);

    for (i=0; i<PERIOD; i++) {
        smpsl[i] =(smpsl[i]+inputl[i]*vol);
        smpsr[i] =(smpsr[i]+inputr[i]*vol);
    }

}

CoilCrafter::CoilCrafter (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ptone = 20;
    att = 16.0f*powf(PI,fSAMPLE_RATE/44100.0f);

    tfreqs[0]=4000.0f;
    tfreqs[1]=4400.0f;
    tfreqs[2]=4200.0f;
    tfreqs[3]=2900.0f;
    tfreqs[4]=3000.0f;
    tfreqs[5]=2700.0f;
    tfreqs[6]=3300.0f;
    tfreqs[7]=3300.0f;
    tfreqs[8]=2800.0f;

    tqs[0]=4.2f;
    tqs[1]=4.2f;
    tqs[2]=2.3f;
    tqs[3]=1.8f;
    tqs[4]=2.2f;
    tqs[5]=2.1f;
    tqs[6]=1.7f;
    tqs[7]=1.7f;
    tqs[8]=1.8f;



    for(int i=0; i<10; i++) {
        rm[i]=0.0f;
    }
    rm[0]=1.0;
    rm[2]= -1.0;
    rm[4]=1.0;
    rm[6]=-1.0;
    rm[8]=1.0;


    harm = new HarmEnhancer (rm, 2500.0f,4400.0f,1.0f);

    RB1l =  new AnalogFilter(2,2000.0f,1.0f,0);
    RB1r =  new AnalogFilter(2,2000.0f,1.0f,0);
    RB2l =  new AnalogFilter(2,2000.0f,1.0f,0);
    RB2r =  new AnalogFilter(2,2000.0f,1.0f,0);


    cleanup ();

    setpreset (Ppreset);
};

CoilCrafter::~CoilCrafter ()
{
};

/*
 * Cleanup the effect
 */
void
CoilCrafter::cleanup ()
{
    harm->cleanup ();
    harm->calcula_mag(rm);
    harm->set_vol(1,1.0f);

    RB1l->cleanup();
    RB1r->cleanup();
    RB2l->cleanup();
    RB2r->cleanup();

};


/*
 * Effect output
 */
void
CoilCrafter::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;


    if(Ppo>0) {
        RB1l->filterout(smpsl);
        RB1r->filterout(smpsr);

        for (i=0; i<PERIOD; i++) {
            smpsl[i]*=att;
            smpsr[i]*=att;
        }

    }
    if(Ppd>0) {
        RB2l->filterout(smpsl);
        RB2r->filterout(smpsr);
    }

    if(Pmode) harm->harm_out(smpsl,smpsr);


    for (i=0; i<PERIOD; i++) {
        smpsl[i]*=outvolume;
        smpsr[i]*=outvolume;

        if(Pmode) {
            smpsl[i]*=.5f;
            smpsr[i]*=.5f;
        }
    }

};


/*
 * Parameter control
 */
void
CoilCrafter::setvolume (int value)
{
    Pvolume = value;
    outvolume = (1.0f + (DspFloatType)Pvolume) / 127.0f;

};

void
CoilCrafter::setfreq1()
{
    RB1l->setfreq(freq1);
    RB1l->reversecoeffs();
    RB1r->setfreq(freq1);
    RB1r->reversecoeffs();
}

void
CoilCrafter::setq1()
{
    RB1l->setq(q1);
    RB1l->reversecoeffs();
    RB1r->setq(q1);
    RB1r->reversecoeffs();
}

void
CoilCrafter::setfreq2()
{

    RB2l->setfreq(freq2);
    RB2r->setfreq(freq2);
}

void
CoilCrafter::setq2()
{

    RB2l->setq(q2);
    RB2r->setq(q2);



}

void
CoilCrafter::sethpf (int value)
{
    harm->set_freqh (0,(DspFloatType)Ptone);
};


void
CoilCrafter::setpreset (int npreset)
{
    const int PRESET_SIZE = 9;
    const int NUM_PRESETS = 2;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //H to S
        {32, 6, 1, 3300, 16,  4400, 42, 20, 0},
        //S to H
        {32, 1, 6, 4400, 42, 3300, 16, 20, 0},


    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(33,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;
    cleanup ();

};


void
CoilCrafter::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        Ppo = value;
        if (Ppo>0) {
            freq1 = tfreqs[value];
            Pfreq1 = (int)freq1;
            setfreq1();
            q1 = tqs[value];
            Pq1 = (int)(q1*10.0f);
            setq1();
        }
        break;
    case 2:
        Ppd = value;
        if(Ppd>0) {
            freq2 = tfreqs[value];
            Pfreq2 = (int)freq2;
            setfreq2();
            q2 = tqs[value];
            Pq2 =(int)(q2*10.0f);
            setq2();
        }
        break;
    case 3:
        Pfreq1 = value;
        freq1 = (DspFloatType) value;
        setfreq1();
        break;
    case 4:
        Pq1 = value;
        q1 = (DspFloatType)value/10.0f;
        setq1();
        break;
    case 5:
        Pfreq2 = value;
        freq2 = (DspFloatType) value;
        setfreq2();
        break;
    case 6:
        Pq2 = value;
        q2 = (DspFloatType)value/10.0f;
        setq2();
        break;
    case 7:
        Ptone = value;
        sethpf(value);
        break;
    case 8:
        Pmode = value;
        break;

    };
};

int
CoilCrafter::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppo);
        break;
    case 2:
        return (Ppd);
        break;
    case 3:
        return (Pfreq1);
        break;
    case 4:
        return (Pq1);
        break;
    case 5:
        return (Pfreq2);
        break;
    case 6:
        return (Pq2);
        break;
    case 7:
        return (Ptone);
        break;
    case 8:
        return (Pmode);
        break;

    };
    return (0);			//in case of bogus parameter number
};


#define  MIN_GAIN  0.00001f        // -100dB  This will help prevent evaluation of denormal numbers

Compressor::Compressor (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    rvolume = 0.0f;
    rvolume_db = 0.0f;
    lvolume = 0.0f;
    lvolume_db = 0.0f;
    tthreshold = -24;
    tratio = 4;
    toutput = -10;
    tatt = 20;
    trel = 50;
    a_out = 1;
    stereo = 0;
    tknee = 30;
    rgain = 1.0f;
    rgain_old = 1.0f;
    lgain = 1.0f;
    lgain_old = 1.0f;
    lgain_t = 1.0f;
    rgain_t = 1.0f;
    ratio = 1.0;
    kpct = 0.0f;
    peak = 0;
    lpeak = 0.0f;
    rpeak = 0.0f;
    rell = relr = attr = attl = 1.0f;

    ltimer = rtimer = 0;
    hold = (int) (SAMPLE_RATE*0.0125);  //12.5ms
    clipping = 0;
    limit = 0;


}

Compressor::~Compressor ()
{
}


void
Compressor::cleanup ()
{

    lgain = rgain = 1.0f;
    lgain_old = rgain_old = 1.0f;
    rpeak = 0.0f;
    lpeak = 0.0f;
    limit = 0;
    clipping = 0;
}


void
Compressor::Compressor_Change (int np, int value)
{

    switch (np) {

    case 1:
        tthreshold = value;
        thres_db = (DspFloatType)tthreshold;    //implicit type cast int to DspFloatType
        break;

    case 2:
        tratio = value;
        ratio = (DspFloatType)tratio;
        break;

    case 3:
        toutput = value;
        break;

    case 4:
        tatt = value;
        att = cSAMPLE_RATE /(((DspFloatType)value / 1000.0f) + cSAMPLE_RATE);
        attr = att;
        attl = att;
        break;

    case 5:
        trel = value;
        rel = cSAMPLE_RATE /(((DspFloatType)value / 1000.0f) + cSAMPLE_RATE);
        rell = rel;
        relr = rel;
        break;

    case 6:
        a_out = value;
        break;

    case 7:
        tknee = value;  //knee expressed a percentage of range between thresh and zero dB
        kpct = (DspFloatType)tknee/100.1f;
        break;

    case 8:
        stereo = value;
        break;
    case 9:
        peak = value;
        break;


    }

    kratio = logf(ratio)/LOG_2;  //  Log base 2 relationship matches slope
    knee = -kpct*thres_db;

    coeff_kratio = 1.0 / kratio;
    coeff_ratio = 1.0 / ratio;
    coeff_knee = 1.0 / knee;

    coeff_kk = knee * coeff_kratio;


    thres_mx = thres_db + knee;  //This is the value of the input when the output is at t+k
    makeup = -thres_db - knee/kratio + thres_mx/ratio;
    makeuplin = dB2rap(makeup);
    if (a_out)
        outlevel = dB2rap((DspFloatType)toutput) * makeuplin;
    else
        outlevel = dB2rap((DspFloatType)toutput);

}

int
Compressor::getpar (int np)
{

    switch (np)

    {

    case 1:
        return (tthreshold);
        break;
    case 2:
        return (tratio);
        break;
    case 3:
        return (toutput);
        break;
    case 4:
        return (tatt);
        break;
    case 5:
        return (trel);
        break;
    case 6:
        return (a_out);
        break;
    case 7:
        return (tknee);
        break;
    case 8:
        return (stereo);
        break;
    case 9:
        return (peak);
        break;
    }

    return (0);

}


void
Compressor::Compressor_Change_Preset (int dgui, int npreset)
{

    const int PRESET_SIZE = 10;
    const int NUM_PRESETS = 7;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //2:1
        {-30, 2, -6, 20, 120, 1, 0, 0, 0},
        //4:1
        {-26, 4, -8, 20, 120, 1, 10, 0, 0},
        //8:1
        {-24, 8, -12, 20, 35, 1, 30, 0, 0},
        //Final Limiter
        {-1, 15, 0, 5, 250, 0 ,0 ,1 ,1},
        //HarmonicEnhancer
        {-20, 15, -3, 5, 50, 0 ,0 ,1 ,1},
        //Band CompBand
        {-3, 2, 0, 5, 50, 1, 0, 1, 0},
        //End CompBand
        {-60, 2, 0, 10, 500, 1, 0, 1, 1},


    };

    if((dgui)&&(npreset>2)) {
        Fpre->ReadPreset(1,npreset-2);
        for (int n = 1; n < PRESET_SIZE; n++)
            Compressor_Change (n , pdata[n-1]);

    } else {
        for (int n = 1; n < PRESET_SIZE; n++)
            Compressor_Change (n , presets[npreset][n-1]);
    }

}



void
Compressor::out (DspFloatType *efxoutl, DspFloatType *efxoutr)
{

    int i;


    for (i = 0; i < PERIOD; i++) {
        DspFloatType rdelta = 0.0f;
        DspFloatType ldelta = 0.0f;
//Right Channel

        if(peak) {
            if (rtimer > hold) {
                rpeak *= 0.9998f;   //The magic number corresponds to ~0.1s based on T/(RC + T),
                rtimer--;
            }
            if (ltimer > hold) {
                lpeak *= 0.9998f;	//leaky peak detector.
                ltimer --;  //keeps the timer from eventually exceeding max int & rolling over
            }
            ltimer++;
            rtimer++;
            if(rpeak<fabs(efxoutr[i])) {
                rpeak = fabs(efxoutr[i]);
                rtimer = 0;
            }
            if(lpeak<fabs(efxoutl[i])) {
                lpeak = fabs(efxoutl[i]);
                ltimer = 0;
            }

            if(lpeak>20.0f) lpeak = 20.0f;
            if(rpeak>20.0f) rpeak = 20.0f; //keeps limiter from getting locked up when signal levels go way out of bounds (like hundreds)

        } else {
            rpeak = efxoutr[i];
            lpeak = efxoutl[i];
        }

        if(stereo) {
            rdelta = fabsf (rpeak);
            if(rvolume < 0.9f) {
                attr = att;
                relr = rel;
            } else if (rvolume < 1.0f) {
                attr = att + ((1.0f - att)*(rvolume - 0.9f)*10.0f);	//dynamically change attack time for limiting mode
                relr = rel/(1.0f + (rvolume - 0.9f)*9.0f);  //release time gets longer when signal is above limiting
            } else {
                attr = 1.0f;
                relr = rel*0.1f;
            }

            if (rdelta > rvolume)
                rvolume = attr * rdelta + (1.0f - attr)*rvolume;
            else
                rvolume = relr * rdelta + (1.0f - relr)*rvolume;


            rvolume_db = rap2dB (rvolume);
            if (rvolume_db < thres_db) {
                rgain = outlevel;
            } else if (rvolume_db < thres_mx) {
                //Dynamic ratio that depends on volume.  As can be seen, ratio starts
                //at something negligibly larger than 1 once volume exceeds thres, and increases toward selected
                // ratio by the time it has reached thres_mx.  --Transmogrifox

                eratio = 1.0f + (kratio-1.0f)*(rvolume_db-thres_db)* coeff_knee;
                rgain =   outlevel*dB2rap(thres_db + (rvolume_db-thres_db)/eratio - rvolume_db);
            } else {
                rgain = outlevel*dB2rap(thres_db + coeff_kk + (rvolume_db-thres_mx)*coeff_ratio - rvolume_db);
                limit = 1;
            }

            if ( rgain < MIN_GAIN) rgain = MIN_GAIN;
            rgain_t = .4f * rgain + .6f * rgain_old;
        };

//Left Channel
        if(stereo)  {
            ldelta = fabsf (lpeak);
        } else  {
            ldelta = 0.5f*(fabsf (lpeak) + fabsf (rpeak));
        };  //It's not as efficient to check twice, but it's small expense worth code clarity

        if(lvolume < 0.9f) {
            attl = att;
            rell = rel;
        } else if (lvolume < 1.0f) {
            attl = att + ((1.0f - att)*(lvolume - 0.9f)*10.0f);	//dynamically change attack time for limiting mode
            rell = rel/(1.0f + (lvolume - 0.9f)*9.0f);  //release time gets longer when signal is above limiting
        } else {
            attl = 1.0f;
            rell = rel*0.1f;
        }

        if (ldelta > lvolume)
            lvolume = attl * ldelta + (1.0f - attl)*lvolume;
        else
            lvolume = rell*ldelta + (1.0f - rell)*lvolume;

        lvolume_db = rap2dB (lvolume);

        if (lvolume_db < thres_db) {
            lgain = outlevel;
        } else if (lvolume_db < thres_mx) { //knee region
            eratio = 1.0f + (kratio-1.0f)*(lvolume_db-thres_db)* coeff_knee;
            lgain =   outlevel*dB2rap(thres_db + (lvolume_db-thres_db)/eratio - lvolume_db);
        } else {
            lgain = outlevel*dB2rap(thres_db + coeff_kk + (lvolume_db-thres_mx)*coeff_ratio - lvolume_db);
            limit = 1;
        }


        if ( lgain < MIN_GAIN) lgain = MIN_GAIN;
        lgain_t = .4f * lgain + .6f * lgain_old;

        if (stereo) {
            efxoutl[i] *= lgain_t;
            efxoutr[i] *= rgain_t;
            rgain_old = rgain;
            lgain_old = lgain;
        } else {
            efxoutl[i] *= lgain_t;
            efxoutr[i] *= lgain_t;
            lgain_old = lgain;
        }

        if(peak) {
            if(efxoutl[i]>0.999f) {            //output hard limiting
                efxoutl[i] = 0.999f;
                clipping = 1;
            }
            if(efxoutl[i]<-0.999f) {
                efxoutl[i] = -0.999f;
                clipping = 1;
            }
            if(efxoutr[i]>0.999f) {
                efxoutr[i] = 0.999f;
                clipping = 1;
            }
            if(efxoutr[i]<-0.999f) {
                efxoutr[i] = -0.999f;
                clipping = 1;
            }
            //highly probably there is a more elegant way to do that, but what the hey...
        }
    }

}



/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



CompBand::CompBand (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lowl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    lowr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midll = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midlr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midhl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midhr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);


    lpf1l = new AnalogFilter (2, 500.0f,.7071f, 0);
    lpf1r = new AnalogFilter (2, 500.0f,.7071f, 0);
    hpf1l = new AnalogFilter (3, 500.0f,.7071f, 0);
    hpf1r = new AnalogFilter (3, 500.0f,.7071f, 0);
    lpf2l = new AnalogFilter (2, 2500.0f,.7071f, 0);
    lpf2r = new AnalogFilter (2, 2500.0f,.7071f, 0);
    hpf2l = new AnalogFilter (3, 2500.0f,.7071f, 0);
    hpf2r = new AnalogFilter (3, 2500.0f,.7071f, 0);
    lpf3l = new AnalogFilter (2, 5000.0f,.7071f, 0);
    lpf3r = new AnalogFilter (2, 5000.0f,.7071f, 0);
    hpf3l = new AnalogFilter (3, 5000.0f,.7071f, 0);
    hpf3r = new AnalogFilter (3, 5000.0f,.7071f, 0);


    CL = new Compressor(efxoutl,efxoutr);
    CML = new Compressor(efxoutl,efxoutr);
    CMH = new Compressor(efxoutl,efxoutr);
    CH = new Compressor(efxoutl,efxoutr);

    CL->Compressor_Change_Preset(0,5);
    CML->Compressor_Change_Preset(0,5);
    CMH->Compressor_Change_Preset(0,5);
    CH->Compressor_Change_Preset(0,5);


    //default values
    Ppreset = 0;
    Pvolume = 50;

    setpreset (Ppreset);
    cleanup ();
};

CompBand::~CompBand ()
{
};

/*
 * Cleanup the effect
 */
void
CompBand::cleanup ()
{
    lpf1l->cleanup ();
    hpf1l->cleanup ();
    lpf1r->cleanup ();
    hpf1r->cleanup ();
    lpf2l->cleanup ();
    hpf2l->cleanup ();
    lpf2r->cleanup ();
    hpf2r->cleanup ();
    lpf3l->cleanup ();
    hpf3l->cleanup ();
    lpf3r->cleanup ();
    hpf3r->cleanup ();
    CL->cleanup();
    CML->cleanup();
    CMH->cleanup();
    CH->cleanup();

};
/*
 * Effect output
 */
void
CompBand::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;


    memcpy(lowl,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(midll,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(midhl,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(highl,smpsl,sizeof(DspFloatType) * PERIOD);

    lpf1l->filterout(lowl);
    hpf1l->filterout(midll);
    lpf2l->filterout(midll);
    hpf2l->filterout(midhl);
    lpf3l->filterout(midhl);
    hpf3l->filterout(highl);

    memcpy(lowr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(midlr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(midhr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(highr,smpsr,sizeof(DspFloatType) * PERIOD);

    lpf1r->filterout(lowr);
    hpf1r->filterout(midlr);
    lpf2r->filterout(midlr);
    hpf2r->filterout(midhr);
    lpf3r->filterout(midhr);
    hpf3r->filterout(highr);


    CL->out(lowl,lowr);
    CML->out(midll,midlr);
    CMH->out(midhl,midhr);
    CH->out(highl,highr);


    for (i = 0; i < PERIOD; i++) {
        efxoutl[i]=(lowl[i]+midll[i]+midhl[i]+highl[i])*level;
        efxoutr[i]=(lowr[i]+midlr[i]+midhr[i]+highr[i])*level;
    }



};


/*
 * Parameter control
 */
void
CompBand::setvolume (int value)
{
    Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 128.0f;

};


void
CompBand::setlevel (int value)
{
    Plevel = value;
    level = dB2rap (60.0f * (DspFloatType)value / 127.0f - 36.0f);


};



void
CompBand::setratio(int ch, int value)
{

    switch(ch) {
    case 0:
        CL->Compressor_Change(2,value);
        break;
    case 1:
        CML->Compressor_Change(2,value);
        break;
    case 2:
        CMH->Compressor_Change(2,value);
        break;
    case 3:
        CH->Compressor_Change(2,value);
        break;
    }
}


void
CompBand::setthres(int ch, int value)
{

    switch(ch) {
    case 0:
        CL->Compressor_Change(1,value);
        break;
    case 1:
        CML->Compressor_Change(1,value);
        break;
    case 2:
        CMH->Compressor_Change(1,value);
        break;
    case 3:
        CH->Compressor_Change(1,value);
        break;
    }
}




void
CompBand::setCross1 (int value)
{
    Cross1 = value;
    lpf1l->setfreq ((DspFloatType)value);
    lpf1r->setfreq ((DspFloatType)value);
    hpf1l->setfreq ((DspFloatType)value);
    hpf1r->setfreq ((DspFloatType)value);

};

void
CompBand::setCross2 (int value)
{
    Cross2 = value;
    hpf2l->setfreq ((DspFloatType)value);
    hpf2r->setfreq ((DspFloatType)value);
    lpf2l->setfreq ((DspFloatType)value);
    lpf2r->setfreq ((DspFloatType)value);

};

void
CompBand::setCross3 (int value)
{
    Cross3 = value;
    hpf3l->setfreq ((DspFloatType)value);
    hpf3r->setfreq ((DspFloatType)value);
    lpf3l->setfreq ((DspFloatType)value);
    lpf3r->setfreq ((DspFloatType)value);

};


void
CompBand::setpreset (int npreset)
{
    const int PRESET_SIZE = 13;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Good Start
        {0, 16, 16, 16, 16, 0, 0, 0, 0, 1000, 5000, 10000, 48},

        //Loudness
        {0, 16, 2, 2, 4, -16, 24, 24, -8, 140, 1000, 5000, 48},

        //Loudness 2
        {64, 16, 2, 2, 2, -32, 24, 24, 24, 100, 1000, 5000, 48}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(43,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
    cleanup ();
};


void
CompBand::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        PLratio = value;
        setratio(0,value);
        break;
    case 2:
        PMLratio = value;
        setratio(1,value);
        break;
    case 3:
        PMHratio = value;
        setratio(2,value);
        break;
    case 4:
        PHratio = value;
        setratio(3,value);
        break;
    case 5:
        PLthres = value;
        setthres(0,value);
        break;
    case 6:
        PMLthres = value;
        setthres(1,value);
        break;
    case 7:
        PMHthres = value;
        setthres(2,value);
        break;
    case 8:
        PHthres = value;
        setthres(3,value);
        break;
    case 9:
        setCross1 (value);
        break;
    case 10:
        setCross2 (value);
        break;
    case 11:
        setCross3(value);
        break;
    case 12:
        setlevel(value);
        break;


    };
};

int
CompBand::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (PLratio);
        break;
    case 2:
        return (PMLratio);
        break;
    case 3:
        return (PMHratio);
        break;
    case 4:
        return (PHratio);
        break;
    case 5:
        return (PLthres);
        break;
    case 6:
        return (PMLthres);
        break;
    case 7:
        return (PMHthres);
        break;
    case 8:
        return (PHthres);
        break;
    case 9:
        return (Cross1);
        break;
    case 10:
        return (Cross2);
        break;
    case 11:
        return (Cross3);
        break;
    case 12:
        return (Plevel);
        break;
    };
    return (0);			//in case of bogus parameter number
};


DspFloatType fft_filter::halsec[20]= {
    0,\
    0,\
    .54119610014619698439972320536638942006107206337801,\
    .50979557910415916894193980398784391368261849190893,\
    .50241928618815570551167011928012092247859337193963,\
    .50060299823519630134550410676638239611758632599591,\
    .50015063602065098821477101271097658495974913010340,\
    .50003765191554772296778139077905492847503165398345,\
    .50000941253588775676512870469186533538523133757983,\
    .50000235310628608051401267171204408939326297376426,\
    .50000058827484117879868526730916804925780637276181,\
    .50000014706860214875463798283871198206179118093251,\
    .50000003676714377807315864400643020315103490883972,\
    .50000000919178552207366560348853455333939112569380,\
    .50000000229794635411562887767906868558991922348920,\
    .50000000057448658687873302235147272458812263401372\
};
DspFloatType fft_filter::costab[20]= {
    .00000000000000000000000000000000000000000000000000,\
    .70710678118654752440084436210484903928483593768847,\
    .92387953251128675612818318939678828682241662586364,\
    .98078528040323044912618223613423903697393373089333,\
    .99518472667219688624483695310947992157547486872985,\
    .99879545620517239271477160475910069444320361470461,\
    .99969881869620422011576564966617219685006108125772,\
    .99992470183914454092164649119638322435060646880221,\
    .99998117528260114265699043772856771617391725094433,\
    .99999529380957617151158012570011989955298763362218,\
    .99999882345170190992902571017152601904826792288976,\
    .99999970586288221916022821773876567711626389934930,\
    .99999992646571785114473148070738785694820115568892,\
    .99999998161642929380834691540290971450507605124278,\
    .99999999540410731289097193313960614895889430318945,\
    .99999999885102682756267330779455410840053741619428\
};
DspFloatType fft_filter::sintab[20]= {
    1.0000000000000000000000000000000000000000000000000,\
    .70710678118654752440084436210484903928483593768846,\
    .38268343236508977172845998403039886676134456248561,\
    .19509032201612826784828486847702224092769161775195,\
    .09801714032956060199419556388864184586113667316749,\
    .04906767432741801425495497694268265831474536302574,\
    .02454122852291228803173452945928292506546611923944,\
    .01227153828571992607940826195100321214037231959176,\
    .00613588464915447535964023459037258091705788631738,\
    .00306795676296597627014536549091984251894461021344,\
    .00153398018628476561230369715026407907995486457522,\
    .00076699031874270452693856835794857664314091945205,\
    .00038349518757139558907246168118138126339502603495,\
    .00019174759731070330743990956198900093346887403385,\
    .00009587379909597734587051721097647635118706561284,\
    .00004793689960306688454900399049465887274686668768\
};
DspFloatType fft_filter::coswrk[20]= {
    .00000000000000000000000000000000000000000000000000,\
    .70710678118654752440084436210484903928483593768847,\
    .92387953251128675612818318939678828682241662586364,\
    .98078528040323044912618223613423903697393373089333,\
    .99518472667219688624483695310947992157547486872985,\
    .99879545620517239271477160475910069444320361470461,\
    .99969881869620422011576564966617219685006108125772,\
    .99992470183914454092164649119638322435060646880221,\
    .99998117528260114265699043772856771617391725094433,\
    .99999529380957617151158012570011989955298763362218,\
    .99999882345170190992902571017152601904826792288976,\
    .99999970586288221916022821773876567711626389934930,\
    .99999992646571785114473148070738785694820115568892,\
    .99999998161642929380834691540290971450507605124278,\
    .99999999540410731289097193313960614895889430318945,\
    .99999999885102682756267330779455410840053741619428\
};
DspFloatType fft_filter::sinwrk[20]= {
    1.0000000000000000000000000000000000000000000000000,\
    .70710678118654752440084436210484903928483593768846,\
    .38268343236508977172845998403039886676134456248561,\
    .19509032201612826784828486847702224092769161775195,\
    .09801714032956060199419556388864184586113667316749,\
    .04906767432741801425495497694268265831474536302574,\
    .02454122852291228803173452945928292506546611923944,\
    .01227153828571992607940826195100321214037231959176,\
    .00613588464915447535964023459037258091705788631738,\
    .00306795676296597627014536549091984251894461021344,\
    .00153398018628476561230369715026407907995486457522,\
    .00076699031874270452693856835794857664314091945205,\
    .00038349518757139558907246168118138126339502603495,\
    .00019174759731070330743990956198900093346887403385,\
    .00009587379909597734587051721097647635118706561284,\
    .00004793689960306688454900399049465887274686668768\
};
/*
fft_filter::fft_filter (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType size)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
}
*/
fft_filter::fft_filter ()
{

}

fft_filter::~fft_filter ()
{

}

void
fft_filter::cleanup ()
{

}

void
fft_filter::out (DspFloatType * smpsl, DspFloatType * smpr)
{

}


void fft_filter::mayer_fht(DspFloatType *fz, int n)
{

    int  k,k1,k2,k3,k4,kx;
    DspFloatType *fi,*fn,*gi;
    TRIG_VARS;

    for (k1=1,k2=0; k1<n; k1++) {
        DspFloatType aa;
        for (k=n>>1; (!((k2^=k)&k)); k>>=1);
        if (k1>k2) {
            aa=fz[k1];
            fz[k1]=fz[k2];
            fz[k2]=aa;
        }
    }
    for ( k=0 ; (1<<k)<n ; k++ );
    k  &= 1;
    if (k==0) {
        for (fi=fz,fn=fz+n; fi<fn; fi+=4) {
            DspFloatType f0,f1,f2,f3;
            f1     = fi[0 ]-fi[1 ];
            f0     = fi[0 ]+fi[1 ];
            f3     = fi[2 ]-fi[3 ];
            f2     = fi[2 ]+fi[3 ];
            fi[2 ] = (f0-f2);
            fi[0 ] = (f0+f2);
            fi[3 ] = (f1-f3);
            fi[1 ] = (f1+f3);
        }
    } else {
        for (fi=fz,fn=fz+n,gi=fi+1; fi<fn; fi+=8,gi+=8) {
            DspFloatType bs1,bc1,bs2,bc2,bs3,bc3,bs4,bc4,
                  bg0,bf0,bf1,bg1,bf2,bg2,bf3,bg3;
            bc1     = fi[0 ] - gi[0 ];
            bs1     = fi[0 ] + gi[0 ];
            bc2     = fi[2 ] - gi[2 ];
            bs2     = fi[2 ] + gi[2 ];
            bc3     = fi[4 ] - gi[4 ];
            bs3     = fi[4 ] + gi[4 ];
            bc4     = fi[6 ] - gi[6 ];
            bs4     = fi[6 ] + gi[6 ];
            bf1     = (bs1 - bs2);
            bf0     = (bs1 + bs2);
            bg1     = (bc1 - bc2);
            bg0     = (bc1 + bc2);
            bf3     = (bs3 - bs4);
            bf2     = (bs3 + bs4);
            bg3     = M_SQRT22*bc4;
            bg2     = M_SQRT22*bc3;
            fi[4 ] = bf0 - bf2;
            fi[0 ] = bf0 + bf2;
            fi[6 ] = bf1 - bf3;
            fi[2 ] = bf1 + bf3;
            gi[4 ] = bg0 - bg2;
            gi[0 ] = bg0 + bg2;
            gi[6 ] = bg1 - bg3;
            gi[2 ] = bg1 + bg3;
        }
    }
    if (n<16) return;

    do {
        DspFloatType s1,c1;
        int ii;
        k  += 2;
        k1  = 1  << k;
        k2  = k1 << 1;
        k4  = k2 << 1;
        k3  = k2 + k1;
        kx  = k1 >> 1;
        fi  = fz;
        gi  = fi + kx;
        fn  = fz + n;
        do {
            DspFloatType g0,f0,f1,g1,f2,g2,f3,g3;
            f1      = fi[0 ] - fi[k1];
            f0      = fi[0 ] + fi[k1];
            f3      = fi[k2] - fi[k3];
            f2      = fi[k2] + fi[k3];
            fi[k2]  = f0         - f2;
            fi[0 ]  = f0         + f2;
            fi[k3]  = f1         - f3;
            fi[k1]  = f1         + f3;
            g1      = gi[0 ] - gi[k1];
            g0      = gi[0 ] + gi[k1];
            g3      = M_SQRT22  * gi[k3];
            g2      = M_SQRT22  * gi[k2];
            gi[k2]  = g0         - g2;
            gi[0 ]  = g0         + g2;
            gi[k3]  = g1         - g3;
            gi[k1]  = g1         + g3;
            gi     += k4;
            fi     += k4;
        } while (fi<fn);
        TRIG_INIT(k,c1,s1);
        for (ii=1; ii<kx; ii++) {
            DspFloatType c2,s2;
            TRIG_NEXT(k,c1,s1);
            c2 = c1*c1 - s1*s1;
            s2 = 2*(c1*s1);
            fn = fz + n;
            fi = fz +ii;
            gi = fz +k1-ii;
            do {
                DspFloatType a,b,g0,f0,f1,g1,f2,g2,f3,g3;
                b       = s2*fi[k1] - c2*gi[k1];
                a       = c2*fi[k1] + s2*gi[k1];
                f1      = fi[0 ]    - a;
                f0      = fi[0 ]    + a;
                g1      = gi[0 ]    - b;
                g0      = gi[0 ]    + b;
                b       = s2*fi[k3] - c2*gi[k3];
                a       = c2*fi[k3] + s2*gi[k3];
                f3      = fi[k2]    - a;
                f2      = fi[k2]    + a;
                g3      = gi[k2]    - b;
                g2      = gi[k2]    + b;
                b       = s1*f2     - c1*g3;
                a       = c1*f2     + s1*g3;
                fi[k2]  = f0        - a;
                fi[0 ]  = f0        + a;
                gi[k3]  = g1        - b;
                gi[k1]  = g1        + b;
                b       = c1*g2     - s1*f3;
                a       = s1*g2     + c1*f3;
                gi[k2]  = g0        - a;
                gi[0 ]  = g0        + a;
                fi[k3]  = f1        - b;
                fi[k1]  = f1        + b;
                gi     += k4;
                fi     += k4;
            } while (fi<fn);
        }
        TRIG_RESET(k,c1,s1);
    } while (k4<n);
}

void fft_filter::mayer_fft(int n, DspFloatType *real, DspFloatType *imag)
{
    DspFloatType a,b,c,d;
    DspFloatType q,r,s,t;
    int i,j,k;
    for (i=1,j=n-1,k=n/2; i<k; i++,j--) {
        a = real[i];
        b = real[j];
        q=a+b;
        r=a-b;
        c = imag[i];
        d = imag[j];
        s=c+d;
        t=c-d;
        real[i] = (q+t)*.5;
        real[j] = (q-t)*.5;
        imag[i] = (s-r)*.5;
        imag[j] = (s+r)*.5;
    }
    mayer_fht(real,n);
    mayer_fht(imag,n);
}

void fft_filter::mayer_ifft(int n, DspFloatType *real, DspFloatType *imag)
{
    DspFloatType a,b,c,d;
    DspFloatType q,r,s,t;
    int i,j,k;
    mayer_fht(real,n);
    mayer_fht(imag,n);
    for (i=1,j=n-1,k=n/2; i<k; i++,j--) {
        a = real[i];
        b = real[j];
        q=a+b;
        r=a-b;
        c = imag[i];
        d = imag[j];
        s=c+d;
        t=c-d;
        imag[i] = (s+r)*0.5;
        imag[j] = (s-r)*0.5;
        real[i] = (q-t)*0.5;
        real[j] = (q+t)*0.5;
    }
}

void fft_filter::mayer_realfft(int n, DspFloatType *real)
{
    DspFloatType a,b;
    int i,j,k;

    mayer_fht(real,n);
    for (i=1,j=n-1,k=n/2; i<k; i++,j--) {
        a = real[i];
        b = real[j];
        real[j] = (a-b)*0.5;
        real[i] = (a+b)*0.5;
    }
}

void fft_filter::mayer_realifft(int n, DspFloatType *real)
{
    DspFloatType a,b;
    int i,j,k;

    for (i=1,j=n-1,k=n/2; i<k; i++,j--) {
        a = real[i];
        b = real[j];
        real[j] = (a-b);
        real[i] = (a+b);
    }
    mayer_fht(real,n);
}

void
fft_filter::realifft(int n, DspFloatType *real)
{
//this way, realpart[0] = real[0]
//and imagpart[0] = real[n/2] for easier indexing
//This function will reverse the order set with reallfft()
    int i,j,k;
    DspFloatType tmp = 0.0f;

    real[n/2] = 0.0f;
//unwrap this thing into something easier to index
    for (i=1+(n/2),j=n-1,k=(n - n/4); i<k; i++,j--) {
        tmp = real[j];
        real[j] = real[i];
        real[i] = tmp;
    }

    mayer_realifft(n, real);
}


void
fft_filter::realfft(int n, DspFloatType *real)
{
//this way, realpart[0] = real[0]
//and imagpart[0] = real[n/2] for easier indexing
    int i,j,k;
    DspFloatType tmp = 0.0f;
    mayer_realfft(n, real);
    real[n/2] = 0.0f;
//unwrap this thing into something easier to index
    for (i=1+(n/2),j=n-1,k=(n - n/4); i<k; i++,j--) {
        tmp = real[j];
        real[j] = real[i];
        real[i] = tmp;

    }

}

void fft_filter::make_window(int n, DspFloatType *window)
{
    int i;
    DspFloatType Ts = 1.0f/((DspFloatType) n);
    DspFloatType x = 2.0f*M_PI*Ts;
    DspFloatType tt = 0.0f;
    for(i=0; i<n; i++) {
        window[i] = 0.5f - 0.5f*cosf(tt);
        tt+=x;

    }

}

void fft_filter::resample_impulse(int size, DspFloatType* ir)
{

    DspFloatType* fftBuf;
    DspFloatType ratio = sqrtf(256.0f)/((DspFloatType) size);
    int interval = size/128;
    int i,j;
    int fftLength = 512;
    while(fftLength<size) fftLength*=2;  //make sure it's a power of 2

    fftBuf = (DspFloatType*) malloc(fftLength*sizeof(DspFloatType));
    memset(fftBuf, 0.0f, fftLength*sizeof(DspFloatType));
    memcpy(fftBuf, ir, size*sizeof(DspFloatType));

    realfft(fftLength, fftBuf);
    for(i=0,j=fftLength/2; i<fftLength/2; i++, j++) {
        fftBuf[i]*=ratio;
        fftBuf[j]*=ratio;
        fftBuf[i] = fftBuf[i]*fftBuf[i] + fftBuf[j]*fftBuf[j];
        fftBuf[j] = 0.0f;
    }

//now downsample this thing
    DspFloatType tmp = 0.0f;
    DspFloatType sign = -1.0f;
    int cnta, cntb, cntc;
    cnta=cntb=0;
    cntc=128;
    for(i=0; i<fftLength/2; i++) {
        tmp+=fftBuf[i];
        fftBuf[i] = 0.0f;
        if(++cntb>interval) {
            if(cnta<128) {
                fftBuf[cnta] =  tmp*sign;
                fftBuf[cntc] =  -tmp*sign;//tmp;
                sign*=-1.0f;
            }
            tmp = 0.0f;
            cntb = 0;
            cnta++;
            cntc++;
        }

//fftBuf[0]  *= 1.14f;
        fftBuf[128]  = 0;
//fftBuf[255] = 0;

    }
    memset(ir, 0.0f, size*sizeof(DspFloatType));
    realifft(256, fftBuf);
    memcpy(ir, fftBuf, 256*sizeof(DspFloatType));

    for(i=0; i<100; i++) {
        ir[i]=0.0f;
    }
//Now convert it to something that looks more like a causal IR
//by adding the forward asymmetry
    for(i=100; i<128; i++) {
        ir[i]*=(0.5f-0.5f*cos(M_PI*(DspFloatType) (i-100)/27.0f));
        ir[i]*=(0.5f-0.5f*cos(M_PI*(DspFloatType) (i-100)/27.0f)); //square it
    }
    for(i=128; i<256; i++) {
        ir[i]*=(0.5f-0.5f*cos(2.0f*M_PI*(DspFloatType) i/256.0f));
    }
    for(i=0,j=100; j<256; j++,i++) {
        ir[i] = ir[j];
        ir[j] = 0.0f;
    }

//Just some debugging code to verify the freq response
    /*realfft(256, ir);
    for(i=0; i<256; i++)
    {
    ir[i] = fabs(ir[i]);
    }*/

}

void fft_filter::load_impulse(int size, char* filename)
{


}

void fft_filter::run_filter(DspFloatType* smps)
{

}



Resample::Resample(int type)
{
    statel = src_new(type , 1 , &errorl);
    stater = src_new(type , 1 , &errorr);

}


Resample::~Resample()
{
};

void
Resample::cleanup()
{
    src_reset(statel);
    src_reset(stater);

};



void
Resample::out(DspFloatType *inl, DspFloatType *inr, DspFloatType *outl, DspFloatType *outr, int frames, DspFloatType ratio)
{


    long int o_frames = lrint((DspFloatType)frames*ratio);
    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;

    srcinfor.data_in = inr;
    srcinfor.input_frames = frames;
    srcinfor.data_out = outr;
    srcinfor.output_frames = o_frames;
    srcinfor.src_ratio = ratio;
    srcinfor.end_of_input = 0;

    errorl = src_process(statel, &srcinfol);
    errorr = src_process(stater, &srcinfor);


}


void
Resample::mono_out(DspFloatType *inl, DspFloatType *outl, int frames, DspFloatType ratio, int o_frames)
{

    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;

    errorl = src_process(statel, &srcinfol);

}


Convolotron::Convolotron (DspFloatType * efxoutl_, DspFloatType * efxoutr_,int DS, int uq, int dq)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Plrcross = 100;
    Psafe = 1;
    Phidamp = 60;
    Filenum = 0;
    Plength = 50;
    Puser = 0;
    real_len = 0;
    convlength = .5f;
    fb = 0.0f;
    feedback = 0.0f;
    adjust(DS);

    templ = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    tempr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);

    maxx_size = (int) (nfSAMPLE_RATE * convlength);  //just to get the max memory allocated
    buf = (DspFloatType *) malloc (sizeof (DspFloatType) * maxx_size);
    rbuf = (DspFloatType *) malloc (sizeof (DspFloatType) * maxx_size);
    lxn = (DspFloatType *) malloc (sizeof (DspFloatType) * maxx_size);
    maxx_size--;
    offset = 0;
    M_Resample = new Resample(0);
    U_Resample = new Resample(dq);//Downsample, uses sinc interpolation for bandlimiting to avoid aliasing
    D_Resample = new Resample(uq);

    setpreset (Ppreset);
    cleanup ();
};

Convolotron::~Convolotron ()
{
};

/*
 * Cleanup the effect
 */
void
Convolotron::cleanup ()
{


};

void
Convolotron::adjust(int DS)
{

    DS_state=DS;


    switch(DS) {

    case 0:
        nPERIOD = PERIOD;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD*96000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD*48000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD*44100.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD*32000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD*22050.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD*16000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD*12000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD*8000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD*4000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        break;
    }
    u_up= (DspFloatType)nPERIOD / (DspFloatType)PERIOD;
    u_down= (DspFloatType)PERIOD / (DspFloatType)nPERIOD;
}





/*
 * Effect output
 */
void
Convolotron::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j, xindex;
    DspFloatType l,lyn;

    if(DS_state != 0) {
        memcpy(templ, smpsl,sizeof(DspFloatType)*PERIOD);
        memcpy(tempr, smpsr,sizeof(DspFloatType)*PERIOD);
        U_Resample->out(templ,tempr,smpsl,smpsr,PERIOD,u_up);
    }


    for (i = 0; i < nPERIOD; i++) {

        l = smpsl[i] + smpsr[i] + feedback;
        oldl = l * hidamp + oldl * (alpha_hidamp);  //apply damping while I'm in the loop
        lxn[offset] = oldl;


        //Convolve left channel
        lyn = 0;
        xindex = offset;

        for (j =0; j<length; j++) {
            if (--xindex<0) xindex = maxx_size;		//length of lxn is maxx_size.
            lyn += buf[j] * lxn[xindex];		//this is all there is to convolution
        }

        feedback = fb * lyn;
        templ[i] = lyn * levpanl;
        tempr[i] = lyn * levpanr;

        if (++offset>maxx_size) offset = 0;


    };

    if(DS_state != 0) {
        D_Resample->out(templ,tempr,efxoutl,efxoutr,nPERIOD,u_down);

    } else {
        memcpy(efxoutl, templ,sizeof(DspFloatType)*PERIOD);
        memcpy(efxoutr, tempr,sizeof(DspFloatType)*PERIOD);
    }





};


/*
 * Parameter control
 */
void
Convolotron::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Convolotron::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    lpanning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
    rpanning = 1.0f - lpanning;
    levpanl=lpanning*level*2.0f;
    levpanr=rpanning*level*2.0f;

};

int
Convolotron::setfile(int value)
{

    DspFloatType sr_ratio;

    offset = 0;
    maxx_read = maxx_size / 2;
    memset(buf,0,sizeof(DspFloatType) * maxx_size);
    memset(rbuf,0,sizeof(DspFloatType) * maxx_size);
    if(!Puser) {
        Filenum = value;
        memset(Filename,0, sizeof(Filename));
        sprintf(Filename, "%s/%d.wav",DATADIR,Filenum+1);
    }


    sfinfo.format = 0;
    if(!(infile = sf_open(Filename, SFM_READ, &sfinfo))) {
        real_len = 1;
        length = 1;
        rbuf[0] = 1.0f;
        process_rbuf();
        return(0);
    }

    if (sfinfo.frames > maxx_read) real_len = maxx_read;
    else real_len=sfinfo.frames;
    sf_seek (infile,0, SEEK_SET);
    sf_readf_float(infile,buf,real_len);
    sf_close(infile);




    if (sfinfo.samplerate != (int)nSAMPLE_RATE) {
        sr_ratio = (DspFloatType)nSAMPLE_RATE/((DspFloatType) sfinfo.samplerate);
        M_Resample->mono_out(buf,rbuf,real_len,sr_ratio,lrint((DspFloatType)real_len*sr_ratio));
        real_len =lrintf((DspFloatType)real_len*(DspFloatType)sr_ratio);
    }

    else memcpy(rbuf,buf,real_len*sizeof(DspFloatType));

    process_rbuf();

    return(1);
};


void
Convolotron::process_rbuf()
{
    int ii,j,N,N2;
    DspFloatType tailfader, alpha, a0, a1, a2, Nm1p, Nm1pp, IRpowa, IRpowb, ngain, maxamp;
    memset(buf,0, sizeof(DspFloatType)*real_len);

    if (length > real_len) length = real_len;
    /*Blackman Window function
    wn = a0 - a1*cos(2*pi*n/(N-1)) + a2 * cos(4*PI*n/(N-1)
    a0 = (1 - alpha)/2; a1 = 0.5; a2 = alpha/2
    */
    alpha = 0.16f;
    a0 = 0.5f*(1.0f - alpha);
    a1 = 0.5f;
    a2 = 0.5*alpha;
    N = length;
    N2 = length/2;
    Nm1p = D_PI/((DspFloatType) (N - 1));
    Nm1pp = 4.0f * PI/((DspFloatType) (N - 1));

    for(ii=0; ii<length; ii++) {
        if (ii<N2) {
            tailfader = 1.0f;
        } else {
            tailfader = a0 - a1*cosf(ii*Nm1p) + a2 * cosf(ii*Nm1pp);   //Calculate Blackman Window for right half of IR
        }

        buf[ii]= rbuf[ii] * tailfader;   //Apply window function

    }


    //memcpy(buf,rbuf,real_len*sizeof(DspFloatType));

    IRpowa = IRpowb = maxamp = 0.0f;
    //compute IR signal power
    for(j=0; j<length; j++) {
        IRpowa += fabsf(rbuf[j]);
        if(maxamp < fabsf(buf[j])) maxamp = fabsf(buf[j]);   //find maximum level to normalize

        if(j < length) {
            IRpowb += fabsf(buf[j]);
        }

    }

    //if(maxamp < 0.3f) maxamp = 0.3f;
    ngain = IRpowa/IRpowb;
    if (ngain > maxx_read) ngain = maxx_read;
    for(j=0; j<length; j++) buf[j] *= ngain;

    if (Psafe) {
        impulse.resample_impulse(length, buf);
        length = 156;
        convlength = length/fSAMPLE_RATE;
    }
    /*
    //This section can be uncommented to make a text file you can plot
    //with something like gnuplot
    FILE * textfile;
    textfile = fopen("IR.txt", "w");
    if (textfile!=NULL)
    {
    for(j=0;j<length;j++) {
    fprintf (textfile, "%d\t%f\n",j,buf[j]);
    }
    fclose (textfile);
    }
    */



}

void
Convolotron::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (DspFloatType)Phidamp / 127.1f;
    alpha_hidamp = 1.0f - hidamp;
};

void
Convolotron::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 4;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Convolotron 1
        {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
        //Convolotron 2
        {67, 64, 1, 100, 0, 64, 30, 20, 1, 0, 0},
        //Convolotron 3
        {67, 75, 1, 100, 0, 64, 30, 20, 2, 0, 0},
        //Convolotron 4
        {67, 60, 1, 100, 0, 64, 30, 20, 3, 0, 0}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(29,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};

void
Convolotron::UpdateLength ()
{
    convlength = ((DspFloatType) Plength)/1000.0f;                   //time in seconds
    length = (int) (nfSAMPLE_RATE * convlength);        //time in samples
    process_rbuf();
}

void
Convolotron::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        Psafe = value;
        UpdateLength();
        break;
    case 3:
        Plength = value;
        UpdateLength();
        break;
    case 8:
        if(!setfile(value)) error_num=1;
        UpdateLength();
        break;
    case 5:
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        Plevel = value;
        level =  dB2rap (60.0f * (DspFloatType)Plevel / 127.0f - 40.0f);
        levpanl=lpanning*level*2.0f;
        levpanr=rpanning*level*2.0f;
        break;
    case 4:
        Puser = value;
        break;
    case 9:
        break;
    case 10:
        Pfb = value;
        if(Pfb<0) {
            fb = (DspFloatType) .1f*value/250.0f*.15f;
        } else {
            fb = (DspFloatType) .1f*value/500.0f*.15f;
        }
        break;

    };
};

int
Convolotron::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return(Psafe);
        break;
    case 3:
        return(Plength);
        break;
    case 8:
        return (Filenum);
        break;
    case 5:
        return (0);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return(Plevel);
        break;
    case 4:
        return(Puser);
        break;
    case 9:
        return(0);
        break;
    case 10:
        return(Pfb);
        break;

    };
    return (0);			//in case of bogus parameter number
};

Waveshaper::Waveshaper()
{


    switch(Wave_res_amount) {
    case 0:
        period_coeff = 1;
        ncSAMPLE_RATE=cSAMPLE_RATE;
        break;
    case 1:
        period_coeff = 2;
        ncSAMPLE_RATE=cSAMPLE_RATE/2.0f;
        break;
    case 2:
        period_coeff = 4;
        ncSAMPLE_RATE=cSAMPLE_RATE/4.0f;
        break;
    case 3:
        period_coeff = 8;
        ncSAMPLE_RATE=cSAMPLE_RATE/8.0f;
        break;
    case 4:
        period_coeff = 12;
        ncSAMPLE_RATE=cSAMPLE_RATE/12.0f;
        break;

    }


    temps = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD * period_coeff);
    u_up= (DspFloatType)period_coeff;
    u_down = 1.0 / u_up;

    compg = 0.0f;  //used by compression distortion
    cratio = 0.25f;  //used by compression for hardness
    tmpgain = 1.0f;  // compression distortion temp variable
    dthresh = 0.25;
    dyno = 0.0f;
    dynodecay = 0.0167f/(ncSAMPLE_RATE + 0.0167f); //about 60Hz sub modulation from this

    Ip = 0.0f;
    Vsupp = 200.0f;
    Vgbias = 0.075f;  //bias point for Valve1 model
    R = 220000.0f; //Plate resistor, 220k
    P = 0.0002f;  //constant tuning bias current for Valve1

    mu = 100.0f;  //Valve2 gain
    V2bias = 1.5f;  //Valve2 bias voltage
    Is = 105/(R*powf(V2bias*mu, 1.5f));  //bias point for Valve2
    Vg2 = mu*V2bias;
    vfact = 12;   //adjustment of valve shut-off.  Small is hard clipping, large is softer clipping
    ffact = 40;  //Valve2 ffact and vfact set onset of significant limiting.   Small is hard clip, large is soft clip
    Vlv2out = 0.0f;
    V2dyno = 0.0f;

    Vmin = Vsupp - 2.5;  //Approximate cathode voltage when tube is saturated.
    Vfactor = 1.5f;
    Vdyno = 0.0f;

    U_Resample = new Resample(Wave_up_q);  //Downsample, uses sinc interpolation for bandlimiting to avoid aliasing
    D_Resample = new Resample(Wave_down_q);




};

Waveshaper::~Waveshaper()
{
};
void Waveshaper::cleanup()
{
    compg = 0.0f;  //used by compression distortion
    cratio = 0.25f;  //used by compression for hardness
    tmpgain = 1.0f;  // compression distortion temp variable
    dthresh = 0.25;
    dyno = 0.0f;
    dynodecay = 0.0167f/(ncSAMPLE_RATE + 0.0167f); //about 60Hz sub modulation from this
}


void
Waveshaper::waveshapesmps (int n, DspFloatType * smps, int type,
                           int drive, int eff)
{

    int nn=n;

    if(Wave_res_amount > 0) {
        nn=n*period_coeff;
        U_Resample->mono_out(smps,temps,n,u_up,nn);
    }

    else memcpy(temps,smps,sizeof(DspFloatType)*n);

    int i;
    DspFloatType ws = (DspFloatType)drive / 127.0f + .00001f;
    ws = 1.0f - expf (-ws * 4.0f);
    DspFloatType tmpv;
    DspFloatType factor;

    switch (type + 1 ) {
    case 1:
        ws = powf (10.0f, ws * ws * 3.0f) - 1.0f + 0.001f;	//Arctangent
        for (i = 0; i < nn; i++)
            temps[i] = atanf (temps[i] * ws) / atanf (ws);
        break;
    case 2:
        ws = ws * ws * 32.0f + 0.0001f;	//Asymmetric
        if (ws < 1.0)
            tmpv = f_sin (ws) + 0.1f;
        else
            tmpv = 1.1f;
        for (i = 0; i < nn; i++) {
            temps[i] = f_sin (temps[i] * (0.1f + ws - ws * temps[i])) / tmpv;
        };
        break;
    case 3:
        ws = ws * ws * ws * 20.0f + 0.0001f;	//Pow
        for (i = 0; i < nn; i++) {
            temps[i] *= ws;
            if (fabsf (temps[i]) < 1.0) {
                temps[i] = (temps[i] - powf (temps[i], 3.0f)) * 3.0f;
                if (ws < 1.0)
                    temps[i] /= ws;
            } else
                temps[i] = 0.0;
        };
        break;
    case 4:
        ws = ws * ws * ws * 32.0f + 0.0001f;	//Sine
        if (ws < 1.57f)
            tmpv = f_sin (ws);
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++)
            temps[i] = f_sin (temps[i] * ws) / tmpv;
        break;
    case 5:
        ws = ws * ws + 0.000001f;	//Quantisize
        for (i = 0; i < nn; i++)
            temps[i] = floorf (temps[i] / ws + 0.15f) * ws;
        break;
    case 6:
        ws = ws * ws * ws * 32.0f + 0.0001f;	//Zigzag
        if (ws < 1.0)
            tmpv = f_sin (ws);
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++)
            temps[i] = asinf (f_sin (temps[i] * ws)) / tmpv;
        break;
    case 7:
        ws = powf (2.0f, -ws * ws * 8.0f);	//Limiter
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i];
            if (fabsf (tmp) > ws) {
                if (tmp >= 0.0)
                    temps[i] = 1.0f;
                else
                    temps[i] = -1.0f;
            } else
                temps[i] /= ws;
        };
        break;
    case 8:
        ws = powf (2.0f, -ws * ws * 8.0f);	//Upper Limiter
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i];
            if (tmp > ws)
                temps[i] = ws;
            temps[i] *= 2.0f;
        };
        break;
    case 9:
        ws = powf (2.0f, -ws * ws * 8.0f);	//Lower Limiter
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i];
            if (tmp < -ws)
                temps[i] = -ws;
            temps[i] *= 2.0f;
        };
        break;
    case 10:
        ws = (powf (2.0f, ws * 6.0f) - 1.0f) / powf (2.0f, 6.0f);	//Inverse Limiter
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i];
            if (fabsf (tmp) > ws) {
                if (tmp >= 0.0)
                    temps[i] = tmp - ws;
                else
                    temps[i] = tmp + ws;
            } else
                temps[i] = 0;
        };
        break;
    case 11:
        ws = powf (5.0f, ws * ws * 1.0f) - 1.0f;	//Clip
        for (i = 0; i < nn; i++)
            temps[i] =
                temps[i] * (ws + 0.5f) * 0.9999f - floorf (0.5f +
                        temps[i] * (ws +
                                    0.5f) * 0.9999f);
        break;
    case 12:
        ws = ws * ws * ws * 30.0f + 0.001f;	//Asym2
        if (ws < 0.3)
            tmpv = ws;
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i] * ws;
            if ((tmp > -2.0) && (tmp < 1.0))
                temps[i] = tmp * (1.0f - tmp) * (tmp + 2.0f) / tmpv;
            else
                temps[i] = 0.0f;
        };
        break;
    case 13:
        ws = ws * ws * ws * 32.0f + 0.0001f;	//Pow2
        if (ws < 1.0)
            tmpv = ws * (1.0f + ws) / 2.0f;
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i] * ws;
            if ((tmp > -1.0f) && (tmp < 1.618034f))
                temps[i] = tmp * (1.0f - tmp) / tmpv;
            else if (tmp > 0.0)
                temps[i] = -1.0f;
            else
                temps[i] = -2.0f;
        };
        break;
    case 14:
        ws = powf (ws, 5.0f) * 80.0f + 0.0001f;	//sigmoid
        if (ws > 10.0)
            tmpv = 0.5f;
        else
            tmpv = 0.5f - 1.0f / (expf (ws) + 1.0f);
        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i] * ws;
            if (tmp < -10.0)
                tmp = -10.0f;
            else if (tmp > 10.0)
                tmp = 10.0f;
            tmp = 0.5f - 1.0f / (expf (tmp) + 1.0f);
            temps[i] = tmp / tmpv;
        };
        break;
    case 15:		//Sqrt "Crunch" -- Asymmetric square root distortion.
        ws = ws*ws*CRUNCH_GAIN + 1.0f;

        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i] * ws;
            if (tmp < Tlo) {
                temps[i] = Tlc - sqrtf(-tmp*DIV_TLC_CONST);

            } else if (tmp > Thi) {
                temps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);
            } else {
                temps[i] = tmp;
            };

            if (temps[i] < -1.0)
                temps[i] = -1.0f;
            else if (temps[i] > 1.0)
                temps[i] = 1.0f;

        };
        break;
    case 16:		//Sqrt "Crunch2" -- Asymmetric square root distortion.
        ws = ws*ws*CRUNCH_GAIN + 1.0f;

        for (i = 0; i < nn; i++) {
            DspFloatType tmp = temps[i] * ws;
            if (tmp < Tlo) {
                temps[i] = Tlc;

            } else if (tmp > Thi) {
                temps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);
            } else {
                temps[i] = tmp;
            };

            if (temps[i] < -1.0)
                temps[i] = -1.0f;
            else if (temps[i] > 1.0)
                temps[i] = 1.0f;
        };
        break;

    case 17:		//Octave Up
        ws = ws*ws*30.0f + 1.0f;

        for (i = 0; i < nn; i++) {
            DspFloatType tmp = fabs(temps[i])* ws;
            if (tmp > 1.0f) {
                tmp = 1.0f;
            }

            temps[i] = tmp;		//a little bit of DC correction

        };
        break;
    case 18:
        ws = ws*D_PI+.00001;
        if (ws < 1.57f)
            tmpv = f_sin (ws);
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++)
            temps[i]=f_sin(ws * temps[i] + f_sin( ws * 2.0f*temps[i]))/ tmpv;

        break;

    case 19:
        ws =  ws * D_PI + 0.0001f;
        if (ws < 1.57f)
            tmpv = f_sin (ws);
        else
            tmpv = 1.0f;
        for (i = 0; i < nn; i++)
            temps[i]=f_sin(ws * temps[i] + f_sin(ws * temps[i])/tmpv);
        break;

    case 20:  //Compression
        cratio = 1.0f - 0.25f * ws;
        ws =  1.5f*ws*CRUNCH_GAIN + 4.0f;
        for (i = 0; i < nn; i++) {  //apply compression
            tmpv = fabs(ws * temps[i]);
            dyno += 0.01f * (1.0f - dynodecay) * tmpv;
            dyno *= dynodecay;
            tmpv += dyno;

            if(tmpv > dthresh) {                              //if envelope of signal exceeds thresh, then compress
                compg = dthresh + dthresh*(tmpv - dthresh)/tmpv;
                dthresh = 0.25f + cratio*(compg - dthresh);   //dthresh changes dynamically

                if (temps[i] > 0.0f) {
                    temps[i] = compg;
                } else {
                    temps[i] = -1.0f * compg;
                }

            } else {
                temps[i] *= ws;
            }

            if(tmpv < dthresh) dthresh = tmpv;
            if(dthresh < 0.25f) dthresh = 0.25f;


        };
        break;

    case 21: //Overdrive
        ws = powf (10.0f, ws * ws * 3.0f) - 1.0f + 0.001f;
        for (i = 0; i < nn; i++) {
            if(temps[i]>0.0f) temps[i] = sqrtf(temps[i]*ws);
            else temps[i] = -sqrtf(-temps[i]*ws);
        }
        break;

    case 22: //Soft
        ws = powf(4.0f, ws*ws+1.0f);
        for (i = 0; i < nn; i++) {
            if(temps[i]>0.0f) temps[i] = ws*powf(temps[i],1.4142136f);
            else temps[i] = ws* -powf(-temps[i],1.4142136f);
        }
        break;

    case 23: //Super Soft

        ws = powf (20.0f, ws * ws) + 0.5f;
        factor = 1.0f / ws;
        for (i = 0; i < nn; i++) {
            if(temps[i] > 1.0) temps[i] = 1.0f;
            if(temps[i] < -1.0) temps[i] = -1.0f;

            if(temps[i]<factor) temps[i]=temps[i];
            else if(temps[i]>factor) temps[i]=factor+(temps[i]-factor)/powf(1.0f+((temps[i]-factor)/(1.0f-temps[i])),2.0f);
            else if(temps[i]>1.0f) temps[i]=(factor+1.0f)*.5f;
            temps[i]*=ws;
        }
        break;

    case 24:  // Hard Compression (used by stompboxes)
        cratio = 0.05;
        if (eff) {
            ws =  1.5f*ws*CRUNCH_GAIN + 1.0f;
        } else {
            ws = 1.0f;
        }           //allows functions applying gain before waveshaper

        for (i = 0; i < nn; i++) {  //apply compression
            tmpv = fabs(ws * temps[i]);

            if(tmpv > dthresh) {                              //if envelope of signal exceeds thresh, then compress
                compg = dthresh + dthresh*(tmpv - dthresh)/tmpv;
                dthresh = 0.5f + cratio*(compg - dthresh);   //dthresh changes dynamically

                if (temps[i] > 0.0f) {
                    temps[i] = compg;
                } else {
                    temps[i] = -1.0f * compg;
                }

            } else {
                temps[i] *= ws;
            }

            if(tmpv < dthresh) dthresh = tmpv;
            if(dthresh < 0.5f) dthresh = 0.5f;

        };
        break;

    case 25:  // Op Amp limiting (used by stompboxes), needs to get a large signal to do something
        cratio = 0.05;
        for (i = 0; i < nn; i++) {  //apply compression
            tmpv = fabs(temps[i]);

            if(tmpv > dthresh) {                              //if envelope of signal exceeds thresh, then compress
                compg = dthresh + dthresh*(tmpv - dthresh)/tmpv;
                dthresh = 3.5f + cratio*(compg - dthresh);   //dthresh changes dynamically

                if (temps[i] > 0.0f) {
                    temps[i] = compg;
                } else {
                    temps[i] = -1.0f * compg;
                }

            } else {
                temps[i] *= 1.0f;
            }

            if(tmpv < dthresh) dthresh = tmpv;
            if(dthresh < 3.5f) dthresh = 3.5f;

        };
        break;

    case 26: //JFET

        ws = powf (35.0f, ws * ws) + 4.0f;
        factor = sqrt(1.0f / ws);
        for (i = 0; i < nn; i++) {
            temps[i] = temps[i] + factor;
            if(temps[i] < 0.0) temps[i] = 0.0f;
            temps[i] = 1.0f - 2.0f/(ws*temps[i]*temps[i] + 1.0f);
        }
        break;

    case 27: //dyno JFET

        ws = powf (85.0f, ws * ws) + 10.0f;

        for (i = 0; i < nn; i++) {
            tmpv = fabs(temps[i]);
            if(tmpv > 0.15f) { // -16dB crossover distortion... dyno only picks up the peaks above 16dB.  Good for nasty fuzz
                dyno += (1.0f - dynodecay) * tmpv;
            }
            dyno *= dynodecay;  //always decays
            temps[i] = temps[i] + sqrtf((1.0f + 0.05f*dyno) / ws);
            if(temps[i] < 0.0) temps[i] = 0.0f;

            temps[i] = 1.0f - 2.0f/(ws*temps[i]*temps[i] + 1.0f);


        }
        break;

    case 28: //Valve 1

        ws = powf (4.0f, ws * ws) - 0.98f;

        for (i = 0; i < nn; i++) {
            Vg = Vgbias + ws*temps[i] - 0.1f*Vdyno;

            if(Vg<=0.05f) Vg = 0.05f/((-20.0f*Vg) + 2.0f);

            Ip = P*powf(Vg,Vfactor);
            tmpv = Vsupp - (Vmin - (Vmin/(R*Ip + 1.0f)));  //Here is the plate voltage
            tmpv = (tmpv - 106.243f)/100.0f;
            Vdyno += (1.0f - dynodecay) * tmpv;
            Vdyno *= dynodecay;
            temps[i] = tmpv;

        }
        break;

    case 29: //Valve 2

        ws = powf (110.0f, ws);

        for (i = 0; i < nn; i++) {

            Vg2 = mu*(V2bias + V2dyno + ws*temps[i]);

            if(Vg2 <= vfact) Vg2 = vfact/((-Vg2/vfact) + 2.0f);	 //Toward cut-off, behavior is a little different than 2/3 power law
            Vlv2out = Vsupp - R*Is*powf(Vg2,1.5f);   //2/3 power law relationship
            if(Vlv2out <= ffact) Vlv2out = ffact/((-Vlv2out/ffact) + 2.0f);  //Then as Vplate decreases, gain decreases until saturation

            temps[i] = (Vlv2out - 95.0f)*0.01f;
            V2dyno += (1.0f - dynodecay)*temps[i];
            V2dyno *= dynodecay;  //always decays

        }
        break;
    case 30: //Diode clipper

        ws = 5.0f + powf (110.0f, ws);

        for (i = 0; i < nn; i++) {

            tmpv = ws*temps[i];
            if (tmpv>0.0f) tmpv = 1.0f - 1.0f/powf(4.0f, tmpv);
            else tmpv = -(1.0f - 1.0f/powf(4.0f, -tmpv));
            temps[i] = tmpv;

        }
        break;


    };

    if(Wave_res_amount>= 0) {
        D_Resample->mono_out(temps,smps,nn,u_down,n);
    } else
        memcpy(smps,temps,sizeof(DspFloatType)*n);


};


Dflange::Dflange (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    period_const = 1.0f/fPERIOD;

    //default values
    Ppreset = 0;

    ldelay = NULL;
    rdelay = NULL;


    maxx_delay = (int) SAMPLE_RATE * 0.055f;
    ldelay = new DspFloatType[maxx_delay];
    rdelay = new DspFloatType[maxx_delay];
    zldelay = new DspFloatType[maxx_delay];
    zrdelay = new DspFloatType[maxx_delay];

    ldelayline0  = new delayline(0.055f, 2);
    rdelayline0  = new delayline(0.055f, 2);
    ldelayline1  = new delayline(0.055f, 2);
    rdelayline1  = new delayline(0.055f, 2);
    ldelayline0 -> set_averaging(0.05f);
    rdelayline0 -> set_averaging(0.05f);
    ldelayline0->set_mix( 0.5f );
    rdelayline0->set_mix( 0.5f );
    ldelayline1 -> set_averaging(0.05f);
    rdelayline1 -> set_averaging(0.05f);
    ldelayline1->set_mix( 0.5f );
    rdelayline1->set_mix( 0.5f );

    fsubtract = 0.5f;
    fhidamp = 1.0f;
    fwidth = 800;
    fdepth = 50;
    zcenter = (int) fSAMPLE_RATE/floorf(0.5f * (fdepth + fwidth));
    base = 7.0f;		//sets curve of modulation to frequency relationship
    ibase = 1.0f/base;
    //default values
    Ppreset = 0;
    Pintense = 0;
    rsA = 0.0f;
    rsB = 0.0f;
    lsA  = 0.0f;
    lsB = 0.0f;
    logmax = logf(1000.0f)/logf(2.0f);
    setpreset (Ppreset);
    cleanup ();
};

Dflange::~Dflange ()
{
};

/*
 * Cleanup the effect
 */
void
Dflange::cleanup ()
{
    int i;
    for (i = 0; i < maxx_delay; i++) {
        ldelay[i] = 0.0;
        rdelay[i] = 0.0;
        zldelay[i] = 0.0;
        zrdelay[i] = 0.0;
    };

    //loop variables
    l = 0.0f;
    r = 0.0f;
    ldl = 0.0f;
    rdl = 0.0f;
    rflange0 = 0.0f;
    lflange0 = 0.0f;
    rflange1 = 0.0f;
    lflange1 = 0.0f;

};


/*
 * Effect output
 */
void
Dflange::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    //deal with LFO's
    int tmp0, tmp1;

    DspFloatType lfol, lfor, lmod, rmod, lmodfreq, rmodfreq, rx0, rx1, lx0, lx1;
    DspFloatType ldif0, ldif1, rdif0, rdif1;  //Difference between fractional delay and floor(fractional delay)
    DspFloatType drA, drB, dlA, dlB;	//LFO inside the loop.

    lfo.effectlfoout (&lfol, &lfor);
    lmod = lfol;
    if(Pzero && Pintense) rmod = 1.0f - lfol;  //using lfol is intentional
    else rmod = lfor;

    if(Pintense) {
//do intense stuff
        lmodfreq = (f_pow2(lmod*lmod*logmax)) * fdepth;  //2^x type sweep for musical interpretation of moving delay line.
        rmodfreq = (f_pow2(rmod*rmod*logmax)) * fdepth;  //logmax depends on depth
        rflange0 = 0.5f/rmodfreq;		//Turn the notch frequency into 1/2 period delay
        rflange1 = rflange0 + (1.0f - foffset)/fdepth;				//Set relationship of second delay line
        lflange0 = 0.5f/lmodfreq;
        lflange1 = lflange0 + (1.0f - foffset)/fdepth;

        rx0 = (rflange0 - oldrflange0) * period_const;  //amount to add each time around the loop.  Less processing of linear LFO interp inside the loop.
        rx1 =  (rflange1 - oldrflange1) * period_const;
        lx0 = (lflange0 - oldlflange0) * period_const;
        lx1  = (lflange1 - oldlflange1) * period_const;

// Now there is a fractional amount to add
        drA = oldrflange0;
        drB = oldrflange1;
        dlA = oldlflange0;
        dlB = oldlflange1;

        oldrflange0 = rflange0;
        oldrflange1 = rflange1;
        oldlflange0 = lflange0;
        oldlflange1 = lflange1;
        //lfo ready...

        if(Pzero) {
            for (i = 0; i < PERIOD; i++) {

                ldl = smpsl[i] * lpan + ldl * ffb;
                rdl = smpsr[i] * rpan + rdl * ffb;

                //LowPass Filter
                ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
                rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
                oldl = ldl + DENORMAL_GUARD;
                oldr = rdl + DENORMAL_GUARD;
                /*
                Here do the delay line stuff
                basically,
                dl1(dl2(smps));
                ^^This runs two flangers in series so you can get a DspFloatType notch
                */

                ldl = ldelayline0->delay(ldl,dlA, 0, 1, 0)  + ldelayline1->delay(ldl,drA, 0, 1, 0);
                rdl = rdelayline0->delay(rdl,dlB, 0, 1, 0) + rdelayline1->delay(rdl,drB, 0, 1, 0);

                efxoutl[i] = ldl = ldl * flrcross + rdl * frlcross;
                efxoutr[i] = rdl = rdl * flrcross + ldl * frlcross;

// Increment LFO
                drA += rx0;
                drB += rx1;
                dlA += lx0;
                dlB += lx1;
            }
        } else {
            for (i = 0; i < PERIOD; i++) {

                ldl = smpsl[i] * lpan + ldl * ffb;
                rdl = smpsr[i] * rpan + rdl * ffb;

                //LowPass Filter
                ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
                rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
                oldl = ldl + DENORMAL_GUARD;
                oldr = rdl + DENORMAL_GUARD;
                /*
                Here do the delay line stuff
                basically,
                dl1(dl2(smps));
                ^^This runs two flangers in series so you can get a DspFloatType notch
                */

                ldl = ldelayline0->delay(ldl,dlA, 0, 1, 0);
                ldl = ldelayline1->delay(ldl,dlB, 0, 1, 0);

                rdl = rdelayline0->delay(rdl,drA, 0, 1, 0);
                rdl = rdelayline1->delay(rdl,drB, 0, 1, 0);

                efxoutl[i] = ldl = ldl * flrcross + rdl * frlcross;
                efxoutr[i] = rdl = rdl * flrcross + ldl * frlcross;

// Increment LFO
                drA += rx0;
                drB += rx1;
                dlA += lx0;
                dlB += lx1;
            }
        }


    } else {

        lmodfreq = fdepth + fwidth*(powf(base, lmod) - 1.0f)*ibase;	//sets frequency of lowest notch. // 20 <= fdepth <= 4000 // 20 <= width <= 16000 //
        rmodfreq = fdepth + fwidth*(powf(base, rmod) - 1.0f)*ibase;


        if (lmodfreq > 10000.0f)
            lmodfreq = 10000.0f;
        else if (lmodfreq < 10.0f)
            lmodfreq = 10.0f;
        if (rmodfreq > 10000.0)
            rmodfreq = 10000.0f;
        else if (rmodfreq < 10.0f)
            rmodfreq = 10.0f;

        rflange0 = fSAMPLE_RATE * 0.5f/rmodfreq;		//Turn the notch frequency into a number for delay
        rflange1 = rflange0 * foffset;				//Set relationship of second delay line
        lflange0 = fSAMPLE_RATE * 0.5f/lmodfreq;
        lflange1 = lflange0 * foffset;

//now is a delay expressed in number of samples.  Number here
//will be fractional, but will use linear interpolation inside the loop to make a decent guess at
//the numbers between samples.

        rx0 = (rflange0 - oldrflange0) * period_const;  //amount to add each time around the loop.  Less processing of linear LFO interp inside the loop.
        rx1 =  (rflange1 - oldrflange1) * period_const;
        lx0 = (lflange0 - oldlflange0) * period_const;
        lx1  = (lflange1 - oldlflange1) * period_const;
// Now there is a fractional amount to add

        drA = oldrflange0;
        drB = oldrflange1;
        dlA = oldlflange0;
        dlB = oldlflange1;
        // dr, dl variables are the LFO inside the loop.

        oldrflange0 = rflange0;
        oldrflange1 = rflange1;
        oldlflange0 = lflange0;
        oldlflange1 = lflange1;
        //lfo ready...


        for (i = 0; i < PERIOD; i++) {

            //Delay line utility
            ldl = ldelay[kl];
            rdl = rdelay[kr];
            l = ldl * flrcross + rdl * frlcross;
            r = rdl * flrcross + ldl * frlcross;
            ldl = l;
            rdl = r;
            ldl = smpsl[i] * lpan - ldl * ffb;
            rdl = smpsr[i] * rpan - rdl * ffb;


            //LowPass Filter
            ldelay[kl] = ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
            rdelay[kr] = rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
            oldl = ldl + DENORMAL_GUARD;
            oldr = rdl + DENORMAL_GUARD;

            if(Pzero) {
                //Offset zero reference delay
                zdl = zldelay[zl];
                zdr = zrdelay[zr];
                zldelay[zl] = smpsl[i];
                zrdelay[zr] = smpsr[i];
                if (--zl < 0)   //Cycle delay buffer in reverse so delay time can be indexed directly with addition
                    zl =  zcenter;
                if (--zr < 0)
                    zr =  zcenter;
            }

            //End delay line management, start flanging:

            //Right Channel, delay A
            rdif0 = drA - floor(drA);
            tmp0 = (kr + (int) floor(drA)) %  maxx_delay;
            tmp1 = tmp0 + 1;
            if (tmp1 >= maxx_delay) tmp1 =  0;
            //rsA = rdelay[tmp0] + rdif0 * (rdelay[tmp1] - rdelay[tmp0] );	//here is the first right channel delay
            rsA = rdelay[tmp1] + rdif0 * (rdelay[tmp0] - rsA );	//All-pass interpolator

            //Right Channel, delay B
            rdif1 = drB - floor(drB);
            tmp0 = (kr + (int) floor(drB)) %  maxx_delay;
            tmp1 = tmp0 + 1;
            if (tmp1 >= maxx_delay) tmp1 =  0;
            //rsB = rdelay[tmp0] + rdif1 * (rdelay[tmp1] - rdelay[tmp0]);	//here is the second right channel delay
            rsB = rdelay[tmp1] + rdif1 * (rdelay[tmp0] - rsB );

            //Left Channel, delay A
            ldif0 = dlA - floor(dlA);
            tmp0 = (kl + (int) floor(dlA)) %  maxx_delay;
            tmp1 = tmp0 + 1;
            if (tmp1 >= maxx_delay) tmp1 =  0;
            //lsA = ldelay[tmp0] + ldif0 * (ldelay[tmp1] - ldelay[tmp0]);	//here is the first left channel delay
            lsA = ldelay[tmp1] + ldif0 * (ldelay[tmp0] - lsA );

            //Left Channel, delay B
            ldif1 = dlB - floor(dlB);
            tmp0 = (kl + (int) floor(dlB)) %  maxx_delay;
            tmp1 = tmp0 + 1;
            if (tmp1 >= maxx_delay) tmp1 =  0;
            //lsB = ldelay[tmp0] + ldif1 * (ldelay[tmp1] - ldelay[tmp0]);	//here is the second left channel delay
            lsB = ldelay[tmp1] + ldif1 * (ldelay[tmp0] - lsB );
            //End flanging, next process outputs

            if(Pzero) {
                efxoutl[i]= dry * smpsl[i] +  fsubtract * wet * (fsubtract * (lsA + lsB)  + zdl);    // Make final FX out mix
                efxoutr[i]= dry * smpsr[i] +  fsubtract * wet * (fsubtract * (rsA + rsB)  + zdr);
            } else {
                efxoutl[i]= dry * smpsl[i] +  wet * fsubtract * (lsA + lsB);    // Make final FX out mix
                efxoutr[i]= dry * smpsr[i] +  wet * fsubtract * (rsA + rsB);
            }




            if (--kl < 0)   //Cycle delay buffer in reverse so delay time can be indexed directly with addition
                kl =  maxx_delay;
            if (--kr < 0)
                kr =  maxx_delay;



// Increment LFO
            drA += rx0;
            drB += rx1;
            dlA += lx0;
            dlB += lx1;

        };  //end for loop

    }  //end intense if statement


};


/*
 * Parameter control
 */

void
Dflange::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        Pwetdry = value;
        dry = (DspFloatType) (Pwetdry+64) /128.0f;
        wet = 1.0f - dry;

        if(Psubtract) {
            ldelayline0->set_mix(-dry);
            rdelayline0->set_mix(-dry);
            ldelayline1->set_mix(-dry);
            rdelayline1->set_mix(-dry);
        } else {
            ldelayline0->set_mix(dry);
            rdelayline0->set_mix(dry);
            ldelayline1->set_mix(dry);
            rdelayline1->set_mix(dry);
        }

        break;
    case 1:
        Ppanning = value;
        if (value < 0) {
            rpan = 1.0f + (DspFloatType) Ppanning/64.0;
            lpan = 1.0f;
        } else {
            lpan = 1.0f - (DspFloatType) Ppanning/64.0;
            rpan = 1.0f;
        };
        break;
    case 2:
        Plrcross = value;
        flrcross = (DspFloatType) Plrcross/127.0;
        frlcross = 1.0f - flrcross;	//keep this out of the DSP loop
        break;
    case 3:
        Pdepth = value;
        fdepth =  (DspFloatType) Pdepth;
        zcenter = (int) fSAMPLE_RATE/floor(0.5f * (fdepth + fwidth));
        logmax = logf( (fdepth + fwidth)/fdepth )/LOG_2;
        break;
    case 4:
        Pwidth = value;
        fwidth = (DspFloatType) Pwidth;
        zcenter = (int) fSAMPLE_RATE/floor(0.5f * (fdepth + fwidth));
        logmax = logf( (fdepth + fwidth)/fdepth )/LOG_2;
        break;
    case 5:
        Poffset = value;
        foffset = 0.5f + (DspFloatType) Poffset/255.0;
        break;
    case 6:
        Pfb = value;
        ffb = (DspFloatType) Pfb/64.5f;
        break;
    case 7:
        Phidamp = value;
        fhidamp = f_exp(-D_PI * (DspFloatType) Phidamp/fSAMPLE_RATE);
        break;
    case 8:
        Psubtract = value;
        fsubtract = 0.5f;
        if(Psubtract) {
            fsubtract = -0.5f;  //In loop a mult by 0.5f is necessary, so this kills 2 birds with 1...
            ldelayline0->set_mix(-dry);
            rdelayline0->set_mix(-dry);
            ldelayline1->set_mix(-dry);
            rdelayline1->set_mix(-dry);
        }
        break;
    case 9:
        Pzero = value;
        if (Pzero) fzero = 1.0f;
        break;
    case 10:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 11:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 12:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 13:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 14:
        Pintense = value;
        break;
    };
};

int
Dflange::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pwetdry);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Plrcross);
        break;
    case 3:
        return (Pdepth);
        break;
    case 4:
        return (Pwidth);
        break;
    case 5:
        return (Poffset);
        break;
    case 6:
        return (Pfb);
        break;
    case 7:
        return (Phidamp);
        break;
    case 8:
        return (Psubtract);
        break;
    case 9:
        return (Pzero);
        break;
    case 10:
        return (lfo.Pfreq);
        break;
    case 11:
        return (lfo.Pstereo);
        break;
    case 12:
        return (lfo.PLFOtype);
        break;
    case 13:
        return (lfo.Prandomness);
        break;
    case 14:
        return Pintense;
        break;
    };
    return (0);			//in case of bogus parameter number
};


void
Dflange::setpreset (int npreset)
{
    const int PRESET_SIZE = 15;
    const int NUM_PRESETS = 9;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Preset 1
        {-32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0},
        //Flange-Wha
        {0, 0, 64, 500, 3000, 50, -40, 8000, 1, 0, 196, 96, 0, 0, 0},
        //FbFlange
        {0, 0, 64, 68, 75, 50, -50, 8000, 0, 1, 23, 96, 5, 0, 0},
        //SoftFlange
        {-32, 0, 64, 60, 10, 100, 20, 16000, 0, 0, 16, 90, 4, 0, 0},
        //Flanger
        {-32, 0, 64, 170, 1200, 50, 0, 16000, 1, 0, 68, 127, 0, 0, 0},
        //Chorus 1
        {-15, 0, 0, 42, 12, 50, -10, 1500, 0, 0, 120, 0, 0, 20, 0},
        //Chorus 2
        {-40, 0, 0, 35, 9, 67, 12, 4700, 1, 1, 160, 75, 0, 60, 0},
        //Preset 8
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        //Preset 9
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(20,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};



DynamicFilter::DynamicFilter (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    Ppreset = 0;
    filterl = NULL;
    filterr = NULL;
    filterpars = new FilterParams (0, 64, 64);
    setpreset (Ppreset);
    cleanup ();
};

DynamicFilter::~DynamicFilter ()
{
};


/*
 * Apply the effect
 */
void
DynamicFilter::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType lfol, lfor;

    if (filterpars->changed) {
        filterpars->changed = false;
        cleanup ();
    };

    lfo.effectlfoout (&lfol, &lfor);
    lfol *= depth * 5.0f;
    lfor *= depth * 5.0f;
    DspFloatType freq = filterpars->getfreq ();
    DspFloatType q = filterpars->getq ();

    for (i = 0; i < PERIOD; i++) {
        efxoutl[i] = smpsl[i];
        efxoutr[i] = smpsr[i];

        DspFloatType x = (fabsf (smpsl[i]) + fabsf (smpsr[i])) * 0.5f;
        ms1 = ms1 * (1.0f - ampsmooth) + x * ampsmooth + 1e-10f;
    };


    DspFloatType ampsmooth2 = powf (ampsmooth, 0.2f) * 0.3f;
    ms2 = ms2 * (1.0f - ampsmooth2) + ms1 * ampsmooth2;
    ms3 = ms3 * (1.0f - ampsmooth2) + ms2 * ampsmooth2;
    ms4 = ms4 * (1.0f - ampsmooth2) + ms3 * ampsmooth2;
    DspFloatType rms = (sqrtf (ms4)) * ampsns;

    DspFloatType frl = filterl->getrealfreq (freq + lfol + rms);
    DspFloatType frr = filterr->getrealfreq (freq + lfor + rms);

    filterl->setfreq_and_q (frl, q);
    filterr->setfreq_and_q (frr, q);


    filterl->filterout (efxoutl);
    filterr->filterout (efxoutr);

    //panning
    for (i = 0; i < PERIOD; i++) {
        efxoutl[i] *= panning;
        efxoutr[i] *= (1.0f - panning);
    };

};

/*
 * Cleanup the effect
 */
void
DynamicFilter::cleanup ()
{
    reinitfilter ();
    ms1 = 0.0;
    ms2 = 0.0;
    ms3 = 0.0;
    ms4 = 0.0;
};


/*
 * Parameter control
 */

void
DynamicFilter::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = powf (((DspFloatType)Pdepth / 127.0f), 2.0f);
};


void
DynamicFilter::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;

};

void
DynamicFilter::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + .5f) / 127.0f;
};


void
DynamicFilter::setampsns (int Pampsns)
{
    ampsns = powf ((DspFloatType)Pampsns / 127.0f, 2.5f) * 10.0f;
    if (Pampsnsinv != 0)
        ampsns = -ampsns;
    ampsmooth = expf ((DspFloatType)-Pampsmooth / 127.0f * 10.0f) * 0.99f;
    this->Pampsns = Pampsns;
};

void
DynamicFilter::reinitfilter ()
{
    if (filterl != NULL)
        delete (filterl);
    if (filterr != NULL)
        delete (filterr);
    filterl = new Filter (filterpars);
    filterr = new Filter (filterpars);
};

void
DynamicFilter::setpreset (int npreset)
{
    const int PRESET_SIZE = 10;
    const int NUM_PRESETS = 5;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //WahWah
        {64, 64, 80, 0, 0, 64, 70, 90, 0, 60},
        //AutoWah
        {64, 64, 70, 0, 0, 80, 70, 0, 0, 60},
        //Sweep
        {64, 64, 30, 0, 0, 50, 80, 0, 0, 60},
        //VocalMorph1
        {64, 64, 80, 0, 0, 64, 70, 64, 0, 60},
        //VocalMorph1
        {64, 64, 50, 0, 0, 96, 64, 0, 0, 60}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(10,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    filterpars->defaults ();
    switch (npreset) {
    case 0:
        filterpars->Pcategory = 0;
        filterpars->Ptype = 2;
        filterpars->Pfreq = 45;
        filterpars->Pq = 64;
        filterpars->Pstages = 1;
        filterpars->Pgain = 64;
        break;
    case 1:
        filterpars->Pcategory = 2;
        filterpars->Ptype = 0;
        filterpars->Pfreq = 72;
        filterpars->Pq = 64;
        filterpars->Pstages = 0;
        filterpars->Pgain = 64;
        break;
    case 2:
        filterpars->Pcategory = 0;
        filterpars->Ptype = 4;
        filterpars->Pfreq = 64;
        filterpars->Pq = 64;
        filterpars->Pstages = 2;
        filterpars->Pgain = 64;
        break;
    case 3:
        filterpars->Pcategory = 1;
        filterpars->Ptype = 0;
        filterpars->Pfreq = 50;
        filterpars->Pq = 70;
        filterpars->Pstages = 1;
        filterpars->Pgain = 64;

        filterpars->Psequencesize = 2;
        // "I"
        filterpars->Pvowels[0].formants[0].freq = 34;
        filterpars->Pvowels[0].formants[0].amp = 127;
        filterpars->Pvowels[0].formants[0].q = 64;
        filterpars->Pvowels[0].formants[1].freq = 99;
        filterpars->Pvowels[0].formants[1].amp = 122;
        filterpars->Pvowels[0].formants[1].q = 64;
        filterpars->Pvowels[0].formants[2].freq = 108;
        filterpars->Pvowels[0].formants[2].amp = 112;
        filterpars->Pvowels[0].formants[2].q = 64;
        // "A"
        filterpars->Pvowels[1].formants[0].freq = 61;
        filterpars->Pvowels[1].formants[0].amp = 127;
        filterpars->Pvowels[1].formants[0].q = 64;
        filterpars->Pvowels[1].formants[1].freq = 71;
        filterpars->Pvowels[1].formants[1].amp = 121;
        filterpars->Pvowels[1].formants[1].q = 64;
        filterpars->Pvowels[1].formants[2].freq = 99;
        filterpars->Pvowels[1].formants[2].amp = 117;
        filterpars->Pvowels[1].formants[2].q = 64;
        break;
    case 4:
        filterpars->Pcategory = 1;
        filterpars->Ptype = 0;
        filterpars->Pfreq = 64;
        filterpars->Pq = 70;
        filterpars->Pstages = 1;
        filterpars->Pgain = 64;

        filterpars->Psequencesize = 2;
        filterpars->Pnumformants = 2;
        filterpars->Pvowelclearness = 0;

        filterpars->Pvowels[0].formants[0].freq = 70;
        filterpars->Pvowels[0].formants[0].amp = 127;
        filterpars->Pvowels[0].formants[0].q = 64;
        filterpars->Pvowels[0].formants[1].freq = 80;
        filterpars->Pvowels[0].formants[1].amp = 122;
        filterpars->Pvowels[0].formants[1].q = 64;

        filterpars->Pvowels[1].formants[0].freq = 20;
        filterpars->Pvowels[1].formants[0].amp = 127;
        filterpars->Pvowels[1].formants[0].q = 64;
        filterpars->Pvowels[1].formants[1].freq = 100;
        filterpars->Pvowels[1].formants[1].amp = 121;
        filterpars->Pvowels[1].formants[1].q = 64;
        break;
    };

//          for (int i=0;i<5;i++){
//              printf("freq=%d  amp=%d  q=%d\n",filterpars->Pvowels[0].formants[i].freq,filterpars->Pvowels[0].formants[i].amp,filterpars->Pvowels[0].formants[i].q);
//          };

    Ppreset = npreset;

    reinitfilter ();
};


void
DynamicFilter::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setdepth (value);
        break;
    case 7:
        setampsns (value);
        break;
    case 8:
        Pampsnsinv = value;
        setampsns (Pampsns);
        break;
    case 9:
        Pampsmooth = value;
        setampsns (Pampsns);
        break;


    };
};

int
DynamicFilter::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pdepth);
        break;
    case 7:
        return (Pampsns);
        break;
    case 8:
        return (Pampsnsinv);
        break;
    case 9:
        return (Pampsmooth);
        break;
    default:
        return (0);
    };

};


PluginEQ::PluginEQ (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{

    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    for (int i = 0; i < MAX_EQ_BANDS; i++) {
        filter[i].Ptype = 0;
        filter[i].Pfreq = 64;
        filter[i].Pgain = 64;
        filter[i].Pq = 64;
        filter[i].Pstages = 0;
        filter[i].l = new AnalogFilter (6, 1000.0f, 1.0f, 0);
        filter[i].r = new AnalogFilter (6, 1000.0f, 1.0f, 0);
    };
    //default values
    Ppreset = 0;
    Pvolume = 50;

    setpreset (Ppreset);
    cleanup ();
};

PluginEQ::~PluginEQ ()
{
};

/*
 * Cleanup the effect
 */
void
PluginEQ::cleanup ()
{
    for (int i = 0; i < MAX_EQ_BANDS; i++) {
        filter[i].l->cleanup ();
        filter[i].r->cleanup ();
    };
};



/*
 * Effect output
 */
void
PluginEQ::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    for (i = 0; i < MAX_EQ_BANDS; i++) {
        if (filter[i].Ptype == 0)
            continue;
        filter[i].l->filterout (efxoutl);
        filter[i].r->filterout (efxoutr);
    };


    for (i = 0; i < PERIOD; i++) {
        efxoutl[i] = smpsl[i] * outvolume;
        efxoutr[i] = smpsr[i] * outvolume;
    };

};


/*
 * Parameter control
 */
void
PluginEQ::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;

    outvolume = powf (0.005f, (1.0f - (DspFloatType)Pvolume / 127.0f)) * 10.0f;


};


void
PluginEQ::setpreset (int npreset)
{
    const int PRESET_SIZE = 1;
    const int NUM_PRESETS = 2;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //EQ 1
        {67},
        //EQ 2
        {67}
    };

    for (int n = 0; n < PRESET_SIZE; n++)
        changepar (n, presets[npreset][n]);
    Ppreset = npreset;
};


void
PluginEQ::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    };
    if (npar < 10)
        return;

    int nb = (npar - 10) / 5;	//number of the band (filter)
    if (nb >= MAX_EQ_BANDS)
        return;
    int bp = npar % 5;		//band paramenter

    DspFloatType tmp;
    switch (bp) {
    case 0:
        if (value > 9)
            value = 0;		//has to be changed if more filters will be added
        filter[nb].Ptype = value;
        if (value != 0) {
            filter[nb].l->settype (value - 1);
            filter[nb].r->settype (value - 1);
        };
        break;
    case 1:
        filter[nb].Pfreq = value;
        tmp = (DspFloatType)value;
        filter[nb].l->setfreq (tmp);
        filter[nb].r->setfreq (tmp);
        break;
    case 2:
        filter[nb].Pgain = value;
        tmp = 30.0f * ((DspFloatType)value - 64.0f) / 64.0f;
        filter[nb].l->setgain (tmp);
        filter[nb].r->setgain (tmp);
        break;
    case 3:
        filter[nb].Pq = value;
        tmp = powf (30.0f, ((DspFloatType)value - 64.0f) / 64.0f);
        filter[nb].l->setq (tmp);
        filter[nb].r->setq (tmp);
        break;
    case 4:
        if (value >= MAX_FILTER_STAGES)
            value = MAX_FILTER_STAGES - 1;
        filter[nb].Pstages = value;
        filter[nb].l->setstages (value);
        filter[nb].r->setstages (value);
        break;
    };
};

int
PluginEQ::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    };

    if (npar < 10)
        return (0);

    int nb = (npar - 10) / 5;	//number of the band (filter)
    if (nb >= MAX_EQ_BANDS)
        return (0);
    int bp = npar % 5;		//band paramenter
    switch (bp) {
    case 0:
        return (filter[nb].Ptype);
        break;
    case 1:
        return (filter[nb].Pfreq);
        break;
    case 2:
        return (filter[nb].Pgain);
        break;
    case 3:
        return (filter[nb].Pq);
        break;
    case 4:
        return (filter[nb].Pstages);
        break;
    };

    return (0);			//in case of bogus parameter number
};




DspFloatType PluginEQ::getfreqresponse (DspFloatType freq)
{
    DspFloatType
    resp = 1.0f;

    for (int i = 0; i < MAX_EQ_BANDS; i++) {
        if (filter[i].Ptype == 0)
            continue;
        resp *= filter[i].l->H (freq);
    };
    return (rap2dB (resp * outvolume));
};


Echo::Echo (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;
    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Pdelay = 60;
    Plrdelay = 100;
    Plrcross = 100;
    Pfb = 40;
    Phidamp = 60;

    lrdelay = 0;
    Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
    maxx_delay = SAMPLE_RATE * MAX_DELAY;
    fade = SAMPLE_RATE / 5;    //1/5 SR fade time available

    ldelay = new delayline(2.0f, 1);
    rdelay = new delayline(2.0f, 1);

    setpreset (Ppreset);
    cleanup ();
};

Echo::~Echo ()
{
};

/*
 * Cleanup the effect
 */
void
Echo::cleanup ()
{
    ldelay->cleanup();
    rdelay->cleanup();
    ldelay->set_averaging(0.25f);
    rdelay->set_averaging(0.25f);
    oldl = 0.0;
    oldr = 0.0;
};


/*
 * Initialize the delays
 */
void
Echo::initdelays ()
{

    oldl = 0.0;
    oldr = 0.0;
    ltime = delay + lrdelay;
    rtime = delay - lrdelay;

    if(ltime > 2.0f) ltime = 2.0f;
    if(ltime<0.01f) ltime = 0.01f;

    if(rtime > 2.0f) rtime = 2.0f;
    if(rtime<0.01f) rtime = 0.01f;
};

/*
 * Effect output
 */
void
Echo::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType l, r, ldl, rdl, ldlout, rdlout, rvl, rvr;

    for (i = 0; i < PERIOD; i++) {

        ldl = ldelay->delay_simple(oldl, ltime, 0, 1, 0);
        rdl = rdelay->delay_simple(oldr, rtime, 0, 1, 0);

        if(Preverse) {
            rvl = ldelay->delay_simple(oldl, ltime, 1, 0, 1)*ldelay->envelope();
            rvr = rdelay->delay_simple(oldr, rtime, 1, 0, 1)*rdelay->envelope();
            ldl = ireverse*ldl + reverse*rvl;
            rdl = ireverse*rdl + reverse*rvr;
        }

        l = ldl * (1.0f - lrcross) + rdl * lrcross;
        r = rdl * (1.0f - lrcross) + ldl * lrcross;
        ldl = l;
        rdl = r;

        ldlout = -ldl*fb;
        rdlout = -rdl*fb;
        if (!Pdirect) {
            l = ldl = smpsl[i] * panning + ldlout;
            r = rdl = smpsr[i] * (1.0f - panning) + rdlout;
        } else {
            ldl = smpsl[i] * panning + ldlout;
            rdl = smpsr[i] * (1.0f - panning) + rdlout;
        }

        efxoutl[i]= l;
        efxoutr[i]= r;

        //LowPass Filter
        oldl = ldl * hidamp + oldl * (1.0f - hidamp);
        oldr = rdl * hidamp + oldr * (1.0f - hidamp);
        oldl += DENORMAL_GUARD;
        oldr += DENORMAL_GUARD;

    };

};


/*
 * Parameter control
 */
void
Echo::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;

};

void
Echo::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
};

void
Echo::setreverse (int Preverse)
{
    this->Preverse = Preverse;
    reverse = (DspFloatType) Preverse / 127.0f;
    ireverse = 1.0f - reverse;
};

void
Echo::Tempo2Delay(int value)
{

    Pdelay = 60.0f/(DspFloatType)value * 1000.0f;
    delay = (DspFloatType)Pdelay / 1000.0f;
    if ((unsigned int) delay > (MAX_DELAY)) delay = MAX_DELAY;
    ldelay->set_averaging(10.0f);
    rdelay->set_averaging(10.0f);
    initdelays();
}

void
Echo::setdelay (int Pdelay)
{
    this->Pdelay = Pdelay;
    delay= ((DspFloatType) Pdelay)/1000.0f;
    ldelay->set_averaging(0.05f);
    rdelay->set_averaging(0.05f);
    initdelays ();
};

void
Echo::setlrdelay (int Plrdelay)
{
    DspFloatType tmp;
    this->Plrdelay = Plrdelay;
    tmp =
        (powf (2.0, fabsf ((DspFloatType)Plrdelay - 64.0f) / 64.0f * 9.0f) -
         1.0f) / 1000.0f;
    if (Plrdelay < 64.0)
        tmp = -tmp;
    lrdelay = tmp;
    initdelays ();
};

void
Echo::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f * 1.0f;
};

void
Echo::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = (DspFloatType)Pfb / 128.0f;
};

void
Echo::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (DspFloatType)Phidamp / 127.0f;
};

void
Echo::setdirect (int Pdirect)
{
    if (Pdirect > 0)
        Pdirect = 1;
    this->Pdirect = Pdirect;
};

void
Echo::setpreset (int npreset)
{
    const int PRESET_SIZE = 9;
    const int NUM_PRESETS = 9;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {67, 64, 565, 64, 30, 59, 0, 127, 0},
        //Echo 2
        {67, 64, 357, 64, 30, 59, 0, 64, 0},
        //Echo 3
        {67, 75, 955, 64, 30, 59, 10, 0, 0},
        //Simple Echo
        {67, 60, 705, 64, 30, 0, 0, 0, 0},
        //Canyon
        {67, 60, 1610, 50, 30, 82, 48, 0, 0},
        //Panning Echo 1
        {67, 64, 705, 17, 0, 82, 24, 0, 0},
        //Panning Echo 2
        {81, 60, 737, 118, 100, 68, 18, 0, 0},
        //Panning Echo 3
        {81, 60, 472, 100, 127, 67, 36, 0, 0},
        //Feedback Echo
        {62, 64, 456, 64, 100, 90, 55, 0, 0}
    };


    for (int n = 0; n < PRESET_SIZE; n++)
        changepar (n, presets[npreset][n]);
    Ppreset = npreset;
};


void
Echo::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setdelay (value);
        break;
    case 3:
        setlrdelay (value);
        break;
    case 4:
        setlrcross (value);
        break;
    case 5:
        setfb (value);
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        setreverse (value);
        break;
    case 8:
        setdirect (value);
        break;
    };
};

int
Echo::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Pdelay);
        break;
    case 3:
        return (Plrdelay);
        break;
    case 4:
        return (Plrcross);
        break;
    case 5:
        return (Pfb);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return (Preverse);
        break;
    case 8:
        return (Pdirect);
        break;

    };
    return (0);			//in case of bogus parameter number
};


Echotron::Echotron (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    initparams=0;
    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Plrcross = 100;
    Phidamp = 60;
    Filenum = 0;
    Plength = 10;
    Puser = 0;
    fb = 0.0f;
    lfeedback = 0.0f;
    rfeedback = 0.0f;
    subdiv_dmod = 1.0f;
    subdiv_fmod = 1.0f;
    f_qmode = 0;

    maxx_size = (SAMPLE_RATE * 6);   //6 Seconds delay time

    lxn = new delayline(6.0f, ECHOTRON_F_SIZE);
    rxn = new delayline(6.0f, ECHOTRON_F_SIZE);

    lxn->set_mix(0.0f);
    rxn->set_mix(0.0f);

    offset = 0;

    lpfl =  new AnalogFilter (0, 800, 1, 0);;
    lpfr =  new AnalogFilter (0, 800, 1, 0);;

    DspFloatType center, qq;
    for (int i = 0; i < ECHOTRON_MAXFILTERS; i++) {
        center = 500;
        qq = 1.0f;
        filterbank[i].sfreq = center;
        filterbank[i].sq = qq;
        filterbank[i].sLP = 0.25f;
        filterbank[i].sBP = -1.0f;
        filterbank[i].sHP = 0.5f;
        filterbank[i].sStg = 1.0f;
        filterbank[i].l = new RBFilter (0, center, qq, 0);
        filterbank[i].r = new RBFilter (0, center, qq, 0);

        filterbank[i].l->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
        filterbank[i].r->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
    };

    setpreset (Ppreset);
    cleanup ();
};

Echotron::~Echotron ()
{
};

/*
 * Cleanup the effect
 */
void
Echotron::cleanup ()
{

    lxn->cleanup();
    rxn->cleanup();
    lxn->set_averaging(0.05f);
    rxn->set_averaging(0.05f);

    lpfl->cleanup ();
    lpfr->cleanup ();

};

/*
 * Effect output
 */
void
Echotron::out (DspFloatType * smpsl, DspFloatType * smpsr)
{

    int i, j, k;
    int length = Plength;
    DspFloatType l,r,lyn, ryn;
    DspFloatType rxindex,lxindex;


    if((Pmoddly)||(Pmodfilts)) modulate_delay();
    else interpl = interpr = 0;

    DspFloatType tmpmodl = oldldmod;
    DspFloatType tmpmodr = oldrdmod;

    for (i = 0; i < PERIOD; i++) {
        tmpmodl+=interpl;
        tmpmodr+=interpr;

        l = lxn->delay( (lpfl->filterout_s(smpsl[i] + lfeedback) ), 0.0f, 0, 1, 0);  //High Freq damping
        r = rxn->delay( (lpfr->filterout_s(smpsr[i] + rfeedback) ), 0.0f, 0, 1, 0);

        //Convolve
        lyn = 0.0f;
        ryn = 0.0f;

        if(Pfilters) {

            j=0;
            for (k=0; k<length; k++) {
                lxindex = ltime[k] + tmpmodl;
                rxindex = rtime[k] + tmpmodr;

                if((iStages[k]>=0)&&(j<ECHOTRON_MAXFILTERS)) {
                    lyn += filterbank[j].l->filterout_s(lxn->delay(l, lxindex, k, 0, 0)) * ldata[k];		//filter each tap specified
                    ryn += filterbank[j].r->filterout_s(rxn->delay(r, lxindex, k, 0, 0)) * rdata[k];
                    j++;
                } else {
                    lyn += lxn->delay(l, lxindex, k, 0, 0) * ldata[k];
                    ryn += rxn->delay(r, rxindex, k, 0, 0) * rdata[k];
                }

            }

        } else {
            for (k=0; k<length; k++) {
                lxindex = ltime[k] + tmpmodl;
                rxindex = rtime[k] + tmpmodr;

                lyn += lxn->delay(l, lxindex, k, 0, 0) * ldata[k];
                ryn += rxn->delay(r, rxindex, k, 0, 0) * rdata[k];
            }

        }

        lfeedback =  (lrcross*ryn + ilrcross*lyn) * lpanning;
        rfeedback = (lrcross*lyn + ilrcross*ryn) * rpanning;
        efxoutl[i] = lfeedback;
        efxoutr[i] = rfeedback;
        lfeedback *= fb;
        rfeedback *= fb;

    };

    if(initparams) init_params();

};


/*
 * Parameter control
 */
void
Echotron::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Echotron::setpanning (int value)
{
    Ppanning = value;
    rpanning = ((DspFloatType)Ppanning) / 64.0f;
    lpanning = 2.0f - rpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
    rpanning = 1.0f - 1.0f/(rpanning + 1.0f);
    lpanning *= 1.1f;
    rpanning *= 1.1f;
    if(lpanning>1.0f) lpanning = 1.0f;
    if(rpanning>1.0f) rpanning = 1.0f;
};

int
Echotron::setfile(int value)
{
    DspFloatType tPan=0.0f;
    DspFloatType tTime=0.0f;
    DspFloatType tLevel=0.0f;
    DspFloatType tLP=0.0f;
    DspFloatType tBP=0.0f;
    DspFloatType tHP=0.0f;
    DspFloatType tFreq=20.0f;
    DspFloatType tQ=1.0f;
    int tiStages = 0;

    FILE *fs;

    char wbuf[128];

    if(!Puser) {
        Filenum = value;
        memset(Filename,0,sizeof(Filename));
        sprintf(Filename, "%s/%d.dly",DATADIR,Filenum+1);
    }

    if ((fs = fopen (Filename, "r")) == NULL) {
        loaddefault();
        return(0);
    }

    while (fgets(wbuf,sizeof wbuf,fs) != NULL) {
        //fgets(wbuf,sizeof wbuf,fs);
        if(wbuf[0]!='#') break;
        memset(wbuf,0,sizeof(wbuf));
    }

    sscanf(wbuf,"%f\t%f\t%d",&subdiv_fmod,&subdiv_dmod,&f_qmode); //Second line has tempo subdivision
//printf("subdivs:\t%f\t%f\n",subdiv_fmod,subdiv_dmod);

    int count = 0;
    memset(iStages,0,sizeof(iStages));



    while ((fgets(wbuf,sizeof wbuf,fs) != NULL) && (count<ECHOTRON_F_SIZE)) {
        if(wbuf[0]==10) break;  // Check Carriage Return
        if(wbuf[0]=='#') continue;
        sscanf(wbuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d",&tPan, &tTime, &tLevel,
               &tLP,  &tBP,  &tHP,  &tFreq,  &tQ,  &tiStages);
        //printf("params:\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",tPan, tTime, tLevel,
        //tLP,  tBP,  tHP,  tFreq,  tQ,  tiStages);
        if((tPan<-1.0f) || (tPan>1.0f)) {
            error_num=5;
            break;
        } else fPan[count]=tPan;

        if((tTime <-6.0) || (tTime>6.0f)) {
            error_num=6;
            break;
        } else fTime[count]=fabs(tTime);

        if((tLevel <-10.0f) || (tLevel>10.0f)) {
            error_num=7;
            break;
        } else fLevel[count]=tLevel;

        if((tLP <-2.0f) || (tLP>2.0f)) {
            error_num=8;
            break;
        } else fLP[count]=tLP;

        if((tBP<-2.0f) || (tBP>2.0f)) {
            error_num=9;
            break;
        } else fBP[count]=tBP;

        if((tHP<-2.0f) || (tHP>2.0f)) {
            error_num=10;
            break;
        } else fHP[count]=tHP;

        if((tFreq <20.0f) || (tFreq>26000.0f)) {
            error_num=11;
            break;
        } else fFreq[count]=tFreq;

        if((tQ <0.0) || (tQ>300.0f)) {
            error_num=12;
            break;
        } else fQ[count]=tQ;

        if((tiStages<0) || (tiStages>MAX_FILTER_STAGES)) {
            error_num=13;
            break;
        } else iStages[count]=tiStages-1;   //check in main loop if <0, then skip filter


        memset(wbuf,0,sizeof(wbuf));
        count++;
    }
    fclose(fs);

    if(!Pchange) Plength=count;
    cleanup();
    init_params();
    return(1);
};

void
Echotron::loaddefault()
{
    Plength = 1;
    fPan[0] = 0.0f;  //
    fTime[0] = 1.0f;  //default 1 measure delay
    fLevel[0] = 0.7f;
    fLP[0] = 1.0f;
    fBP[0] = -1.0f;
    fHP[0] = 1.0f;
    fFreq[0]=800.0f;;
    fQ[0]=2.0f;
    iStages[0]=1;
    subdiv_dmod = 1.0f;
    subdiv_fmod = 1.0f;
    init_params();
}

void Echotron::init_params()
{

    DspFloatType hSR = fSAMPLE_RATE*0.5f;
    DspFloatType tpanl, tpanr;
    DspFloatType tmptempo;
    int tfcnt = 0;

    initparams=0;
    depth = ((DspFloatType) (Pdepth - 64))/64.0f;
    dlyrange = 0.008*f_pow2(4.5f*depth);
    width = ((DspFloatType) Pwidth)/127.0f;

    tmptempo = (DspFloatType) Ptempo;
    lfo.Pfreq = lrintf(subdiv_fmod*tmptempo);
    dlfo.Pfreq = lrintf(subdiv_dmod*tmptempo);

    for(int i=0; i<Plength; i++) {
// tmp_time=lrintf(fTime[i]*tempo_coeff*fSAMPLE_RATE);
// if(tmp_time<maxx_size) rtime[i]=tmp_time; else rtime[i]=maxx_size;

        ltime[i] = rtime[i] = fTime[i]*tempo_coeff;


        if(fPan[i]>=0.0f) {
            tpanr = 1.0;
            tpanl = 1.0f - fPan[i];
        } else {
            tpanl = 1.0;
            tpanr = 1.0f + fPan[i];
        }

        ldata[i]=fLevel[i]*tpanl;
        rdata[i]=fLevel[i]*tpanr;

        if((tfcnt<ECHOTRON_MAXFILTERS)&&(iStages[i]>=0)) {
            int Freq=fFreq[i]*f_pow2(depth*4.5f);
            if (Freq<20.0) Freq=20.0f;
            if (Freq>hSR) Freq=hSR;
            filterbank[tfcnt].l->setfreq_and_q(Freq,fQ[i]);
            filterbank[tfcnt].r->setfreq_and_q(Freq,fQ[i]);
            filterbank[tfcnt].l->setstages(iStages[i]);
            filterbank[tfcnt].r->setstages(iStages[i]);
            filterbank[tfcnt].l->setmix (1, fLP[i] , fBP[i], fHP[i]);
            filterbank[tfcnt].r->setmix (1, fLP[i] , fBP[i], fHP[i]);
            filterbank[tfcnt].l->setmode(f_qmode);
            filterbank[tfcnt].r->setmode(f_qmode);
            tfcnt++;
        }
    }

};

void Echotron::modulate_delay()
{

    DspFloatType lfmod, rfmod, lfol, lfor, dlfol, dlfor;
    DspFloatType fperiod = 1.0f/fPERIOD;

    lfo.effectlfoout (&lfol, &lfor);
    dlfo.effectlfoout (&dlfol, &dlfor);
    if(Pmodfilts) {
        lfmod = f_pow2((lfol*width + 0.25f + depth)*4.5f);
        rfmod = f_pow2((lfor*width + 0.25f + depth)*4.5f);
        for(int i=0; i<ECHOTRON_MAXFILTERS; i++) {

            filterbank[i].l->setfreq(lfmod*fFreq[i]);
            filterbank[i].r->setfreq(rfmod*fFreq[i]);

        }

    }

    if(Pmoddly) {
        oldldmod = ldmod;
        oldrdmod = rdmod;
        ldmod = width*dlfol;
        rdmod = width*dlfor;

// ldmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*ldmod);
// rdmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*rdmod);
        ldmod=dlyrange*tempo_coeff*ldmod;
        rdmod=dlyrange*tempo_coeff*rdmod;
        interpl = (ldmod - oldldmod)*fperiod;
        interpr = (rdmod - oldrdmod)*fperiod;
    } else {
        oldldmod = 0.0f;
        oldrdmod = 0.0f;
        ldmod = 0.0f;
        rdmod = 0.0f;
        interpl = 0.0f;
        interpr = 0.0f;
    }

};


void
Echotron::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (DspFloatType)Phidamp / 127.1f;
    DspFloatType fr = 20.0f*f_pow2(hidamp*10.0f);
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Echotron::setfb(int value)
{

    fb = (DspFloatType)value/64.0f;
}


void
Echotron::setpreset (int npreset)
{
    const int PRESET_SIZE = 16;
    const int NUM_PRESETS = 5;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Summer
        {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1},
        //Ambience
        {96, 64, 16, 4, 0, 180, 50, 64, 1, 96, -4, 64, 1, 0, 0, 0},
        //Arranjer
        {64, 64, 10, 4, 0, 400, 32, 64, 1, 96, -8, 64, 1, 0, 0, 0},
        //Suction
        {0, 47, 28, 8, 0, 92, 0, 64, 3, 32, 0, 64, 1, 1, 1, 1},
        //SucFlange
        {64, 36, 93, 8, 0, 81, 0, 64, 3, 32, 0, 64, 1, 0, 1, 1}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(41,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Echotron::changepar (int npar, int value)
{
    DspFloatType tmptempo;
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        Pdepth=value;
        initparams=1;
        break;
    case 2:
        Pwidth=value;
        initparams=1;
        break;
    case 3:
        Plength = value;
        if(Plength>127) Plength = 127;
        initparams=1;
        break;
    case 4:
        Puser = value;
        break;
    case 5:
        Ptempo = value;

        tmptempo = (DspFloatType) Ptempo;
        tempo_coeff = 60.0f / tmptempo;
        lfo.Pfreq = lrintf(subdiv_fmod*tmptempo);
        dlfo.Pfreq = lrintf(subdiv_dmod*tmptempo);
        lfo.updateparams ();
        initparams=1;
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        Plrcross = value;
        lrcross = ((DspFloatType)(Plrcross)-64)/64.0;
        ilrcross = 1.0f - abs(lrcross);
        break;
    case 8:
        if(!setfile(value)) error_num=4;
        break;
    case 9:
        lfo.Pstereo = value;
        dlfo.Pstereo = value;
        lfo.updateparams ();
        dlfo.updateparams ();
        break;
    case 10:
        Pfb = value;
        setfb(value);
        break;
    case 11:
        setpanning (value);
        break;
    case 12:
        Pmoddly = value;//delay modulation on/off
        break;
    case 13:
        Pmodfilts = value;//filter modulation on/off
        if(!Pmodfilts) initparams=1;
        break;
    case 14:
        //LFO Type
        lfo.PLFOtype = value;
        lfo.updateparams ();
        dlfo.PLFOtype = value;
        dlfo.updateparams ();
        break;
    case 15:
        Pfilters = value;//Pfilters
        break;
    };
};

int
Echotron::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pdepth);
        break;
    case 2:
        return(Pwidth);
        break;
    case 3:
        return(Plength);
        break;
    case 8:
        return (Filenum);
        break;
    case 5:
        return (Ptempo);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return(Plrcross);
        break;
    case 4:
        return(Puser);
        break;
    case 9:
        return(lfo.Pstereo);
        break;
    case 10:
        return(Pfb);
        break;
    case 11:
        return(Ppanning);
        break;
    case 12:
        return(Pmoddly);  //modulate delays
        break;
    case 13:
        return(Pmodfilts); //modulate filters
        break;
    case 14:
        return(lfo.PLFOtype);
        break;
    case 15:
        return(Pfilters);  //Filter delay line on/off
        break;


    };
    return (0);			//in case of bogus parameter number
};


RBFilter::RBFilter (int Ftype, DspFloatType Ffreq, DspFloatType Fq,
                    int Fstages)
{
    stages = Fstages;
    type = Ftype;
    freq = Ffreq;
    q = Fq;
    qmode = 0;
    gain = 1.0f;
    outgain = 1.0f;
    needsinterpolation = 0;
    firsttime = 1;
    en_mix = 0;
    oldq = 0.0f;
    oldsq = 0.0f;
    oldf = 0.0f;
    hpg = lpg = bpg = 0.0f;
    if (stages >= MAX_FILTER_STAGES)
        stages = MAX_FILTER_STAGES;
    cleanup ();
    setfreq_and_q (Ffreq, Fq);
    iper = 1.0f/fPERIOD;
    a_smooth_tc = cSAMPLE_RATE/(cSAMPLE_RATE + 0.01f);  //10ms time constant for averaging coefficients
    b_smooth_tc = 1.0f - a_smooth_tc;
};

RBFilter::~RBFilter ()
{
};

void
RBFilter::cleanup ()
{
    for (int i = 0; i < MAX_FILTER_STAGES + 1; i++) {
        st[i].low = 0.0;
        st[i].high = 0.0;
        st[i].band = 0.0;
        st[i].notch = 0.0;
    };
    oldabovenq = 0;
    abovenq = 0;
};

void
RBFilter::computefiltercoefs ()
{
    par.f = 2.0f * sinf(PI*freq / fSAMPLE_RATE);
    if (par.f > 0.99999f)
        par.f = 0.99999f;
    par.q = 1.0f - atanf (sqrtf (q)) * 2.0f / PI;
    par.q = powf (par.q, 1.0f / (DspFloatType)(stages + 1));
    par.q_sqrt = sqrtf (par.q);

};

void
RBFilter::computefiltercoefs_hiQ ()  //potentially unstable at some settings, but better sound
{
    par.f = 2.0f * sinf(PI*freq / fSAMPLE_RATE);
    if (par.f > 0.99999f)
        par.f = 0.99999f;
    if(q<0.5f) q = 0.5f;
    par.q = 1.0f/q;
    par.q = powf (par.q, 1.0f / (DspFloatType)(stages + 1));
    par.q_sqrt = 1.0f;

};

void
RBFilter::directmod (DspFloatType lfo)  //potentially unstable at some settings, but better sound
{
    par.f = fabsf(lfo);  //cannot be less than 0
    if (par.f > 0.99999f)
        par.f = 0.99999f;
    //note range on input LFO should be scaled assuming the following:
    //A value of 1.0f sets a resonant frequency of SAMPLE_RATE/6.0
    //A value greater than 1 makes the filter go unstable, thus the limit.
    //Call the filter on the same sample multiple times for cheap and dirty oversampling
};
void
RBFilter::setmode (int mode)
{
    if (mode) qmode = 1;
    else qmode = 0;
}

void
RBFilter::setfreq (DspFloatType frequency)
{
    if (frequency > (fSAMPLE_RATE / 2.0f - 500.0f)) frequency = fSAMPLE_RATE / 2.0f - 500.0f;
    if (frequency < 0.1)
        frequency = 0.1f;
    DspFloatType rap = freq / frequency;
    if (rap < 1.0)
        rap = 1.0f / rap;

    oldabovenq = abovenq;
    abovenq = frequency > (fSAMPLE_RATE / 2.0f - 500.0f);

    int nyquistthresh = (abovenq ^ oldabovenq);


    if ((rap > 3.0) || (nyquistthresh != 0)) {
        //if the frequency is changed fast, it needs interpolation (now, filter and coeficients backup)
        if (firsttime == 0)
            needsinterpolation = 1;
        ipar = par;
    };
    freq = frequency;

    if(!qmode) computefiltercoefs ();
    else computefiltercoefs_hiQ ();
    firsttime = 0;

};

void
RBFilter::setfreq_and_q (DspFloatType frequency, DspFloatType q_)
{
    q = q_;
    setfreq (frequency);
};

void
RBFilter::setq (DspFloatType q_)
{
    q = q_;
    if(!qmode) computefiltercoefs ();
    else computefiltercoefs_hiQ ();
};

void
RBFilter::settype (int type_)
{
    type = type_;
    if(!qmode) computefiltercoefs ();
    else computefiltercoefs_hiQ ();
};

void
RBFilter::setgain (DspFloatType dBgain)
{
    gain = dB2rap (dBgain);
    if(!qmode) computefiltercoefs ();
    else computefiltercoefs_hiQ ();
};

void
RBFilter::setstages (int stages_)
{
    if (stages_ >= MAX_FILTER_STAGES)
        stages_ = MAX_FILTER_STAGES - 1;
    stages = stages_;
    cleanup ();
    if(!qmode) computefiltercoefs ();
    else computefiltercoefs_hiQ ();
};
void
RBFilter::setmix (int mix, DspFloatType lpmix, DspFloatType bpmix, DspFloatType hpmix)
{
    if(mix) {
        en_mix = 1;
    } else {
        en_mix = 0;
    }

    lpg = lpmix;
    bpg = bpmix;
    hpg = hpmix;
};


void
RBFilter::singlefilterout (DspFloatType * smp, fstage & x, parameters & par)
{
    int i;
    DspFloatType *out = NULL;
    switch (type) {
    case 0:
        out = &x.low;
        break;
    case 1:
        out = &x.high;
        break;
    case 2:
        out = &x.band;
        break;
    case 3:
        out = &x.notch;
        break;
    };

    DspFloatType tmpq, tmpsq, tmpf, qdiff, sqdiff, fdiff;
    qdiff = (par.q - oldq)*iper;
    sqdiff = (par.q_sqrt - oldsq)*iper;
    fdiff = (par.f - oldf)*iper;
    tmpq = oldq;
    tmpsq = oldsq;
    tmpf = oldf;

    for (i = 0; i < PERIOD; i++) {
        tmpq += qdiff;
        tmpsq += sqdiff;
        tmpf += fdiff;   //Modulation interpolation

        x.low = x.low + tmpf * x.band;
        x.high = tmpsq * smp[i] - x.low - tmpq * x.band;
        //x.high = smp[i] - x.low - tmpq * x.band;
        x.band = tmpf * x.high + x.band;

        if(en_mix) {
            smp[i] = lpg * x.low + hpg * x.high + bpg * x.band;
        } else {
            x.notch = x.high + x.low;
            smp[i] = *out;
        }
    };

    oldf = par.f;
    oldq = par.q;
    oldsq = par.q_sqrt;


};

void
RBFilter::filterout (DspFloatType * smp)
{
    int i;
    DspFloatType *ismp = NULL;

    if (needsinterpolation != 0) {
        ismp = new DspFloatType[PERIOD];
        for (i = 0; i < PERIOD; i++)
            ismp[i] = smp[i];
        for (i = 0; i < stages + 1; i++)
            singlefilterout (ismp, st[i], ipar);

        delete (ismp);
        needsinterpolation = 0;
    };

    for (i = 0; i < stages + 1; i++)
        singlefilterout (smp, st[i], par);


    for (i = 0; i < PERIOD; i++)
        smp[i] *= outgain;

};

DspFloatType
RBFilter::filterout_s (DspFloatType smp)
{
    int i;

    if (needsinterpolation != 0) {
        for (i = 0; i < stages + 1; i++)
            smp=singlefilterout_s (smp, st[i], ipar);
        needsinterpolation = 0;
    };

    for (i = 0; i < stages + 1; i++)
        smp=singlefilterout_s (smp, st[i], par);


    return(smp *= outgain);

};

DspFloatType inline
RBFilter::singlefilterout_s (DspFloatType smp, fstage & x, parameters & par)
{
    DspFloatType *out = NULL;
    switch (type) {
    case 0:
        out = &x.low;
        break;
    case 1:
        out = &x.high;
        break;
    case 2:
        out = &x.band;
        break;
    case 3:
        out = &x.notch;
        break;
    };


    oldq = b_smooth_tc*oldq + a_smooth_tc*par.q;
    oldsq = b_smooth_tc*oldsq + a_smooth_tc*par.q_sqrt;
    oldf = b_smooth_tc*oldf + a_smooth_tc*par.f;   //modulation interpolation

    x.low = x.low + oldf * x.band;
    x.high = oldsq * smp - x.low - oldq * x.band;
    x.band = oldf * x.high + x.band;

    if(en_mix) {
        smp = lpg * x.low + hpg * x.high + bpg * x.band;
    } else {
        x.notch = x.high + x.low;
        smp = *out;
    }

    oldf = par.f;
    oldq = par.q;
    oldsq = par.q_sqrt;

    return(smp);
};


Exciter::Exciter (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    //default values
    Ppreset = 0;
    Pvolume = 50;
    lpffreq = 8000;
    hpffreq = 2500;

    for(int i=0; i<10; i++) {
        Prm[i]=0;
        rm[i]=0.0f;
    }

    harm = new HarmEnhancer (rm, 2500.0f,8000.0,1.0f);

    cleanup ();

    setpreset (Ppreset);
};

Exciter::~Exciter ()
{
};

/*
 * Cleanup the effect
 */
void
Exciter::cleanup ()
{
    harm->cleanup ();
};


/*
 * Effect output
 */
void
Exciter::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    harm->harm_out(smpsl,smpsr);

};


/*
 * Parameter control
 */
void
Exciter::setvolume (int value)
{
    Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    harm->set_vol(0,outvolume);
};

void
Exciter::setlpf (int value)
{
    lpffreq=value;
    harm->set_freql (0, (DspFloatType)value);
};

void
Exciter::sethpf (int value)
{
    hpffreq=value;
    harm->set_freqh (0,(DspFloatType)value);
};

void
Exciter::sethar(int num,int value)
{
    DspFloatType har=32.0f*((DspFloatType)num+1.0f);
    if (num%2==1) har=11200.0f-64.0f*((DspFloatType)num+1.0f);
    Prm[num]=value;
    rm[num]= (DspFloatType)value/har;
    harm->calcula_mag(rm);
}



void
Exciter::setpreset (int npreset)
{
    const int PRESET_SIZE = 13;
    const int NUM_PRESETS = 5;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20},
        //Loudness
        {64, 0, 0, 0, 0, 24, 0, -24, 0, 24, 0, 80, 20},
        //Exciter 1
        {64, 64, 0, 0, 0, 64, 0, 0, 0, 64, 0, 20000, 20},
        //Exciter 2
        {64, 0, 0, 0, -32, 0, -32, -64, 0, 48, 0, 14100, 5660},
        //Exciter 3
        {64, 64, 0, -64, 0, 64, 0, -64, 0, 64, 0, 20000, 20}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(22,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
    cleanup ();
};


void
Exciter::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        sethar(0,value);
        break;
    case 2:
        sethar(1,value);
        break;
    case 3:
        sethar(2,value);
        break;
    case 4:
        sethar(3,value);
        break;
    case 5:
        sethar(4,value);
        break;
    case 6:
        sethar(5,value);
        break;
    case 7:
        sethar(6,value);
        break;
    case 8:
        sethar(7,value);
        break;
    case 9:
        sethar(8,value);
        break;
    case 10:
        sethar(9,value);
        break;
    case 11:
        setlpf(value);
        break;
    case 12:
        sethpf(value);
        break;

    };
};

int
Exciter::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Prm[0]);
        break;
    case 2:
        return (Prm[1]);
        break;
    case 3:
        return (Prm[2]);
        break;
    case 4:
        return (Prm[3]);
        break;
    case 5:
        return (Prm[4]);
        break;
    case 6:
        return (Prm[5]);
        break;
    case 7:
        return (Prm[6]);
        break;
    case 8:
        return (Prm[7]);
        break;
    case 9:
        return (Prm[8]);
        break;
    case 10:
        return (Prm[9]);
        break;
    case 11:
        return (lpffreq);
        break;
    case 12:
        return (hpffreq);
        break;
    };
    return (0);			//in case of bogus parameter number
};

Expander::Expander (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{

    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    lpfl = new AnalogFilter (2, 22000, 1, 0);
    lpfr = new AnalogFilter (2, 22000, 1, 0);
    hpfl = new AnalogFilter (3, 20, 1, 0);
    hpfr = new AnalogFilter (3, 20, 1, 0);

    env = 0.0;
    oldgain = 0.0;
    efollower = 0;
    fs = fSAMPLE_RATE;

    Expander_Change_Preset(0);


}

Expander::~Expander ()
{
}



void
Expander::cleanup ()
{
    lpfl->cleanup ();
    hpfl->cleanup ();
    lpfr->cleanup ();
    hpfr->cleanup ();
    oldgain = 0.0f;

}




void
Expander::setlpf (int value)
{
    Plpf = value;
    DspFloatType fr = (DspFloatType)Plpf;
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Expander::sethpf (int value)
{
    Phpf = value;
    DspFloatType fr = (DspFloatType)Phpf;
    hpfl->setfreq (fr);
    hpfr->setfreq (fr);
};


void
Expander::Expander_Change (int np, int value)
{

    switch (np) {

    case 1:
        Pthreshold = value;
        tfactor = dB2rap (-((DspFloatType) Pthreshold));
        tlevel = 1.0f/tfactor;
        break;
    case 2:
        Pshape = value;
        sfactor = dB2rap ((DspFloatType)Pshape/2);
        sgain = expf(-sfactor);
        break;
    case 3:
        Pattack = value;
        a_rate = 1000.0f/((DspFloatType)Pattack * fs);
        break;
    case 4:
        Pdecay = value;
        d_rate = 1000.0f/((DspFloatType)Pdecay * fs);
        break;
    case 5:
        setlpf(value);
        break;
    case 6:
        sethpf(value);
        break;
    case 7:
        Plevel = value;
        level = dB2rap((DspFloatType) value/6.0f);
        break;

    }


}

int
Expander::getpar (int np)
{

    switch (np)

    {
    case 1:
        return (Pthreshold);
        break;
    case 2:
        return (Pshape);
        break;
    case 3:
        return (Pattack);
        break;
    case 4:
        return (Pdecay);
        break;
    case 5:
        return (Plpf);
        break;
    case 6:
        return (Phpf);
        break;
    case 7:
        return (Plevel);
        break;
    }

    return (0);

}


void
Expander::Expander_Change_Preset (int npreset)
{

    const int PRESET_SIZE = 7;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {

        //Noise Gate
        {-50, 20, 50, 50, 3134, 76, 0},
        //Boost Gate
        {-55, 30, 50, 50, 1441, 157, 50},
        //Treble swell
        {-30, 9, 950, 25, 6703, 526, 90}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(25,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            Expander_Change (n+1, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            Expander_Change (n + 1, presets[npreset][n]);
    }

}



void
Expander::out (DspFloatType *efxoutl, DspFloatType *efxoutr)
{


    int i;
    DspFloatType delta = 0.0f;
    DspFloatType expenv = 0.0f;


    lpfl->filterout (efxoutl);
    hpfl->filterout (efxoutl);
    lpfr->filterout (efxoutr);
    hpfr->filterout (efxoutr);


    for (i = 0; i < PERIOD; i++) {

        delta = 0.5f*(fabsf (efxoutl[i]) + fabsf (efxoutr[i])) - env;    //envelope follower from Compressor.C
        if (delta > 0.0)
            env += a_rate * delta;
        else
            env += d_rate * delta;

        //End envelope power detection

        if (env > tlevel) env = tlevel;
        expenv = sgain * (expf(env*sfactor*tfactor) - 1.0f);		//Envelope waveshaping

        gain = (1.0f - d_rate) * oldgain + d_rate * expenv;
        oldgain = gain;				//smooth it out a little bit

        if(efollower) {
            efxoutl[i] = gain;
            efxoutr[i] += gain;
        } else {
            efxoutl[i] *= gain*level;
            efxoutr[i] *= gain*level;
        }

    }



};

Gate::Gate (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{

    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    lpfl = new AnalogFilter (2, 22000, 1, 0);
    lpfr = new AnalogFilter (2, 22000, 1, 0);
    hpfl = new AnalogFilter (3, 20, 1, 0);
    hpfr = new AnalogFilter (3, 20, 1, 0);

    env = 0.0;
    gate = 0.0;
    fs = fSAMPLE_RATE;
    state = CLOSED;
    hold_count = 0;

}

Gate::~Gate ()
{
}



void
Gate::cleanup ()
{
    lpfl->cleanup ();
    hpfl->cleanup ();
    lpfr->cleanup ();
    hpfr->cleanup ();
}




void
Gate::setlpf (int value)
{
    Plpf = value;
    DspFloatType fr = (DspFloatType)Plpf;
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Gate::sethpf (int value)
{
    Phpf = value;
    DspFloatType fr = (DspFloatType)Phpf;
    hpfl->setfreq (fr);
    hpfr->setfreq (fr);
};


void
Gate::Gate_Change (int np, int value)
{

    switch (np) {

    case 1:
        Pthreshold = value;
        t_level = dB2rap ((DspFloatType)Pthreshold);
        break;
    case 2:
        Prange = value;
        cut = dB2rap ((DspFloatType)Prange);
        break;
    case 3:
        Pattack = value;
        a_rate = 1000.0f / ((DspFloatType)Pattack * fs);
        break;
    case 4:
        Pdecay = value;
        d_rate = 1000.0f / ((DspFloatType)Pdecay * fs);
        break;
    case 5:
        setlpf(value);
        break;
    case 6:
        sethpf(value);
        break;
    case 7:
        Phold = value;
        hold = (DspFloatType)Phold;
        break;

    }


}

int
Gate::getpar (int np)
{

    switch (np)

    {
    case 1:
        return (Pthreshold);
        break;
    case 2:
        return (Prange);
        break;
    case 3:
        return (Pattack);
        break;
    case 4:
        return (Pdecay);
        break;
    case 5:
        return (Plpf);
        break;
    case 6:
        return (Phpf);
        break;
    case 7:
        return (Phold);
        break;

    }

    return (0);

}


void
Gate::Gate_Change_Preset (int npreset)
{

    const int PRESET_SIZE = 7;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //0
        {0, 0, 1, 2, 6703, 76, 2},
        //-10
        {0, -10, 1, 2, 6703, 76, 2},
        //-20
        {0, -20, 1, 2, 6703, 76, 2}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(16,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            Gate_Change(n + 1, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            Gate_Change (n + 1, presets[npreset][n]);
    }

}



void
Gate::out (DspFloatType *efxoutl, DspFloatType *efxoutr)
{


    int i;
    DspFloatType sum;


    lpfl->filterout (efxoutl);
    hpfl->filterout (efxoutl);
    lpfr->filterout (efxoutr);
    hpfr->filterout (efxoutr);


    for (i = 0; i < PERIOD; i++) {

        sum = fabsf (efxoutl[i]) + fabsf (efxoutr[i]);


        if (sum > env)
            env = sum;
        else
            env = sum * ENV_TR + env * (1.0f - ENV_TR);

        if (state == CLOSED) {
            if (env >= t_level)
                state = OPENING;
        } else if (state == OPENING) {
            gate += a_rate;
            if (gate >= 1.0) {
                gate = 1.0f;
                state = OPEN;
                hold_count = lrintf (hold * fs * 0.001f);
            }
        } else if (state == OPEN) {
            if (hold_count <= 0) {
                if (env < t_level) {
                    state = CLOSING;
                }
            } else
                hold_count--;

        } else if (state == CLOSING) {
            gate -= d_rate;
            if (env >= t_level)
                state = OPENING;
            else if (gate <= 0.0) {
                gate = 0.0;
                state = CLOSED;
            }
        }

        efxoutl[i] *= (cut * (1.0f - gate) + gate);
        efxoutr[i] *= (cut * (1.0f - gate) + gate);

    }



};


PitchShifter::PitchShifter (long fftFrameSize, long osamp, DspFloatType sampleRate)
{

    /* set up some handy variables */
    fftFrameSize2 = fftFrameSize / 2;
    FS_osamp = fftFrameSize2 * osamp;
    stepSize = fftFrameSize / osamp;
    dfftFrameSize = (DspFloatType) fftFrameSize;
    coef_dfftFrameSize = 1.0 / dfftFrameSize;
    coef_dpi = 1.0 / D_PI;
    coef_mpi = 1.0 / M_PI;
    dpi_coef = coef_dfftFrameSize * D_PI;
    freqPerBin = sampleRate / (DspFloatType) fftFrameSize;
    coefPB = 1.0 /freqPerBin;
    expct = D_PI * (DspFloatType) stepSize / (DspFloatType) fftFrameSize;
    inFifoLatency = fftFrameSize - stepSize;
    gRover = inFifoLatency;
    ratio = 1.0;

    /* initialize our static arrays */
    memset (gInFIFO, 0.0, MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (gOutFIFO, 0.0, MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (gFFTworksp, 0.0, 2 * MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (gLastPhase, 0.0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof (DspFloatType));
    memset (gSumPhase, 0.0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof (DspFloatType));
    memset (gOutputAccum, 0.0, 2 * MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (gAnaFreq, 0.0, MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (gAnaMagn, 0.0, MAX_FRAME_LENGTH * sizeof (DspFloatType));
    memset (window, 0.0, MAX_FRAME_LENGTH * sizeof (DspFloatType));

    //create FFTW plan
    int nfftFrameSize = (int) fftFrameSize;
    //printf("nfs= %d, lfs= %ld\n", nfftFrameSize, fftFrameSize);

    ftPlanForward = fftw_plan_dft_1d(nfftFrameSize, fftw_in, fftw_out, FFTW_FORWARD, FFTW_MEASURE);
    ftPlanInverse = fftw_plan_dft_1d(nfftFrameSize, fftw_in, fftw_out, FFTW_BACKWARD, FFTW_MEASURE);

    //Pre-compute window function
    makeWindow(fftFrameSize);
}

PitchShifter::~PitchShifter ()
{

    fftw_destroy_plan(ftPlanForward);
    fftw_destroy_plan(ftPlanInverse);
}

void
PitchShifter::makeWindow(long fftFrameSize)
{
    for (k = 0; k < fftFrameSize; k++) {
        DspFloatType dk = (DspFloatType) k;
        window[k] = 0.5 -0.5 * cos (dpi_coef * dk);
    }
}

void
PitchShifter::smbPitchShift (DspFloatType pitchShift, long numSampsToProcess,
                             long fftFrameSize, long osamp, DspFloatType sampleRate,
                             DspFloatType *indata, DspFloatType *outdata)
{
    long i;
    DspFloatType maxmag = 0.0f;
    DspFloatType tunefrq = 0.0f;

    /* main processing loop */
    for (i = 0; i < numSampsToProcess; i++) {

        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover - inFifoLatency];
        gRover++;

        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;

            /* do windowing and re,im interleave */
            for (k = 0; k < fftFrameSize; k++) {
//	      DspFloatType dk = (DspFloatType) k;
//	      window =
//		-.5 * cos (dpi_coef * dk) +
//		.5;

                fftw_in[k][0] = gInFIFO[k] * window[k];
                fftw_in[k][1] = 0.0;
                //  gFFTworksp[2 * k] = gInFIFO[k] * window;
                // gFFTworksp[2 * k + 1] = 0.;
            }
            /* ***************** ANALYSIS ******************* */
            /* do transform */
            // smbFft (gFFTworksp, fftFrameSize, -1);

            fftw_execute(ftPlanForward);

            /* this is the analysis step */
            for (k = 0; k <= fftFrameSize2; k++) {
                DspFloatType dk = (DspFloatType) k;
                /* de-interlace FFT buffer */
                //real = gFFTworksp[2 * k];
                //imag = gFFTworksp[2 * k + 1];
                real = fftw_out[k][0];
                imag = fftw_out[k][1];
                /* compute magnitude and phase */
                magn = 2. * sqrt (real * real + imag * imag);
                phase = atan2 (imag, real);

                /* compute phase difference */
                tmp = phase - gLastPhase[k];
                gLastPhase[k] = phase;

                /* subtract expected phase difference */
                tmp -= dk *expct;

                /* map delta phase into +/- Pi interval */
                qpd = lrint(tmp * coef_mpi);
                if (qpd >= 0)
                    qpd += qpd & 1;

                else
                    qpd -= qpd & 1;
                tmp -= M_PI * (DspFloatType) qpd;

                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = osamp * tmp * coef_dpi;

                /* compute the k-th partials' true frequency */
                tmp = dk *freqPerBin + tmp * freqPerBin;

                /* store magnitude and true frequency in analysis arrays */
                gAnaMagn[k] = magn;
                gAnaFreq[k] = tmp;
                if(magn>maxmag) {
                    maxmag = magn;
                    tunefrq = tmp;
                }
            }

            /* ***************** PROCESSING ******************* */
            /* this does the actual pitch shifting */
            memset (gSynMagn, 0, fftFrameSize * sizeof (DspFloatType));
            memset (gSynFreq, 0, fftFrameSize * sizeof (DspFloatType));
            for (k = 0; k <= fftFrameSize2; k++) {
                index = long (k * pitchShift);
                if (index <= fftFrameSize2) {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index] = gAnaFreq[k] * pitchShift;
                }
            }

            /* ***************** SYNTHESIS ******************* */
            /* this is the synthesis step */
            for (k = 0; k <= fftFrameSize2; k++) {
                DspFloatType dk = (DspFloatType) k;
                /* get magnitude and true frequency from synthesis arrays */
                magn = gSynMagn[k];
                tmp = gSynFreq[k];

                /* subtract bin mid frequency */
                tmp -= dk *freqPerBin;

                /* get bin deviation from freq deviation */
                tmp *= coefPB;

                /* take osamp into account */
                tmp = D_PI * tmp / osamp;

                /* add the overlap phase advance back in */
                tmp += dk *expct;

                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += tmp;
                phase = gSumPhase[k];

                /* get real and imag part and re-interleave */
                //gFFTworksp[2 * k] = magn * cos (phase);
                // gFFTworksp[2 * k + 1] = magn * sin (phase);
                fftw_in[k][0] = magn * cos (phase);
                fftw_in[k][1] = magn * sin (phase);

            }
            /* zero negative frequencies */
//	  for (k = fftFrameSize + 2; k < 2 * fftFrameSize; k++)
//	    gFFTworksp[k] = 0.;
            for (k = 2 + fftFrameSize2; k < fftFrameSize; k++) {
                fftw_in[k][0] = 0.;
                fftw_in[k-1][1] = 0.;
            }
            /* do inverse transform */
            //  smbFft (gFFTworksp, fftFrameSize, 1);

            fftw_execute(ftPlanInverse);

            /* do windowing and add to output accumulator */
            for (k = 0; k < fftFrameSize; k++) {
                //window =
                //	-.5 * cos (dpi_coef * (DspFloatType) k) +
                //	.5;
                gOutputAccum[k] += 2. * window[k] * fftw_out[k][0] / FS_osamp;
                //gOutputAccum[k] +=
//		2. * window * gFFTworksp[2 * k] / FS_osamp;
            }
            for (k = 0; k < stepSize; k++)
                gOutFIFO[k] = gOutputAccum[k];

            /* shift accumulator */
            memmove (gOutputAccum, gOutputAccum + stepSize,
                     fftFrameSize * sizeof (DspFloatType));

            /* move input FIFO */
            for (k = 0; k < inFifoLatency; k++)
                gInFIFO[k] = gInFIFO[k + stepSize];
        }
    }

    //printf("\rFreq: %f", tunefrq);
}


// -----------------------------------------------------------------------------------------------------------------
void
PitchShifter::smbFft (DspFloatType *fftBuffer, long fftFrameSize, long sign)
/*
	FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].
*/
{
    DspFloatType wr, wi, arg, *p1, *p2, temp;
    DspFloatType tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long i, bitm, j, le, le2, k;
    for (i = 2; i < 2 * fftFrameSize - 2; i += 2) {
        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1) {
            if (i & bitm)
                j++;
            j <<= 1;
        }
        if (i < j) {
            p1 = fftBuffer + i;
            p2 = fftBuffer + j;
            temp = *p1;
            *(p1++) = *p2;
            *(p2++) = temp;
            temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }
    for (k = 0, le = 2; k < (long) (log (fftFrameSize) / LOG_2 + .5); k++) {
        le <<= 1;
        le2 = le >> 1;
        ur = 1.0;
        ui = 0.0;
        arg = M_PI / (le2 >> 1);
        wr = cosf (arg);
        wi = sign * sinf (arg);
        for (j = 0; j < le2; j += 2) {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;
            for (i = j; i < 2 * fftFrameSize; i += le) {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}


// -----------------------------------------------------------------------------------------------------------------

/*
    12/12/02, smb
    PLEASE NOTE:
    There have been some reports on domain errors when the atan2() function was used
    as in the above code. Usually, a domain error should not interrupt the program flow
    (maybe except in Debug mode) but rather be handled "silently" and a global variable
    should be set according to this error. However, on some occasions people ran into
    this kind of scenario, so a replacement atan2() function is provided here.
    If you are experiencing domain errors and your program stops, simply replace all
    instances of atan2() with calls to the smbAtan2() function below.
*/
DspFloatType
PitchShifter::smbAtan2 (DspFloatType x, DspFloatType y)
{
    DspFloatType signx;
    if (x > 0.)
        signx = 1.;

    else
        signx = -1.;
    if (x == 0.)
        return 0.;
    if (y == 0.)
        return signx * M_PI_2;
    return atan2 (x, y);
}



Harmonizer::Harmonizer (DspFloatType *efxoutl_, DspFloatType *efxoutr_, long int Quality, int DS, int uq, int dq)
{



    efxoutl = efxoutl_;
    efxoutr = efxoutr_;
    hq = Quality;
    adjust(DS);

    templ = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    tempr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);


    outi = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);
    outo = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);

    memset (outi, 0, sizeof (DspFloatType) * nPERIOD);
    memset (outo, 0, sizeof (DspFloatType) * nPERIOD);

    U_Resample = new Resample(dq);
    D_Resample = new Resample(uq);


    pl = new AnalogFilter (6, 22000, 1, 0);

    PS = new PitchShifter (window, hq, nfSAMPLE_RATE);
    PS->ratio = 1.0f;

    Ppreset = 0;
    PMIDI = 0;
    mira = 0;
    setpreset (Ppreset);


    cleanup ();

};



Harmonizer::~Harmonizer ()
{
};

void
Harmonizer::cleanup ()
{
    mira = 0;
    memset(outi, 0, sizeof(DspFloatType)*nPERIOD);
    memset(outo, 0, sizeof(DspFloatType)*nPERIOD);
};


void
Harmonizer::applyfilters (DspFloatType * efxoutl)
{
    pl->filterout (efxoutl);
};



void
Harmonizer::out (DspFloatType *smpsl, DspFloatType *smpsr)
{

    int i;

    if((DS_state != 0) && (Pinterval !=12)) {
        memcpy(templ, smpsl,sizeof(DspFloatType)*PERIOD);
        memcpy(tempr, smpsr,sizeof(DspFloatType)*PERIOD);
        U_Resample->out(templ,tempr,smpsl,smpsr,PERIOD,u_up);
    }


    for (i = 0; i < nPERIOD; i++) {
        outi[i] = (smpsl[i] + smpsr[i])*.5;
        if (outi[i] > 1.0)
            outi[i] = 1.0f;
        if (outi[i] < -1.0)
            outi[i] = -1.0f;

    }

    if ((PMIDI) || (PSELECT))
        PS->ratio = r__ratio[0];

    if (Pinterval != 12) {
        PS->smbPitchShift (PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

        if((DS_state != 0) && (Pinterval != 12)) {
            D_Resample->mono_out(outo,templ,nPERIOD,u_down,PERIOD);
        } else {
            memcpy(templ, outo,sizeof(DspFloatType)*PERIOD);
        }



        applyfilters (templ);

        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] = templ[i] * gain * panning;
            efxoutr[i] = templ[i] * gain * (1.0f - panning);
        }

    }

};



void
Harmonizer::setvolume (int value)
{
    this->Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};



void
Harmonizer::setpanning (int value)
{
    this->Ppan = value;
    panning = (DspFloatType)Ppan / 127.0f;
};



void
Harmonizer::setgain (int value)
{
    this->Pgain = value;
    gain = (DspFloatType)Pgain / 127.0f;
    gain *=2.0;
};


void
Harmonizer::setinterval (int value)
{

    this->Pinterval = value;
    interval = (DspFloatType)Pinterval - 12.0f;
    PS->ratio = powf(2.0f, interval / 12.0f);
    if (Pinterval % 12 == 0)
        mira = 0;
    else
        mira = 1;

};

void
Harmonizer::fsetfreq (int value)
{

    fPfreq = value;
    DspFloatType tmp = (DspFloatType)value;
    pl->setfreq (tmp);
}

void
Harmonizer::fsetgain (int value)
{

    DspFloatType tmp;

    this->fPgain = value;
    tmp = 30.0f * ((DspFloatType)value - 64.0f) / 64.0f;
    pl->setgain (tmp);

}

void
Harmonizer::fsetq (int value)
{

    DspFloatType tmp;
    this->fPq = value;
    tmp = powf(30.0f, ((DspFloatType)value - 64.0f) / 64.0f);
    pl->setq (tmp);

}

void
Harmonizer::setMIDI (int value)
{

    this->PMIDI = value;
}


void
Harmonizer::adjust(int DS)
{

    DS_state=DS;


    switch(DS) {

    case 0:
        nPERIOD = PERIOD;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        window = 2048;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD*96000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        window = 2048;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD*48000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        window = 2048;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD*44100.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        window = 2048;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD*32000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        window = 2048;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD*22050.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        window = 1024;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD*16000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        window = 1024;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD*12000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        window = 512;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD*8000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        window = 512;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD*4000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        window = 256;
        break;
    }
    u_up= (DspFloatType)nPERIOD / (DspFloatType)PERIOD;
    u_down= (DspFloatType)PERIOD / (DspFloatType)nPERIOD;
}





void
Harmonizer::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0},
        //Octavador
        {64, 64, 64, 0, 6000, 0, 0, 0, 64, 64, 0},
        //3mdown
        {64, 64, 64, 9, 6000, 0, 0, 0, 64, 64, 0}
    };


    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(14,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;


};



void
Harmonizer::changepar (int npar, int value)
{

    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setgain (value);
        break;
    case 3:
        setinterval (value);
        break;
    case 4:
        fsetfreq (value);
        break;
    case 5:
        PSELECT = value;;
        break;
    case 6:
        Pnote = value;
        break;
    case 7:
        Ptype = value;
        break;
    case 8:
        fsetgain (value);
        break;
    case 9:
        fsetq (value);
        break;
    case 10:
        setMIDI (value);
        break;



    }


};


int
Harmonizer::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppan);
        break;
    case 2:
        return (Pgain);
        break;
    case 3:
        return (Pinterval);
        break;
    case 4:
        return (fPfreq);
        break;
    case 5:
        return (PSELECT);
        break;
    case 6:
        return (Pnote);
        break;
    case 7:
        return (Ptype);
        break;
    case 8:
        return (fPgain);
        break;
    case 9:
        return (fPq);
        break;
    case 10:
        return (PMIDI);
        break;
    default:
        return (0);

    }

};



Infinity::Infinity (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    int i;
    for (i = 0; i<NUM_INF_BANDS; i++) {
        filterl[i] = new RBFilter (0, 80.0f, 70.0f, 1.0f);
        filterr[i] = new RBFilter (0, 80.0f, 70.0f, 1.0f);
        rbandstate[i].level = 1.0f;
        rbandstate[i].vol = 1.0f;
        lphaser[i].gain = 0.5f;
        rphaser[i].gain = 0.5f;
        for (int j = 0; j<MAX_PHASER_STAGES; j++) {
            lphaser[i].yn1[j] = 0.0f;
            rphaser[i].yn1[j] = 0.0f;
            lphaser[i].xn1[j] = 0.0f;
            rphaser[i].xn1[j] = 0.0f;
        }

        Pb[i] = 1;
    }
    Ppreset = 2;
    setpreset (Ppreset);
    Pvolume = 64;
    outvolume = 0.5;
    Pq = 30;
    Pstartfreq = 5;
    Pendfreq = 80;
    Prate = 2;
    Psubdiv = 32;
    Pstdf = 0;
    volmaster = 0.25;
    Preverse = 0;
    Pautopan = 0;
    autopan = 0.0f;
    Pstages = 0;
    phaserfb = 0.0f;
    dsin = 0.0f;
    tflag = 0;
    ratescale = 1.0f;

    DspFloatType dt = 1.0f/fSAMPLE_RATE;
    alpha = dt/(0.5f + dt);          //200ms time constant on parameter change -- quick but not jerky
    beta = 1.0f - alpha;

    adjustfreqs();
    reinitfilter();

};

Infinity::~Infinity ()
{
};

DspFloatType inline
Infinity::phaser(phasevars *pstruct, DspFloatType fxn, int j)
{
    int k;
    DspFloatType xn = fxn + DENORMAL_GUARD;

    for (k = 0; k < Pstages; k++) {
        pstruct[j].yn1[k] = pstruct[j].xn1[k] - pstruct[j].gain * (xn + pstruct[j].yn1[k]);
        //pstruct[j].yn1[k] += DENORMAL_GUARD;
        pstruct[j].xn1[k] = fxn;
        xn = pstruct[j].yn1[k];
    };
    pstruct[j].yn1[0] -= phaserfb*fxn;

    return fxn;

}

void inline
Infinity::oscillator()
{
    DspFloatType rmodulate, lmodulate, ratemod;

    //master oscillator
    msin += mconst*mcos;
    mcos -= msin*mconst;

//introduce doppler effect
    if(Pstages<9) {
//smooth parameters for tempo change
        rampconst = alpha*rampconst + beta*crampconst;
        irampconst = 1.0f/rampconst;
        fconst = alpha*fconst + beta*cfconst;
    } else {
        dsin = autopan*ratescale*msin;
        ratemod = (1.0f + dsin/fSAMPLE_RATE);
//smooth parameters for tempo change
        rampconst = alpha*rampconst + beta*crampconst*ratemod;
        irampconst = 1.0f/rampconst;
        fconst = alpha*fconst + beta*cfconst*ratemod;
    }


    for (int i=0; i<NUM_INF_BANDS; i++)  {
//right
        rbandstate[i].sinp += fconst*rbandstate[i].cosp;
        rbandstate[i].cosp -= rbandstate[i].sinp*fconst;
        rbandstate[i].lfo = (1.0f + rbandstate[i].sinp);  //lfo modulates filter band volume
        rbandstate[i].ramp *= rampconst;  //ramp modulates filter band frequency cutoff
        if (rbandstate[i].ramp > maxlevel)  {
            rbandstate[i].ramp = minlevel;
            rbandstate[i].sinp = -1.0f;
            rbandstate[i].cosp = 0.0f;  //phase drift catch-all
//printf("i: %d sin: %f lfo: %f ramp: %f\n",i,rbandstate[i].sinp, rbandstate[i].lfo, rbandstate[i].ramp);
        }
        if (rbandstate[i].ramp < minlevel) {
            rbandstate[i].ramp = maxlevel;  //if it is going in reverse (rampconst < 0)
            rbandstate[i].sinp = -1.0f;
            rbandstate[i].cosp = 0.0f;   //phase drift catch-all
        }
        rbandstate[i].vol = rbandstate[i].level*rbandstate[i].lfo;

//left
        lbandstate[i].sinp += fconst*lbandstate[i].cosp;
        lbandstate[i].cosp -= lbandstate[i].sinp*fconst;
        lbandstate[i].lfo = (1.0f + lbandstate[i].sinp);  //lfo modulates filter band volume
        if (Preverse) lbandstate[i].ramp *= irampconst;  //left reversed from right
        else lbandstate[i].ramp *= rampconst;   //normal mode
        if (lbandstate[i].ramp > maxlevel)  {
            lbandstate[i].ramp = minlevel;
            lbandstate[i].sinp = -1.0f;
            lbandstate[i].cosp = 0.0f;  //phase drift catch-all
//printf("i: %d sin: %f lfo: %f ramp: %f\n",i,lbandstate[i].sinp, lbandstate[i].lfo, lbandstate[i].ramp);
        }
        if (lbandstate[i].ramp < minlevel) {
            lbandstate[i].ramp = maxlevel;  //if it is going in reverse (rampconst < 0)
            lbandstate[i].sinp = -1.0f;
            lbandstate[i].cosp = 0.0f;  //phase drift catch-all
        }
        lbandstate[i].vol = lbandstate[i].level*lbandstate[i].lfo;

        //lmodulate = linconst*f_pow2(logconst*lbandstate[i].ramp);
        //rmodulate = linconst*f_pow2(logconst*rbandstate[i].ramp);
        lmodulate = 1.0f - 0.25f*lbandstate[i].ramp;
        rmodulate = 1.0f - 0.25f*rbandstate[i].ramp;

        filterl[i]->directmod(lbandstate[i].ramp);
        filterr[i]->directmod(rbandstate[i].ramp);

        lphaser[i].gain = lmodulate;
        rphaser[i].gain = rmodulate;
    }

}

/*
 * Apply the effect
 */
void
Infinity::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j;
    DspFloatType tmpr, tmpl;

    for (i = 0; i<PERIOD; i++)  {
        //modulate
        oscillator();
        tmpr = tmpl = 0.0f;
        //run filter



        if(Pstages) {
            for (j=0; j<NUM_INF_BANDS; j++)  {
                tmpl+=phaser(lphaser, filterl[j]->filterout_s(lbandstate[j].vol*smpsl[i]), j );
                tmpr+=phaser(rphaser, filterr[j]->filterout_s(rbandstate[j].vol*smpsr[i]), j );
            }
        } else {
            for (j=0; j<NUM_INF_BANDS; j++)  {
                tmpl+=filterl[j]->filterout_s(lbandstate[j].vol*smpsl[i]);
                tmpr+=filterr[j]->filterout_s(rbandstate[j].vol*smpsr[i]);
            }
        }


        efxoutl[i] = (1.0f + autopan*mcos)*volmaster*tmpl;
        efxoutr[i] = (1.0f - autopan*mcos)*volmaster*tmpr;



    }

};

/*
 * Cleanup the effect
 */
void
Infinity::cleanup ()
{
    reinitfilter ();
    for ( int i = 0; i<NUM_INF_BANDS; i++) {
        filterl[i]->cleanup();
        filterr[i]->cleanup();
        lphaser[i].gain = 0.5f;
        rphaser[i].gain = 0.5f;
        for (int j = 0; j<MAX_PHASER_STAGES; j++) {
            lphaser[i].yn1[j] = 0.0f;
            rphaser[i].yn1[j] = 0.0f;
            lphaser[i].xn1[j] = 0.0f;
            rphaser[i].xn1[j] = 0.0f;
        }
    }


};


/*
 * Parameter control
 */

void
Infinity::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
Infinity::setq ()
{
    DspFloatType fq = (DspFloatType) Pq;
    if(Pq<0) {
        qq = powf(2.0f, fq/500.0f);  //q ranges down to 0.5
        volmaster = 1.0f;
    } else {
        qq = powf(2.0f, fq/125.0f);  //q can go up to 256
        volmaster = (1.0f-fq/1500.0f)/sqrt(qq);
    }

    for (int i=0; i<NUM_INF_BANDS; i++)  {
        filterl[i]->setq(qq);
        filterr[i]->setq(qq);
    }
}
void
Infinity::reinitfilter ()
{
    DspFloatType fbandnum = (DspFloatType) (NUM_INF_BANDS);
    DspFloatType halfpi = -M_PI/2.0f;  //offset so rbandstate[0].sinp = -1.0 when rbandstate[0].ramp = 0;
    DspFloatType stateconst = 0;
    DspFloatType idx = 0;

    for (int i=0; i<NUM_INF_BANDS; i++)  {  //get them started on their respective phases
//right
        idx = (DspFloatType) i;
        rbandstate[i].sinp = sinf(halfpi + D_PI*idx/fbandnum);
        rbandstate[i].cosp = cosf(halfpi + D_PI*idx/fbandnum);
        rbandstate[i].ramp = linconst*powf(2.0f,logconst*idx/fbandnum);
        rbandstate[i].lfo = 0.5f*(1.0f + rbandstate[i].sinp);  //lfo modulates filter band volume
//left
        stateconst = fmod((stdiff + idx), fbandnum);
        lbandstate[i].sinp = sinf(halfpi + D_PI*stateconst/fbandnum);
        lbandstate[i].cosp = cosf(halfpi + D_PI*stateconst/fbandnum);
        lbandstate[i].ramp = linconst*powf(2.0f,logconst*stateconst/fbandnum);
        lbandstate[i].lfo = 0.5f*(1.0f + rbandstate[i].sinp);  //lfo modulates filter band volume
//printf("i: %d sin: %f lfo: %f ramp: %f max: %f min: %f\n",i,rbandstate[i].sinp, rbandstate[i].lfo, rbandstate[i].ramp, maxlevel, minlevel);

        filterl[i]->setmix(0, 80.0f, 70.0f, 1.0f);
        filterr[i]->setmix(0, 80.0f, 70.0f, 1.0f);
        filterl[i]->setmode(1);
        filterr[i]->setmode(1);
        filterl[i]->settype(2);
        filterr[i]->settype(2);  //bandpass
        filterl[i]->setq(qq);
        filterr[i]->setq(qq);
        filterl[i]->directmod(lbandstate[i].ramp);
        filterr[i]->directmod(rbandstate[i].ramp);

    }
    msin = 0.0f;
    mcos = 1.0f;
};

void
Infinity::adjustfreqs()
{

    DspFloatType frate;
    DspFloatType fs = fSAMPLE_RATE;

    fstart = 20.0f + 6000.0f*((DspFloatType) Pstartfreq/127.0f);
    fend =  20.0f + 6000.0f*((DspFloatType) Pendfreq/127.0f);
    if(Psubdiv>0) frate = ((DspFloatType) ( 1+ Prate ))/(((DspFloatType) Psubdiv)*60.0f);    //beats/second
    else frate = ((DspFloatType) (1 - Psubdiv))*((DspFloatType) Prate)/60.0f;

    if (fstart < fend) {
        frmin = fstart;
        frmax = fend;
        crampconst = 1.0f + frate*logf(frmax/frmin)/fs;
    } else {
        frmax = fstart;
        frmin = fend;
        crampconst = 1.0f/(1.0f + frate*logf(frmax/frmin)/fs);
    }

    cirampconst = 1.0f/crampconst;
    logconst = logf(frmax/frmin)/logf(2.0f);
    linconst = D_PI*frmin/fs;  //these lines are for using powf() in the initialization

    minlevel = D_PI*frmin/fs;
    maxlevel = minlevel*frmax/frmin;
    //printf("min %f max %f rampconst %f irampconst %f\n", minlevel, maxlevel,rampconst, irampconst);
    //fconst =  2.0f * sinf(PI*frate / fs);  //this is a more precise interpretation
    cfconst =  D_PI*frate / fs;  //this seems to work well at low frequencies
    mconst = D_PI*((DspFloatType) Prate)/(fs*60.0f*4.0f);

}

void
Infinity::setpreset (int npreset)
{
    const int PRESET_SIZE = 18;
    const int NUM_PRESETS = 10;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Basic
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 700, 20, 80, 60, 0, 1, 0, 0, 1 },
        //Rising Comb
        {64, 64, -64, 64, -64, 64, -64, 64, -64, 600, 0, 127, 32, 0, 16, 0, 0, 1 },
        //Falling Comb
        {64, 64, -64, 64, -64, 64, -64, 64, -64, 600, 127, 0, 32, 0, 16, 0, 0, 1 },
        //Laser
        {0, 64, -64, 64, -64, 64, -64, 64, -64, 600, 127, 2, 70, 0, 1, 0, 0, 1 },
        //Doppler
        {0,64,-64,64,-64,64,-64,64,-64,-564,0,127,150,-31,2,99,0,10},
        //Freq Shifter
        {0,64,-64,64,-64,64,-64,64,-64,-564,0,127,60,-48,-1,0,0,10},
        //Dizzy Sailor
        {0,64,-64,64,-64,64,-64,64,-64,-564,0,127,60,-48,-1,110,0,10},
        //Stereo Phaser
        {42,64,0,64,0,64,0,64,0,91,0,127,60,32,16,92,1,6},
        //Corkscrew
        {64,64,0,-64,0,64,0,-64,0,120,0,127,120,-16,15,67,1,4},
        //FreqeeVox
        {0,64,-64,64,-64,64,-64,64,-64,-164,0,127,556,-16,-3,0,0,8}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(46,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;

    reinitfilter ();
};


void
Infinity::changepar (int npar, int value)
{

    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        Pb[npar - 1] = value;
        rbandstate[npar - 1].level = (DspFloatType) value/64.0f;
        lbandstate[npar - 1].level = (DspFloatType) value/64.0f;
        break;
    case 9:
        Pq = value;
        setq();
        break;
    case 10:
        Pstartfreq = value;
        adjustfreqs();
        reinitfilter ();
        break;
    case 11:
        Pendfreq = value;
        adjustfreqs();
        reinitfilter ();
        break;
    case 12:
        Prate = value;
        adjustfreqs();
        break;
    case 13:
        Pstdf = value;
        stdiff = ((DspFloatType) value)/127.0f;
        reinitfilter ();
        break;
    case 14:
        Psubdiv = value;
        if(value!=0) ratescale =  10.0f/((DspFloatType) abs(value));
        else ratescale = 10.0f;
        adjustfreqs();
        break;
    case 15:
        Pautopan = value;
        autopan = ((DspFloatType) Pautopan)/127.0f;
        if (autopan > 1.0f) autopan = 1.0f;
        if (autopan < 0.0f) autopan = 0.0f;
        break;
    case 16:
        Preverse = value;
        adjustfreqs();
        reinitfilter ();
        break;
    case 17:
        Pstages = value - 1;
//        for (int i=0; i<NUM_INF_BANDS; i++)  {
//        filterl[i]->setstages(value - 1);
//        filterr[i]->setstages(value - 1);
//        }
        phaserfb = 0.5f + (((DspFloatType) (Pstages))/11.01f)*0.5f;
        break;
    };
};

int
Infinity::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return (Pb[npar - 1]);
        break;
    case 9:
        return (Pq);
        break;
    case 10:
        return (Pstartfreq);
        break;
    case 11:
        return (Pendfreq);
        break;
    case 12:
        return (Prate);
        break;
    case 13:
        return (Pstdf);
        break;
    case 14:
        return (Psubdiv);
        break;
    case 15:
        return (Pautopan);
        break;
    case 16:
        return (Preverse);
        break;
    case 17:
        return (Pstages + 1);
        break;
    default:
        return (0);
    };

};


metronome::metronome ()
{
    dulltick =  new AnalogFilter(4,1600.0f,80.0f,1);   //BPF
    sharptick =  new AnalogFilter(4,2800.0f,80.0f,1);  //BPF
    hpf =  new AnalogFilter(3,850.0f,60.0f,1);  //HPF
    tick_interval = SAMPLE_RATE;
    tickctr = 0;
    markctr = 0;
    ticktype = 4;
    meter = 3;
    tickper = lrintf(0.012f*fSAMPLE_RATE);

};

metronome::~metronome ()
{
};

void
metronome::cleanup()
{
    tickctr = 0;
    markctr = 0;
    dulltick->cleanup();
    sharptick->cleanup();
    hpf->cleanup();
}
/*
 * Update the changed parameters
 */
void
metronome::set_tempo (int bpm)
{

    DspFloatType tickperiod = 60.0f/((DspFloatType) bpm);
    tick_interval = lrintf(fSAMPLE_RATE * tickperiod);

};

void
metronome::set_meter (int counts)  //how many counts to hear the "mark"
{
    ticktype = counts; //always dull if 0, always sharp if 1, mark on interval if more
    if(counts<1) counts = 1;
    meter = counts - 1;

};

/*
 * Audible output
 */
void
metronome::metronomeout (DspFloatType * tickout)
{
    DspFloatType outsharp, outdull;
    DspFloatType ticker = 0.0f;
    DspFloatType hipass = 0.0f;
    int i;

    for(i=0; i<PERIOD; i++) {
        tickctr++;

        if (tickctr>tick_interval) {
            tickctr = 0;
            markctr++;
            if(markctr>meter) markctr = 0;
        }
        if (tickctr<tickper) ticker = 1.0f;
        else ticker = 0.0f;
        hipass  = hpf->filterout_s(ticker);
        if(hipass>0.5f) hipass = 0.5f;
        if(hipass<-0.5f) hipass = -0.5f;
        outdull = dulltick->filterout_s(hipass);
        outsharp = sharptick-> filterout_s(hipass);

        switch(ticktype) {
        case 0:
            tickout[i] = 1.25f*outdull;
            break;
        case 1:
            tickout[i] = 0.65f*outsharp;
            break;
        default:
            if(markctr==0) {
                tickout[i] = 0.65f*outsharp;
            } else {
                tickout[i] = 1.25f*outdull;
            }
            break;

        }

    }


};


Looper::Looper (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType size)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    //default values
    Pclear = 1;
    Pplay = 0;
    Precord = 0;
    Pstop = 1;
    PT1 = 1;
    PT2 = 0;
    Pautoplay = 0;
    rplaystate = 0;
    first_time1 = 1;
    first_time2 = 1;
    Pms = 0;
    Ptempo = 120;
    settempo(120);
    Pbar = 2;
    setbar(2);
    ticker.cleanup();

    Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
    maxx_delay = lrintf(fSAMPLE_RATE * size);
    fade = (int) SAMPLE_RATE / 2;    //1/2 SR fade time available

    ldelay = new DspFloatType[maxx_delay];
    rdelay = new DspFloatType[maxx_delay];
    t2ldelay = new DspFloatType[maxx_delay];
    t2rdelay = new DspFloatType[maxx_delay];

    setpreset (Ppreset);
    cleanup ();
};

Looper::~Looper ()
{
};

/*
 * Cleanup the effect
 */
void
Looper::cleanuppt1 ()
{
    int i;
    for (i = 0; i < maxx_delay; i++) {
        ldelay[i] = 0.0f;
        rdelay[i] = 0.0f;
    }

};
void
Looper::cleanuppt2 ()
{
    int i;
    for (i = 0; i < maxx_delay; i++) {
        t2ldelay[i] = 0.0f;
        t2rdelay[i] = 0.0f;
    }

};

void Looper::cleanup ()
{
    cleanuppt1 ();
    cleanuppt2 ();

};
/*
 * Initialize the delays
 */
void
Looper::initdelays ()
{

    if(PT1) {
        kl = 0;
        ticker.cleanup();
        dl = maxx_delay;
        rvkl = dl - 1;
        cleanuppt1 ();
    }

    if(PT2) {
        kl2 = 0;
        ticker.cleanup();
        dl2 = maxx_delay;
        rvkl2 = dl2 - 1;
        cleanuppt2 ();
    }
    Srate_Attack_Coeff = 90.0f / (dl + dl2);   // Set swell time

};

/*
 * Effect output
 */
void
Looper::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType rswell, lswell;
    DspFloatType ticktock[PERIOD];
    if((Pmetro) && (Pplay) && (!Pstop)) ticker.metronomeout(ticktock);

    for (i = 0; i < PERIOD; i++) {

        if((Pplay) && (!Pstop)) {
            if(Precord) {
                if((Prec1) && (PT1)) {
                    ldelay[kl] += pregain1*smpsl[i];
                    rdelay[kl] += pregain1*smpsr[i];
                }
                if((Prec2) && (PT2)) {
                    t2ldelay[kl2] += pregain2*smpsl[i];
                    t2rdelay[kl2] += pregain2*smpsr[i];
                }

            }


            if(!Pclear) {
                if (++kl >= dl)
                    kl = 0;
                rvkl = dl - 1 - kl;
                if (++kl2 >= dl2)
                    kl2 = 0;
                rvkl2 = dl2 - 1 - kl2;
                if((Plink) || (PT1)) timeposition(kl);
                else timeposition(kl2);


            }

            if(Preverse) {

                lswell =	(DspFloatType)(abs(kl - rvkl)) * Srate_Attack_Coeff;
                if (lswell <= PI) {
                    lswell = 0.5f * (1.0f - cosf(lswell));  //Clickless transition
                    efxoutl[i] = (fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2]) * lswell;   //Volume ducking near zero crossing.
                } else {
                    efxoutl[i] = fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2];
                }

                rswell = 	(DspFloatType)(abs(kl - rvkl)) * Srate_Attack_Coeff;
                if (rswell <= PI) {
                    rswell = 0.5f * (1.0f - cosf(rswell));   //Clickless transition
                    efxoutr[i] = ( fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2] )* rswell;  //Volume ducking near zero crossing.
                } else {
                    efxoutr[i] = fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2];
                }

            } else {

                efxoutl[i]= fade1*ldelay[kl] + fade2*t2ldelay[kl2];
                efxoutr[i]= fade1*rdelay[kl] + fade2*t2rdelay[kl2];

            }

        } else {
            efxoutl[i]= 0.0f;
            efxoutr[i]= 0.0f;
        }

        if((Pmetro) && (Pplay) && (!Pstop)) {
            efxoutl[i] += ticktock[i]*mvol;  //if you want to hear the metronome in Looper
            efxoutr[i] += ticktock[i]*mvol;
        }
    };


};


/*
 * Parameter control
 */

void Looper::setmvol(int value)
{
    mvol = 2.0f*(DspFloatType)value/100.0f;
}


void Looper::settempo(int value)
{
    Ptempo=value;
    setbar(Pbar);
    ticker.set_tempo(value);
}

int Looper::set_len(int value)
{

    int x = value/barlen;
    int a1=0;
    int a2=0;

    a1 = abs(value-(x*barlen));
    a2 = abs(value-((x+1)*barlen));

    if(a2<a1) return(((x+1)*barlen)+1);

    return(x*barlen+1);

}


int Looper::cal_len(int value)
{

    int lenbar = lrintf((DspFloatType)value*60.0/(DspFloatType)Ptempo*fSAMPLE_RATE);

    looper_ts = lenbar / value;
    return(lenbar);
// return(lrintf((DspFloatType)value*60.0/(DspFloatType)Ptempo*fSAMPLE_RATE));

}


void
Looper::timeposition(int value)
{

    looper_bar = value / barlen + 1;
    looper_qua = (value % barlen) / looper_ts + 1;

}


void Looper::setbar(int value)
{

    Pbar=value;
    switch(value) {

    case 0:
        barlen=cal_len(2);
        if(!Pms)ticker.set_meter(2);
        break;
    case 1:
        barlen=cal_len(3);
        if(!Pms)ticker.set_meter(3);
        break;
    case 2:
        barlen=cal_len(4);
        if(!Pms)ticker.set_meter(4);
        break;
    case 3:
        barlen=cal_len(5);
        if(!Pms)ticker.set_meter(5);
        break;
    case 4:
        barlen=cal_len(6);
        if(!Pms)ticker.set_meter(6);
        break;
    case 5:
        barlen=cal_len(7);
        if(!Pms)ticker.set_meter(7);
        break;
    case 6:
        barlen=cal_len(9);
        if(!Pms)ticker.set_meter(9);
        break;
    case 7:
        barlen=cal_len(11);
        if(!Pms)ticker.set_meter(11);
        break;
    }


}



void Looper::setfade ()
{
    fade1 = (DspFloatType) (Pfade1)/64.0f;
    fade2 = (DspFloatType) (Pfade2)/64.0f;
    pregain1 = 1.015f/(fade1 + 0.015f);      //this is so material being recorded mixes at the same level as what you hear
    pregain2 = 1.015f/(fade2 + 0.015f) ;      //magic number is ~1/66, keeps record gain from getting too high
    fade1 *= track1gain;
    fade2 *= track2gain;
};
void
Looper::setpreset (int npreset)
{
    const int PRESET_SIZE = 14;
    const int NUM_PRESETS = 2;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Looper 2 seconds
        {64, 0, 1, 0, 1, 0, 64, 1, 0, 1, 64, 1, 0, 0},
        {64, 0, 1, 0, 1, 1, 64, 1, 0, 1, 64, 1, 0, 0}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(30,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            loadpreset (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Looper::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        Pvolume = value;
        outvolume = (DspFloatType)Pvolume / 127.0f;

        break;

    case 1:	//Play at current pointer position

        if(Pplay) {
            Pplay = 0;
        } else {
            Pplay = (PT1 || PT2);  //prevents accidental record state when niether track is active
            if(!Pstop) {
                //if stop was pushed last time, then resume where it left off
                if(PT1)kl = 0;
                if(PT2)kl2 = 0;
                ticker.cleanup();
            }
            Pstop = 0;
        }
        if(Pstop) {
            Pstop = 0;
            Pplay = 0;
        }
        Pclear=0;

        break;

    case 2:	//stop and reset pointer to 0
        if(Pstop) Pstop=0;
        else Pstop = 1;
        if(Precord) {
            Pstop=0;
            if((first_time1 && Prec1) && PT1) {
                dl = set_len(kl);
                //printf("dl %d\n",dl);
                first_time1 = 0;
                if(Plink) {
                    dl2 = dl;
                }
                kl = 0;
                ticker.cleanup();

            }
            if((first_time2 && Prec2) && PT2) {
                dl2 = set_len(kl2);
                first_time2 = 0;
                if(Plink) {
                    dl = dl2;
                }
                kl2 = 0;
                ticker.cleanup();
            }

            Precord = 0;
            Pplay = 0;
        }
        break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
        if(Precord) {
            if((first_time1 && Prec1) && PT1) {
                dl = set_len(kl);
                first_time1 = 0;
                if(Plink) {
                    dl2 = dl;
                }
                kl = 0;
                ticker.cleanup();
            }
            if((first_time2 && Prec2) && PT2) {
                dl2 = set_len(kl2);
                first_time2 = 0;
                if(Plink) {
                    dl = dl2;
                }
                kl2 = 0;
                ticker.cleanup();
            }
            Precord = 0;
            Pplay = rplaystate;
            if(Pautoplay)  Pplay = 1;
            Pstop = 0;
        } else {
            Precord = 1;
            rplaystate = Pplay;
            if(Pautoplay) {
                Pplay = 1;
                if((first_time1 && Prec1) || (first_time2 && Prec2)) ticker.cleanup();
            } else Pplay = 0;
        }
        Pstop = 0;
        Pclear = 0;
        break;
    case 4:
        Pclear = 1;    //Clear everything and erase the loop
        if(PT1) first_time1 = 1;
        if(PT2) first_time2 = 1;
        if((PT1) && (PT2)) Pplay = 0;
        Precord = 0;
        if(PT1)kl = 0;
        if(PT2)kl2 = 0;
        ticker.cleanup();
        initdelays ();
        break;
    case 5:
        Preverse = value;		//Playback in reverse
        break;
    case 6:
        Pfade1 = value;
        setfade ();
        break;
    case 7:
        if(PT1) {
            PT1 = 0;
        } else {
            PT1 = 1;
            if(!Plink) {
                if(first_time1)initdelays();
                PT2 = 0;       //if they aren't linked in time, then you want to toggle 1&2
                track2gain = 0.0f;
                kl = 0;
                ticker.cleanup();
                setfade();
            }

        }
        track1gain = (DspFloatType) PT1;
        setfade ();
        break;
    case 8:
        if(PT2) {
            PT2 = 0;
        } else {
            PT2 = 1;
            if(!Plink) {
                PT1 = 0;
                if(first_time2)initdelays();
                kl2 = 0;
                ticker.cleanup();
                track1gain = 0.0f;
            }

        }
        track2gain = (DspFloatType) PT2;
        setfade ();
        break;
    case 9:
        Pautoplay = value;
        break;
    case 10:
        Pfade2 = value;
        setfade();
        break;
    case 11:
        Prec1 = value;
        break;
    case 12:
        Prec2 = value;
        break;
    case 13:
        Plink = value;
        if(Plink) {
            if(Prec1) dl2 = dl;
            if(Prec2) dl = dl2;  //if both are true, then it is only a redundant assignment
        }
        break;
    case 14:
        settempo(value);
        break;
    case 15:
        setbar(value);
        break;
    case 16:
        Pmetro=value;
        break;
    case 17:
        Pms=value;
        if(Pms==0) setbar(Pbar);
        if(Pms==1) ticker.set_meter(1);
        if(Pms==2) ticker.set_meter(0);

        break;

    };

};

int
Looper::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pplay);
        break;
    case 2:
        return (Pstop);
        break;
    case 3:
        return (Precord);
        break;
    case 4:
        return (Pclear);
        break;
    case 5:
        return (Preverse);
        break;
    case 6:
        return (Pfade1);
        break;
    case 7:
        return (PT1);
        break;
    case 8:
        return (PT2);
        break;
    case 9:
        return (Pautoplay);
        break;
    case 10:
        return (Pfade2);
        break;
    case 11:
        return (Prec1);
        break;
    case 12:
        return (Prec2);
        break;
    case 13:
        return(Plink);
        break;
    case 14:
        return(Ptempo);
        break;
    case 15:
        return(Pbar);
        break;
    case 16:
        return(Pmetro);
        break;
    case 17:
        return(Pms);
        break;


    };
    return (0);			//in case of bogus parameter number
};

void Looper::getstate ()
{
    progstate[0] = Pplay;
    progstate[1] = Pstop;
    progstate[2] = Precord;
    progstate[3] = Pclear;
    progstate[4] = PT1;
    progstate[5] = PT2;
};

void
Looper::loadpreset (int npar, int value)
{
    switch (npar) {
    case 0:
        Pvolume = value;
        outvolume = (DspFloatType)Pvolume / 127.0f;

        break;
    case 1:	//Play at current pointer position
        Pplay = value;
        break;
    case 2:	//stop and reset pointer to 0
        Pstop = value;
        break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
        Precord = value;
        break;
    case 4:
        Pclear = 1;    //Clear everything and erase the loop
        initdelays ();
        break;
    case 5:
        Preverse = value;		//Playback in reverse
        break;
    case 6:
        Pfade1 = value;
        setfade ();
        break;
    case 7:
        PT1 = value;
        track1gain = (DspFloatType) PT1;
        setfade ();
        break;
    case 8:
        PT2 = value;
        track2gain = (DspFloatType) PT2;
        setfade ();
        break;
    case 9:
        Pautoplay = value;
        break;
    case 10:
        Pfade2 = value;
        setfade();
        break;
    case 11:
        Prec1 = value;
        break;
    case 12:
        Prec2 = value;
        break;
    case 13:
        Plink = value;
        break;



    };

    Pclear = 1;    //Clear everything and erase the loop
    if(PT1) first_time1 = 1;
    if(PT2) first_time2 = 1;
    if((PT1) && (PT2)) Pplay = 0;
    Precord = 0;
    getstate();
};



/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



MBDist::MBDist (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lowl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    lowr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);


    lpf1l = new AnalogFilter (2, 500.0f, .7071f, 0);
    lpf1r = new AnalogFilter (2, 500.0f, .7071f, 0);
    hpf1l = new AnalogFilter (3, 500.0f, .7071f, 0);
    hpf1r = new AnalogFilter (3, 500.0f, .7071f, 0);
    lpf2l = new AnalogFilter (2, 2500.0f, .7071f, 0);
    lpf2r = new AnalogFilter (2, 2500.0f, .7071f, 0);
    hpf2l = new AnalogFilter (3, 2500.0f, .7071f, 0);
    hpf2r = new AnalogFilter (3, 2500.0f, .7071f, 0);
    DCl = new AnalogFilter (3, 30, 1, 0);
    DCr = new AnalogFilter (3, 30, 1, 0);
    DCl->setfreq (30.0f);
    DCr->setfreq (30.0f);


    mbwshape1l = new Waveshaper();
    mbwshape2l = new Waveshaper();
    mbwshape3l = new Waveshaper();

    mbwshape1r = new Waveshaper();
    mbwshape2r = new Waveshaper();
    mbwshape3r = new Waveshaper();

    //default values
    Ppreset = 0;
    Pvolume = 50;
    Plrcross = 40;
    Pdrive = 90;
    Plevel = 64;
    PtypeL = 0;
    PtypeM = 0;
    PtypeH = 0;
    PvolL = 0;
    PvolM = 0;
    PvolH = 0;
    Pnegate = 0;
    Pstereo = 0;

    setpreset (Ppreset);
    cleanup ();
};

MBDist::~MBDist ()
{
};

/*
 * Cleanup the effect
 */
void
MBDist::cleanup ()
{
    lpf1l->cleanup ();
    hpf1l->cleanup ();
    lpf1r->cleanup ();
    hpf1r->cleanup ();
    lpf2l->cleanup ();
    hpf2l->cleanup ();
    lpf2r->cleanup ();
    hpf2r->cleanup ();
    DCl->cleanup();
    DCr->cleanup();

};
/*
 * Effect output
 */
void
MBDist::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType l, r, lout, rout;

    DspFloatType inputvol = powf (5.0f, ((DspFloatType)Pdrive - 32.0f) / 127.0f);
    if (Pnegate != 0)
        inputvol *= -1.0f;


    if (Pstereo) {
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] = smpsl[i] * inputvol * 2.0f;
            efxoutr[i] = smpsr[i] * inputvol * 2.0f;
        };
    } else {
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] =
                (smpsl[i]  +  smpsr[i] ) * inputvol;
        };
    };


    memcpy(lowl,efxoutl,sizeof(DspFloatType) * PERIOD);
    memcpy(midl,efxoutl,sizeof(DspFloatType) * PERIOD);
    memcpy(highl,efxoutl,sizeof(DspFloatType) * PERIOD);

    lpf1l->filterout(lowl);
    hpf1l->filterout(midl);
    lpf2l->filterout(midl);
    hpf2l->filterout(highl);

    if(volL> 0)  mbwshape1l->waveshapesmps (PERIOD, lowl, PtypeL, PdriveL, 1);
    if(volM> 0)  mbwshape2l->waveshapesmps (PERIOD, midl, PtypeM, PdriveM, 1);
    if(volH> 0)  mbwshape3l->waveshapesmps (PERIOD, highl, PtypeH, PdriveH, 1);


    if(Pstereo) {
        memcpy(lowr,efxoutr,sizeof(DspFloatType) * PERIOD);
        memcpy(midr,efxoutr,sizeof(DspFloatType) * PERIOD);
        memcpy(highr,efxoutr,sizeof(DspFloatType) * PERIOD);

        lpf1r->filterout(lowr);
        hpf1r->filterout(midr);
        lpf2r->filterout(midr);
        hpf2r->filterout(highr);

        if(volL> 0)  mbwshape1r->waveshapesmps (PERIOD, lowr, PtypeL, PdriveL, 1);
        if(volM> 0)  mbwshape2r->waveshapesmps (PERIOD, midr, PtypeM, PdriveM, 1);
        if(volH> 0)  mbwshape3r->waveshapesmps (PERIOD, highr, PtypeH, PdriveH, 1);


    }

    for (i = 0; i < PERIOD; i++) {
        efxoutl[i]=lowl[i]*volL+midl[i]*volM+highl[i]*volH;
        if (Pstereo) efxoutr[i]=lowr[i]*volL+midr[i]*volM+highr[i]*volH;
    }

    if (!Pstereo) memcpy(efxoutr, efxoutl, sizeof(DspFloatType)* PERIOD);


    DspFloatType level = dB2rap (60.0f * (DspFloatType)Plevel / 127.0f - 40.0f);

    for (i = 0; i < PERIOD; i++) {
        lout = efxoutl[i];
        rout = efxoutr[i];

        l = lout * (1.0f - lrcross) + rout * lrcross;
        r = rout * (1.0f - lrcross) + lout * lrcross;

        efxoutl[i] = l * 2.0f * level * panning;
        efxoutr[i] = r * 2.0f * level * (1.0f -panning);

    };

    DCr->filterout (efxoutr);
    DCl->filterout (efxoutl);



};


/*
 * Parameter control
 */
void
MBDist::setvolume (int value)
{
    Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
MBDist::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
};


void
MBDist::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f * 1.0f;
};

void
MBDist::setCross1 (int value)
{
    Cross1 = value;
    lpf1l->setfreq ((DspFloatType)value);
    lpf1r->setfreq ((DspFloatType)value);
    hpf1l->setfreq ((DspFloatType)value);
    hpf1r->setfreq ((DspFloatType)value);


};

void
MBDist::setCross2 (int value)
{
    Cross2 = value;
    hpf2l->setfreq ((DspFloatType)value);
    hpf2r->setfreq ((DspFloatType)value);
    lpf2l->setfreq ((DspFloatType)value);
    lpf2r->setfreq ((DspFloatType)value);


};


void
MBDist::setpreset (int npreset)
{
    const int PRESET_SIZE = 15;
    const int NUM_PRESETS = 8;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Saturation
        {0, 64, 0, 41, 64, 26, 19, 26, 41, 20, 35, 0, 400, 1200, 0},
        //Dist 1
        {0, 64, 64, 20, 64, 0, 14, 13, 38, 49, 40, 0, 288, 1315, 0},
        //Soft
        {0, 64, 0, 32, 64, 6, 13, 6, 50, 70, 50, 0, 400, 1800, 0},
        //Modulated
        {0, 64, 0, 36, 64, 18, 17, 18, 40, 70, 30, 0, 500, 2200, 0},
        //Crunch
        {0, 64, 0, 24, 64, 19, 14, 19, 30, 80, 30, 0, 800, 1800, 0},
        //Dist 2
        {0, 64, 0, 64, 64, 22, 27, 22, 25, 50, 25, 0, 440, 1500, 0},
        //Dist 3
        {0, 64, 0, 64, 64, 27, 22, 27, 50, 69, 50, 0, 800, 1200, 0},
        //Dist 4
        {0, 64, 0, 30, 64, 19, 25, 26, 20, 51, 83, 0, 329, 800, 0}

    };
    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(23,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
    cleanup ();
};


void
MBDist::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setlrcross (value);
        break;
    case 3:
        Pdrive = value;
        PdriveL = (int)((DspFloatType)Pdrive*volL);
        PdriveM = (int)((DspFloatType)Pdrive*volM);
        PdriveH = (int)((DspFloatType)Pdrive*volH);
        break;
    case 4:
        Plevel = value;
        break;
    case 5:
        PtypeL = value;
        break;
    case 6:
        PtypeM = value;
        break;
    case 7:
        PtypeH = value;
        break;
    case 8:
        PvolL = value;
        volL = (DspFloatType) value /100.0;
        PdriveL = (int)((DspFloatType)Pdrive*volL);
        break;
    case 9:
        PvolM = value;
        volM = (DspFloatType) value /100.0;
        PdriveM = (int)((DspFloatType)Pdrive*volM);
        break;
    case 10:
        PvolH = value;
        volH = (DspFloatType) value /100.0;
        PdriveH = (int)((DspFloatType)Pdrive*volH);
        break;
    case 11:
        Pnegate = value;
        break;
    case 12:
        setCross1 (value);
        break;
    case 13:
        setCross2 (value);
        break;
    case 14:
        Pstereo = value;
        break;
    };
};

int
MBDist::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Plrcross);
        break;
    case 3:
        return (Pdrive);
        break;
    case 4:
        return (Plevel);
        break;
    case 5:
        return (PtypeL);
        break;
    case 6:
        return (PtypeM);
        break;
    case 7:
        return (PtypeH);
        break;
    case 8:
        return (PvolL);
        break;
    case 9:
        return (PvolM);
        break;
    case 10:
        return (PvolH);
        break;
    case 11:
        return (Pnegate);
        break;
    case 12:
        return (Cross1);
        break;
    case 13:
        return (Cross2);
        break;
    case 14:
        return (Pstereo);
        break;
    };
    return (0);			//in case of bogus parameter number
};


MBVvol::MBVvol (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lowl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    lowr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midll = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midlr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midhl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    midhr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    highr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);


    lpf1l = new AnalogFilter (2, 500.0f, .7071f, 0);
    lpf1r = new AnalogFilter (2, 500.0f, .7071f, 0);
    hpf1l = new AnalogFilter (3, 500.0f, .7071f, 0);
    hpf1r = new AnalogFilter (3, 500.0f, .7071f, 0);
    lpf2l = new AnalogFilter (2, 2500.0f, .7071f, 0);
    lpf2r = new AnalogFilter (2, 2500.0f, .7071f, 0);
    hpf2l = new AnalogFilter (3, 2500.0f, .7071f, 0);
    hpf2r = new AnalogFilter (3, 2500.0f, .7071f, 0);
    lpf3l = new AnalogFilter (2, 5000.0f, .7071f, 0);
    lpf3r = new AnalogFilter (2, 5000.0f, .7071f, 0);
    hpf3l = new AnalogFilter (3, 5000.0f, .7071f, 0);
    hpf3r = new AnalogFilter (3, 5000.0f, .7071f, 0);


    //default values
    Ppreset = 0;
    Pvolume = 50;
    coeff = 1.0 / (DspFloatType) PERIOD;
    volL=volLr=volML=volMLr=volMH=volMHr=volH=volHr=2.0f;

    setpreset (Ppreset);
    cleanup ();
};

MBVvol::~MBVvol ()
{
};

/*
 * Cleanup the effect
 */
void
MBVvol::cleanup ()
{
    lpf1l->cleanup ();
    hpf1l->cleanup ();
    lpf1r->cleanup ();
    hpf1r->cleanup ();
    lpf2l->cleanup ();
    hpf2l->cleanup ();
    lpf2r->cleanup ();
    hpf2r->cleanup ();
    lpf3l->cleanup ();
    hpf3l->cleanup ();
    lpf3r->cleanup ();
    hpf3r->cleanup ();

};
/*
 * Effect output
 */
void
MBVvol::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;


    memcpy(lowl,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(midll,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(midhl,smpsl,sizeof(DspFloatType) * PERIOD);
    memcpy(highl,smpsl,sizeof(DspFloatType) * PERIOD);

    lpf1l->filterout(lowl);
    hpf1l->filterout(midll);
    lpf2l->filterout(midll);
    hpf2l->filterout(midhl);
    lpf3l->filterout(midhl);
    hpf3l->filterout(highl);

    memcpy(lowr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(midlr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(midhr,smpsr,sizeof(DspFloatType) * PERIOD);
    memcpy(highr,smpsr,sizeof(DspFloatType) * PERIOD);

    lpf1r->filterout(lowr);
    hpf1r->filterout(midlr);
    lpf2r->filterout(midlr);
    hpf2r->filterout(midhr);
    lpf3r->filterout(midhr);
    hpf3r->filterout(highr);

    lfo1.effectlfoout (&lfo1l, &lfo1r);
    lfo2.effectlfoout (&lfo2l, &lfo2r);

    d1=(lfo1l-v1l)*coeff;
    d2=(lfo1r-v1r)*coeff;
    d3=(lfo2l-v2l)*coeff;
    d4=(lfo2r-v2r)*coeff;

    for (i = 0; i < PERIOD; i++) {

        setCombi(Pcombi);

        efxoutl[i]=lowl[i]*volL+midll[i]*volML+midhl[i]*volMH+highl[i]*volH;
        efxoutr[i]=lowr[i]*volLr+midlr[i]*volMLr+midhr[i]*volMHr+highr[i]*volHr;
    }

};


/*
 * Parameter control
 */
void
MBVvol::setvolume (int value)
{
    Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
MBVvol::setCombi(int value)
{


    v1l+=d1;
    v1r+=d2;
    v2l+=d3;
    v2r+=d4;


    switch(value) {
    case 0:
        volL=v1l;
        volLr=v1r;
        volML=v1l;
        volMLr=v1r;
        volMH=v2l;
        volMHr=v2r;
        volH =v2l;
        volHr=v2r;
        break;
    case 1:
        volL=v1l;
        volLr=v1r;
        volML=v2l;
        volMLr=v2r;
        volMH=v2l;
        volMHr=v2r;
        volH =v1l;
        volHr=v1r;
        break;
    case 2:
        volL=v1l;
        volLr=v1r;
        volML=v2l;
        volMLr=v2r;
        volMH=v1l;
        volMHr=v1r;
        volH =v2l;
        volHr=v2r;
        break;
    case 3:
        volL=1.0f;
        volLr=1.0f;
        volML=v1l;
        volMLr=v1r;
        volMH=v1l;
        volMHr=v1r;
        volH =1.0f;
        volHr=1.0f;
        break;
    case 4:
        volL=1.0f;
        volLr=1.0f;
        volML=v1l;
        volMLr=v1r;
        volMH=v2l;
        volMHr=v2r;
        volH =1.0f;
        volHr=1.0f;
        break;
    case 5:
        volL=0.0f;
        volLr=0.0f;
        volML=v1l;
        volMLr=v1r;
        volMH=v1l;
        volMHr=v1r;
        volH =0.0f;
        volHr=0.0f;
        break;
    case 6:
        volL=0.0f;
        volLr=0.0f;
        volML=v1l;
        volMLr=v1r;
        volMH=v2l;
        volMHr=v2r;
        volH =0.0f;
        volHr=0.0f;
        break;
    case 7:
        volL=v1l;
        volLr=v1r;
        volML=1.0f;
        volMLr=1.0f;
        volMH=1.0f;
        volMHr=1.0f;
        volH =v1l;
        volHr=v1r;
        break;
    case 8:
        volL=v1l;
        volLr=v1r;
        volML=1.0f;
        volMLr=1.0f;
        volMH=1.0f;
        volMHr=1.0f;
        volH =v2l;
        volHr=v2r;
        break;
    case 9:
        volL=v1l;
        volLr=v1r;
        volML=0.0f;
        volMLr=0.0f;
        volMH=0.0f;
        volMHr=0.0f;
        volH =v1l;
        volHr=v1r;
        break;
    case 10:
        volL=v1l;
        volLr=v1r;
        volML=0.0f;
        volMLr=0.0f;
        volMH=0.0f;
        volMHr=0.0f;
        volH =v2l;
        volHr=v2r;
        break;

    }


}

void
MBVvol::setCross1 (int value)
{
    Cross1 = value;
    lpf1l->setfreq ((DspFloatType)value);
    lpf1r->setfreq ((DspFloatType)value);
    hpf1l->setfreq ((DspFloatType)value);
    hpf1r->setfreq ((DspFloatType)value);

};

void
MBVvol::setCross2 (int value)
{
    Cross2 = value;
    hpf2l->setfreq ((DspFloatType)value);
    hpf2r->setfreq ((DspFloatType)value);
    lpf2l->setfreq ((DspFloatType)value);
    lpf2r->setfreq ((DspFloatType)value);

};

void
MBVvol::setCross3 (int value)
{
    Cross3 = value;
    hpf3l->setfreq ((DspFloatType)value);
    hpf3r->setfreq ((DspFloatType)value);
    lpf3l->setfreq ((DspFloatType)value);
    lpf3r->setfreq ((DspFloatType)value);

};


void
MBVvol::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Vary1
        {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0},
        //Vary2
        {0, 80, 0, 64, 40, 0, 0, 120, 600, 2300, 1},
        //Vary3
        {0, 120, 0, 64, 40, 0, 0, 800, 2300, 5200, 2}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(28,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
    cleanup ();
};


void
MBVvol::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        lfo1.Pfreq = value;
        lfo1.updateparams ();
        break;
    case 2:
        lfo1.PLFOtype = value;
        lfo1.updateparams ();
        break;
    case 3:
        lfo1.Pstereo = value;
        lfo1.updateparams ();
        break;
    case 4:
        lfo2.Pfreq = value;
        lfo2.updateparams ();
        break;
    case 5:
        lfo2.PLFOtype = value;
        lfo2.updateparams ();
        break;
    case 6:
        lfo2.Pstereo = value;
        lfo2.updateparams ();
        break;
    case 7:
        setCross1 (value);
        break;
    case 8:
        setCross2 (value);
        break;
    case 9:
        setCross3(value);
        break;
    case 10:
        Pcombi=value;
        break;
    };
};

int
MBVvol::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (lfo1.Pfreq);
        break;
    case 2:
        return (lfo1.PLFOtype);
        break;
    case 3:
        return (lfo1.Pstereo);
        break;
    case 4:
        return (lfo2.Pfreq);
        break;
    case 5:
        return (lfo2.PLFOtype);
        break;
    case 6:
        return (lfo2.Pstereo);
        break;
    case 7:
        return (Cross1);
        break;
    case 8:
        return (Cross2);
        break;
    case 9:
        return (Cross3);
        break;
    case 10:
        return (Pcombi);
        break;

    };
    return (0);			//in case of bogus parameter number
};

MusicDelay::MusicDelay (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning1 = 64;
    Ppanning2 = 64;
    Pgain1 = 64;
    Pgain2 = 64;
    Pdelay1 = 60;
    Pdelay2 = 60;
    Plrdelay = 100;
    Plrcross = 100;
    Pfb1 = 40;
    Pfb2 = 40;
    Ptempo = 100;
    Phidamp = 60;

    maxx_delay = SAMPLE_RATE * MAX_DELAY;
    ldelay1 = new DspFloatType[maxx_delay];
    rdelay1 = new DspFloatType[maxx_delay];
    ldelay2 = new DspFloatType[maxx_delay];
    rdelay2 = new DspFloatType[maxx_delay];

    dl1 = maxx_delay-1;
    dl2 = maxx_delay-1;
    dr1 = maxx_delay-1;
    dr2 = maxx_delay-1;



    lrdelay = 0;

    setpreset (Ppreset);
    cleanup ();
};

MusicDelay::~MusicDelay ()
{
};

/*
 * Cleanup the effect
 */
void
MusicDelay::cleanup ()
{
    int i;
    for (i = 0; i < dl1; i++)
        ldelay1[i] = 0.0;
    for (i = 0; i < dr1; i++)
        rdelay1[i] = 0.0;
    for (i = 0; i < dl2; i++)
        ldelay2[i] = 0.0;
    for (i = 0; i < dr2; i++)
        rdelay2[i] = 0.0;

    oldl1 = 0.0;
    oldr1 = 0.0;
    oldl2 = 0.0;
    oldr2 = 0.0;
};


/*
 * Initialize the delays
 */
void
MusicDelay::initdelays ()
{
    int i;
    kl1 = 0;
    kr1 = 0;

    if (delay1 >= maxx_delay) delay1=maxx_delay-1;
    if (delay2 >= maxx_delay) delay2=maxx_delay-1;

    dl1 = delay1;
    if (dl1 < 1)
        dl1 = 1;
    dr1 = delay1;
    if (dr1 < 1)
        dr1 = 1;
    kl2 = 0;
    kr2 = 0;
    dl2 = delay2 + lrdelay;
    if (dl2 < 1)
        dl2 = 1;
    dr2 = delay2 + lrdelay;
    if (dr2 < 1)
        dr2 = 1;

    for (i = dl1; i < maxx_delay; i++)
        ldelay1[i] = 0.0;
    for (i = dl2; i < maxx_delay; i++)
        ldelay2[i] = 0.0;

    for (i = dr1; i < maxx_delay; i++)
        rdelay1[i] = 0.0;
    for (i = dr2; i < maxx_delay; i++)
        rdelay2[i] = 0.0;



    cleanup ();
};

/*
 * Effect output
 */
void
MusicDelay::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType l1, r1, ldl1, rdl1, l2, r2, ldl2, rdl2;

    for (i = 0; i < PERIOD; i++) {
        ldl1 = ldelay1[kl1];
        rdl1 = rdelay1[kr1];
        l1 = ldl1 * (1.0f - lrcross) + rdl1 * lrcross;
        r1 = rdl1 * (1.0f - lrcross) + ldl1 * lrcross;
        ldl1 = l1;
        rdl1 = r1;

        ldl2 = ldelay2[kl2];
        rdl2 = rdelay2[kr2];
        l2 = ldl2 * (1.0f - lrcross) + rdl2 * lrcross;
        r2 = rdl2 * (1.0f - lrcross) + ldl2 * lrcross;
        ldl2 = l2;
        rdl2 = r2;

        ldl1 = smpsl[i] * gain1 * panning1 - ldl1 * fb1;
        rdl1 = smpsr[i] * gain1 * (1.0f - panning1) - rdl1 * fb1;

        ldl2 = smpsl[i] * gain2 * panning2 - ldl2 * fb2;
        rdl2 = smpsr[i] * gain2 * (1.0f - panning2) - rdl2 * fb2;

        efxoutl[i] = (ldl1 + ldl2) * 2.0f;
        efxoutr[i] = (rdl1 + rdl2) * 2.0f;



        //LowPass Filter
        ldelay1[kl1] = ldl1 = ldl1 * hidamp + oldl1 * (1.0f - hidamp);
        rdelay1[kr1] = rdl1 = rdl1 * hidamp + oldr1 * (1.0f - hidamp);
        oldl1 = ldl1;
        oldr1 = rdl1;

        ldelay2[kl2] = ldl2 = ldl2 * hidamp + oldl2 * (1.0f - hidamp);
        rdelay2[kr2] = rdl2 = rdl2 * hidamp + oldr2 * (1.0f - hidamp);
        oldl2 = ldl2;
        oldr2 = rdl2;

        if (++kl1 >= dl1)
            kl1 = 0;
        if (++kr1 >= dr1)
            kr1 = 0;

        if (++kl2 >= dl2)
            kl2 = 0;
        if (++kr2 >= dr2)
            kr2 = 0;

    };

};


/*
 * Parameter control
 */
void
MusicDelay::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType) Pvolume / 127.0f;

};

void
MusicDelay::setpanning (int num, int Ppanning)
{

    switch (num) {
    case 1:
        this->Ppanning1 = Ppanning;
        panning1 = ((DspFloatType) Ppanning1 + 0.5f) / 127.0f;
        break;
    case 2:
        this->Ppanning2 = Ppanning;
        panning2 = ((DspFloatType) Ppanning2 + 0.5f) / 127.0f;
        break;
    }

};

void
MusicDelay::setdelay (int num, int Pdelay)
{

    DspFloatType ntem = 60.0f / (DspFloatType) Ptempo;
    DspFloatType coef;
    switch (num) {
    case 1:
        this->Pdelay1 = Pdelay;
        break;
    case 2:
        this->Pdelay2 = Pdelay;
        break;
    case 3:
        this->Plrdelay = Pdelay;
    }

    delay1 = lrintf ((ntem / (DspFloatType)Pdelay1) * fSAMPLE_RATE);

    if (Plrdelay != 0)
        coef = ntem / (DspFloatType)Plrdelay;
    else
        coef = 0;

    delay2 = lrintf ((coef + (ntem / (DspFloatType)Pdelay2)) * fSAMPLE_RATE);


    initdelays ();

};

void
MusicDelay::setgain (int num, int PGain)
{

    switch (num) {
    case 1:
        this->Pgain1 = PGain;
        gain1 = (DspFloatType)Pgain1 / 127.0f;
        break;
    case 2:
        this->Pgain2 = PGain;
        gain2 = (DspFloatType)Pgain2 / 127.0f;
        break;
    }

};

void
MusicDelay::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType) Plrcross / 127.0f * 1.0f;
};

void
MusicDelay::setfb (int num, int Pfb)
{
    switch (num) {
    case 1:
        this->Pfb1 = Pfb;
        fb1 = (DspFloatType) Pfb1 / 127.0f;
        break;
    case 2:
        this->Pfb2 = Pfb;
        fb2 = (DspFloatType) Pfb2 / 127.0f;
        break;
    }
};

void
MusicDelay::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (DspFloatType) Phidamp / 127.0f;
};

void
MusicDelay::settempo (int Ptempo)
{

    DspFloatType coef = 0.0;

    this->Ptempo = Ptempo;
    DspFloatType ntem = 60.0f / (DspFloatType) Ptempo;


    delay1 = lrintf ((ntem / (DspFloatType)Pdelay1) * fSAMPLE_RATE);
    if (Plrdelay != 0)
        coef = ntem / (DspFloatType)Plrdelay;
    else
        coef = 0;
    delay2 = lrintf ((coef + (ntem / (DspFloatType)Pdelay2)) * fSAMPLE_RATE);

    initdelays ();



};

void
MusicDelay::setpreset (int npreset)
{
    const int PRESET_SIZE = 13;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75},
        //Echo 2
        {67, 0, 3, 7, 0, 59, 0, 127, 6, 69, 60, 127, 127}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(15,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = (int) npreset;
};


void
MusicDelay::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (1, value);
        break;
    case 2:
        setdelay (1, value);
        break;
    case 3:
        setdelay (3, value);
        break;
    case 4:
        setlrcross (value);
        break;
    case 5:
        setfb (1, value);
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        setpanning (2, value);
        break;
    case 8:
        setdelay (2, value);
        break;
    case 9:
        setfb (2, value);
        break;
    case 10:
        settempo (value);
        break;
    case 11:
        setgain (1, value);
        break;
    case 12:
        setgain (2, value);
        break;

    };
};

int
MusicDelay::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning1);
        break;
    case 2:
        return (Pdelay1);
        break;
    case 3:
        return (Plrdelay);
        break;
    case 4:
        return (Plrcross);
        break;
    case 5:
        return (Pfb1);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return (Ppanning2);
        break;
    case 8:
        return (Pdelay2);
        break;
    case 9:
        return (Pfb2);
        break;
    case 10:
        return (Ptempo);
        break;
    case 11:
        return (Pgain1);
        break;
    case 12:
        return (Pgain2);
        break;

    };
    return (0);			//in case of bogus parameter number
};



/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */

NewDist::NewDist (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    octoutl = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);
    octoutr = (DspFloatType *) malloc (sizeof (DspFloatType) * PERIOD);



    lpfl = new AnalogFilter (2, 22000, 1, 0);
    lpfr = new AnalogFilter (2, 22000, 1, 0);
    hpfl = new AnalogFilter (3, 20, 1, 0);
    hpfr = new AnalogFilter (3, 20, 1, 0);
    blockDCl = new AnalogFilter (2, 75.0f, 1, 0);
    blockDCr = new AnalogFilter (2, 75.0f, 1, 0);
    wshapel = new Waveshaper();
    wshaper = new Waveshaper();

    blockDCl->setfreq (75.0f);
    blockDCr->setfreq (75.0f);

    DCl = new AnalogFilter (3, 30, 1, 0);
    DCr = new AnalogFilter (3, 30, 1, 0);
    DCl->setfreq (30.0f);
    DCr->setfreq (30.0f);




    filterpars = new FilterParams (0, 64, 64);

    filterpars->Pcategory = 2;
    filterpars->Ptype = 0;
    filterpars->Pfreq = 72;
    filterpars->Pq = 76;
    filterpars->Pstages = 0;
    filterpars->Pgain = 76;



    filterl = new Filter (filterpars);
    filterr = new Filter (filterpars);

    Ppreset=0;
    setpreset (Ppreset);


    //default values
    Ppreset = 0;
    Pvolume = 50;
    Plrcross = 40;
    Pdrive = 1;
    Plevel = 32;
    Ptype = 0;
    Pnegate = 0;
    Plpf = 127;
    Phpf = 0;
    Prfreq = 64;
    Pprefiltering = 0;
    Poctave = 0;
    togglel = 1.0;
    octave_memoryl = -1.0;
    toggler = 1.0;
    octave_memoryr = -1.0;
    octmix=0.0;




    cleanup ();
};

NewDist::~NewDist ()
{
};

/*
 * Cleanup the effect
 */
void
NewDist::cleanup ()
{
    lpfl->cleanup ();
    hpfl->cleanup ();
    lpfr->cleanup ();
    hpfr->cleanup ();
    blockDCr->cleanup ();
    blockDCl->cleanup ();
    DCl->cleanup();
    DCr->cleanup();


};


/*
 * Apply the filters
 */

void
NewDist::applyfilters (DspFloatType * efxoutl, DspFloatType * efxoutr)
{
    lpfl->filterout(efxoutl);
    hpfl->filterout(efxoutl);
    lpfr->filterout(efxoutr);
    hpfr->filterout(efxoutr);

};


/*
 * Effect output
 */
void
NewDist::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType l, r, lout, rout;

    DspFloatType inputvol = .5f;

    if (Pnegate != 0)
        inputvol *= -1.0f;


    if (Pprefiltering != 0)
        applyfilters (smpsl, smpsr);

    //no optimised, yet (no look table)


    wshapel->waveshapesmps (PERIOD, smpsl, Ptype, Pdrive, 2);
    wshaper->waveshapesmps (PERIOD, smpsr, Ptype, Pdrive, 2);




    memcpy(efxoutl,smpsl,PERIOD * sizeof(DspFloatType));
    memcpy(efxoutr,smpsl,PERIOD * sizeof(DspFloatType));





    if (octmix > 0.01f) {
        for (i = 0; i < PERIOD; i++) {
            lout = efxoutl[i];
            rout = efxoutr[i];

            if ( (octave_memoryl < 0.0f) && (lout > 0.0f) ) togglel *= -1.0f;
            octave_memoryl = lout;

            if ( (octave_memoryr < 0.0f) && (rout > 0.0f) ) toggler *= -1.0f;
            octave_memoryr = rout;

            octoutl[i] = lout *  togglel;
            octoutr[i] = rout *  toggler;

        }


        blockDCr->filterout (octoutr);
        blockDCl->filterout (octoutl);
    }



    filterl->filterout(smpsl);
    filterr->filterout(smpsr);



    if (Pprefiltering == 0)
        applyfilters (efxoutl, efxoutr);



    DspFloatType level = dB2rap (60.0f * (DspFloatType)Plevel / 127.0f - 40.0f);

    for (i = 0; i < PERIOD; i++) {
        lout = efxoutl[i];
        rout = efxoutr[i];

        l = lout * (1.0f - lrcross) + rout * lrcross;
        r = rout * (1.0f - lrcross) + lout * lrcross;

        if (octmix > 0.01f) {
            lout = l * (1.0f - octmix) + octoutl[i] * octmix;
            rout = r * (1.0f - octmix) + octoutr[i] * octmix;
        } else {
            lout = l;
            rout = r;
        }

        efxoutl[i] = lout * level * panning;
        efxoutr[i] = rout * level * ( 1.0f - panning);

    };

    DCr->filterout (efxoutr);
    DCl->filterout (efxoutl);


};


/*
 * Parameter control
 */
void
NewDist::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;

    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
NewDist::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
};


void
NewDist::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType) Plrcross / 127.0f * 1.0f;
};

void
NewDist::setlpf (int value)
{
    Plpf = value;
    DspFloatType fr =(DspFloatType)Plpf;
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
NewDist::sethpf (int value)
{
    Phpf = value;
    DspFloatType fr = (DspFloatType)Phpf;
    hpfl->setfreq (fr);
    hpfr->setfreq (fr);
};


void
NewDist::setoctave (int Poctave)
{
    this->Poctave = Poctave;
    octmix = (DspFloatType) (Poctave) / 127.0f;
};


void
NewDist::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //NewDist 1
        {0, 64, 64, 83, 65, 15, 0, 2437, 169, 68, 0},
        //NewDist 2
        {0, 64, 64, 95, 45, 6, 0, 3459, 209, 60, 1},
        //NewDist 3
        {0, 64, 64, 43, 77, 16, 0, 2983, 118, 83, 0}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(17,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;
    cleanup ();
};


void
NewDist::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setlrcross (value);
        break;
    case 3:
        Pdrive = value;
        break;
    case 4:
        Plevel = value;
        break;
    case 5:
        Ptype = value;
        break;
    case 6:
        if (value > 1)
            value = 1;
        Pnegate = value;
        break;
    case 7:
        setlpf (value);
        break;
    case 8:
        sethpf (value);
        break;
    case 9:
        Prfreq = value;
        rfreq = expf (powf ((DspFloatType)value / 127.0f, 0.5f) * logf (25000.0f)) + 40.0f;
        filterl->setfreq(rfreq);
        filterr->setfreq(rfreq);

        break;
    case 10:
        Pprefiltering = value;
        break;
    case 11:
        setoctave (value);
        break;


    };
};

int
NewDist::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Plrcross);
        break;
    case 3:
        return (Pdrive);
        break;
    case 4:
        return (Plevel);
        break;
    case 5:
        return (Ptype);
        break;
    case 6:
        return (Pnegate);
        break;
    case 7:
        return (Plpf);
        break;
    case 8:
        return (Phpf);
        break;
    case 9:
        return (Prfreq);
        break;
    case 10:
        return (Pprefiltering);
        break;
    case 11:
        return (Poctave);
        break;


    };
    return (0);			//in case of bogus parameter number
};



Opticaltrem::Opticaltrem (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    R1 = 2700.0f;	   //tremolo circuit series resistance
    Ra = 1000000.0f;  //Cds cell dark resistance.
    Ra = logf(Ra);		//this is done for clarity
    Rb = 300.0f;         //Cds cell full illumination
    Rp = 100000.0f;      //Resistor in parallel with Cds cell
    b = exp(Ra/logf(Rb)) - CNST_E;
    dTC = 0.03f;
    dRCl = dTC;
    dRCr = dTC;   //Right & left channel dynamic time contsants
    minTC = logf(0.005f/dTC);
    alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
    alphar = alphal;
    lstep = 0.0f;
    rstep = 0.0f;
    Pdepth = 127;
    Ppanning = 64;
    lpanning = 1.0f;
    rpanning = 1.0f;
    fdepth = 1.0f;
    Pinvert = 0;
    oldgl = 0.0f;
    oldgr = 0.0f;
    gl = 0.0f;
    gr = 0.0f;
    cperiod = 1.0f/fPERIOD;

}

Opticaltrem::~Opticaltrem ()
{
}


void
Opticaltrem::cleanup ()
{


};

void
Opticaltrem::out (DspFloatType *smpsl, DspFloatType *smpsr)
{

    int i;
    DspFloatType lfol, lfor, xl, xr, fxl, fxr;
    DspFloatType rdiff, ldiff;
    lfo.effectlfoout (&lfol, &lfor);

    if(Pinvert) {
    lfol = lfol*fdepth;
    lfor = lfor*fdepth;
    } else {
    lfor = 1.0f - lfor*fdepth;
    lfol = 1.0f - lfol*fdepth;
    }

    if (lfol > 1.0f)
        lfol = 1.0f;
    else if (lfol < 0.0f)
        lfol = 0.0f;
    if (lfor > 1.0f)
        lfor = 1.0f;
    else if (lfor < 0.0f)
        lfor = 0.0f;

    lfor = powf(lfor, 1.9f);
    lfol = powf(lfol, 1.9f);  //emulate lamp turn on/off characteristic

    //lfo interpolation
    rdiff = (lfor - oldgr)*cperiod;
    ldiff = (lfol - oldgl)*cperiod;
    gr = lfor;
    gl = lfol;
    oldgr = lfor;
    oldgl = lfol;

    for (i = 0; i < PERIOD; i++) {
        //Left Cds
        stepl = gl*(1.0f - alphal) + alphal*oldstepl;
        oldstepl = stepl;
        dRCl = dTC*f_exp(stepl*minTC);
        alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
        xl = CNST_E + stepl*b;
        fxl = f_exp(Ra/logf(xl));
        if(Pinvert) {
        fxl = fxl*Rp/(fxl + Rp); //Parallel resistance
        fxl = fxl/(fxl + R1);
        }
        else fxl = R1/(fxl + R1);
        
        //Right Cds
        stepr = gr*(1.0f - alphar) + alphar*oldstepr;
        oldstepr = stepr;
        dRCr = dTC*f_exp(stepr*minTC);
        alphar = 1.0f - cSAMPLE_RATE/(dRCr + cSAMPLE_RATE);
        xr = CNST_E + stepr*b;
        fxr = f_exp(Ra/logf(xr));
        if(Pinvert) {
        fxr = fxr*Rp/(fxr + Rp); //Parallel resistance
        fxr = fxr/(fxr + R1);
        } 
        else fxr = R1/(fxr + R1);

        //Modulate input signal
        efxoutl[i] = lpanning*fxl*smpsl[i];
        efxoutr[i] = rpanning*fxr*smpsr[i];

        gl += ldiff;
        gr += rdiff;  //linear interpolation of LFO

    };


};

void
Opticaltrem::setpanning (int value)
{
    Ppanning = value;
    rpanning = ((DspFloatType)Ppanning) / 64.0f;
    lpanning = 2.0f - rpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
    rpanning = 1.0f - 1.0f/(rpanning + 1.0f);
    
    if(Pinvert) {
    rpanning *= 2.0f;
    lpanning *= 2.0f;
    } else {
    rpanning *= 1.3f;
    lpanning *= 1.3f;
    }

};

void
Opticaltrem::setpreset (int npreset)
{
    const int PRESET_SIZE = 7;
    const int NUM_PRESETS = 6;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Fast
        {127, 260, 10, 0, 64, 64, 0},
        //trem2
        {45, 140, 10, 0, 64, 64, 0},
        //hard pan
        {127, 120, 10, 5, 0, 64, 0},
        //soft pan
        {45, 240, 10, 1, 16, 64, 0},
        //ramp down
        {65, 200, 0, 3, 32, 64, 0},
        //hard ramp
        {127, 480, 0, 3, 32, 64, 0}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(44,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

};

void
Opticaltrem::changepar (int npar, int value)
{

    switch (npar) {

    case 0:
        Pdepth = value;
        fdepth = 0.5f + ((DspFloatType) Pdepth)/254.0f;
        break;
    case 1:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 2:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 5: // pan
        Ppanning = value;
        setpanning(value);
        break;
    case 6: //Invert
        Pinvert = value;
        if(Pinvert) {
          R1 = 68000.0f;   //tremolo circuit series resistance
          Ra = 500000.0f;  //Cds cell dark resistance.
        } else {
          R1 = 2700.0f;	   //tremolo circuit series resistance
          Ra = 1000000.0f;  //Cds cell dark resistance.
        }
        setpanning(Ppanning);
 
        Ra = logf(Ra);		//this is done for clarity
        Rb = 300.0f;         //Cds cell full illumination
        b = exp(Ra/logf(Rb)) - CNST_E;
           break;
   }

};

int
Opticaltrem::getpar (int npar)
{

    switch (npar)

    {
    case 0:
        return (Pdepth);
        break;
    case 1:
        return (lfo.Pfreq);
        break;
    case 2:
        return (lfo.Prandomness);
        break;
    case 3:
        return (lfo.PLFOtype);
        break;
    case 4:
        return (lfo.Pstereo);
        break;
    case 5:
        return (Ppanning); //pan
        break;
    case 6:
        return (Pinvert); //pan
        break;

    }

    return (0);

};


Pan::Pan (DspFloatType *efxoutl_, DspFloatType *efxoutr_)
{

    efxoutl = efxoutl_;
    efxoutr = efxoutr_;


    Ppreset = 0;
    setpreset (Ppreset);

    lfo.effectlfoout (&lfol, &lfor);

    cleanup ();


};



Pan::~Pan ()
{
};

void
Pan::cleanup ()
{
};




void
Pan::out (DspFloatType *smpsl, DspFloatType *smpsr)
{

    int i;
    DspFloatType avg, ldiff, rdiff, tmp;
    DspFloatType pp;
    DspFloatType coeff_PERIOD = 1.0 / fPERIOD;
    DspFloatType fi,P_i;



    if (PextraON) {

        for (i = 0; i < PERIOD; i++)

        {

            avg = (smpsl[i] + smpsr[i]) * .5f;

            ldiff = smpsl[i] - avg;
            rdiff = smpsr[i] - avg;

            tmp = avg + ldiff * mul;
            smpsl[i] = tmp*cdvalue;

            tmp = avg + rdiff * mul;
            smpsr[i] = tmp*sdvalue;


        }

    }

    if (PAutoPan) {

        ll = lfol;
        lr = lfor;
        lfo.effectlfoout (&lfol, &lfor);
        for (i = 0; i < PERIOD; i++) {
            fi = (DspFloatType) i;
            P_i = (DspFloatType) (PERIOD - i);

            pp = (ll * P_i + lfol * fi) * coeff_PERIOD;

            smpsl[i] *= pp * panning;

            pp =  (lr * P_i + lfor * fi) * coeff_PERIOD;

            smpsr[i] *= pp * (1.0f - panning);

        }

    }







};



void
Pan::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};



void
Pan::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning)/ 127.0f;
    dvalue= panning*M_PI_2;
    cdvalue=cosf(dvalue);
    sdvalue=sinf(dvalue);


};



void
Pan::setextra (int Pextra)
{
    this->Pextra = Pextra;
    mul = 4.0f * (DspFloatType)Pextra / 127.0f;
};


void
Pan::setpreset (int npreset)
{
    const int PRESET_SIZE = 9;
    const int NUM_PRESETS = 2;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //AutoPan
        {64, 64, 26, 0, 0, 0, 0, 1, 0},
        //Extra Stereo
        {64, 64, 80, 0, 0, 0, 10, 0, 1}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(13,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;


};



void
Pan::changepar (int npar, int value)
{

    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setextra (value);
        break;
    case 7:
        PAutoPan = value;
        break;
    case 8:
        PextraON = value;
        break;

    }


};


int
Pan::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pextra);
        break;
    case 7:
        return (PAutoPan);
        break;
    case 8:
        return (PextraON);
        break;
    default:
        return (0);

    }

};



#define PHASER_LFO_SHAPE 2

Phaser::Phaser (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    oldl = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES * 2);
    oldr = (DspFloatType *) malloc(sizeof(DspFloatType)* MAX_PHASER_STAGES * 2);

    Ppreset = 0;
    setpreset (Ppreset);
    cleanup ();
};

Phaser::~Phaser ()
{
};


/*
 * Effect output
 */
void
Phaser::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j;
    DspFloatType lfol, lfor, lgain, rgain, tmp;

    lfo.effectlfoout (&lfol, &lfor);
    lgain = lfol;
    rgain = lfor;
    lgain =
        (expf (lgain * PHASER_LFO_SHAPE) - 1.0f) / (expf (PHASER_LFO_SHAPE) - 1.0f);
    rgain =
        (expf (rgain * PHASER_LFO_SHAPE) - 1.0f) / (expf (PHASER_LFO_SHAPE) - 1.0f);


    lgain = 1.0f - phase * (1.0f - depth) - (1.0f - phase) * lgain * depth;
    rgain = 1.0f - phase * (1.0f - depth) - (1.0f - phase) * rgain * depth;

    if (lgain > 1.0)
        lgain = 1.0f;
    else if (lgain < 0.0)
        lgain = 0.0f;
    if (rgain > 1.0)
        rgain = 1.0f;
    else if (rgain < 0.0)
        rgain = 0.0f;

    for (i = 0; i < PERIOD; i++) {
        DspFloatType x = (DspFloatType) i / fPERIOD;
        DspFloatType x1 = 1.0f - x;
        DspFloatType gl = lgain * x + oldlgain * x1;
        DspFloatType gr = rgain * x + oldrgain * x1;
        DspFloatType inl = smpsl[i] * panning + fbl;
        DspFloatType inr = smpsr[i] * (1.0f - panning) + fbr;

        //Left channel
        for (j = 0; j < Pstages * 2; j++) {
            //Phasing routine
            tmp = oldl[j] + DENORMAL_GUARD;
            oldl[j] = gl * tmp + inl;
            inl = tmp - gl * oldl[j];
        };
        //Right channel
        for (j = 0; j < Pstages * 2; j++) {
            //Phasing routine
            tmp = oldr[j] + DENORMAL_GUARD;
            oldr[j] = (gr * tmp) + inr;
            inr = tmp - (gr * oldr[j]);
        };
        //Left/Right crossing
        DspFloatType l = inl;
        DspFloatType r = inr;
        inl = l * (1.0f - lrcross) + r * lrcross;
        inr = r * (1.0f - lrcross) + l * lrcross;

        fbl = inl * fb;
        fbr = inr * fb;
        efxoutl[i] = inl;
        efxoutr[i] = inr;

    };

    oldlgain = lgain;
    oldrgain = rgain;

    if (Poutsub != 0)
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] *= -1.0f;
            efxoutr[i] *= -1.0f;
        };

};

/*
 * Cleanup the effect
 */
void
Phaser::cleanup ()
{
    fbl = 0.0;
    fbr = 0.0;
    oldlgain = 0.0;
    oldrgain = 0.0;
    for (int i = 0; i < Pstages * 2; i++) {
        oldl[i] = 0.0;
        oldr[i] = 0.0;
    };
};

/*
 * Parameter control
 */
void
Phaser::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = ((DspFloatType)Pdepth / 127.0f);
};


void
Phaser::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = ((DspFloatType)Pfb - 64.0f) / 64.1f;
};

void
Phaser::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
Phaser::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + .5f)/ 127.0f;
};

void
Phaser::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f;
};

void
Phaser::setstages (int Pstages)
{
    if (Pstages >= MAX_PHASER_STAGES)
        Pstages = MAX_PHASER_STAGES - 1;
    this->Pstages = Pstages;
    cleanup ();
};

void
Phaser::setphase (int Pphase)
{
    this->Pphase = Pphase;
    phase = ((DspFloatType)Pphase / 127.0f);
};


void
Phaser::setpreset (int npreset)
{
    const int PRESET_SIZE = 12;
    const int NUM_PRESETS = 6;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Phaser1
        {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20},
        //Phaser2
        {64, 64, 10, 0, 0, 88, 40, 64, 3, 0, 0, 20},
        //Phaser3
        {64, 64, 8, 0, 0, 66, 68, 107, 2, 0, 0, 20},
        //Phaser4
        {39, 64, 1, 0, 0, 66, 67, 10, 5, 0, 1, 20},
        //Phaser5
        {64, 64, 1, 0, 1, 110, 67, 78, 10, 0, 0, 20},
        //Phaser6
        {64, 64, 31, 100, 0, 58, 37, 78, 3, 0, 0, 20}
    };

    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(6,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Phaser::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setdepth (value);
        break;
    case 7:
        setfb (value);
        break;
    case 8:
        setstages (value);
        break;
    case 9:
        setlrcross (value);
        break;
    case 10:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case 11:
        setphase (value);
        break;
    };
};

int
Phaser::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pdepth);
        break;
    case 7:
        return (Pfb);
        break;
    case 8:
        return (Pstages);
        break;
    case 9:
        return (Plrcross);
        break;
    case 10:
        return (Poutsub);
        break;
    case 11:
        return (Pphase);
        break;
    default:
        return (0);
    };

};


RBEcho::RBEcho (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Pdelay = 60;
    Plrdelay = 100;
    Plrcross = 100;
    Pfb = 40;
    Phidamp = 60;
    Psubdiv = 1;
    subdiv = 1.0f;
    reverse = ireverse = 0.0f;
    pingpong = 0.0f;
    ipingpong = 1.0f;

    lrdelay = 0;
    Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
    maxx_delay = 1 + SAMPLE_RATE * MAX_DELAY;

    ldelay = new delayline(2.0f, 3);
    rdelay = new delayline(2.0f, 3);

    setpreset (Ppreset);
    cleanup ();
};

RBEcho::~RBEcho ()
{
};

/*
 * Cleanup the effect
 */
void
RBEcho::cleanup ()
{
    ldelay->cleanup();
    rdelay->cleanup();
    ldelay->set_averaging(0.25f);
    rdelay->set_averaging(0.25f);
    oldl = 0.0;
    oldr = 0.0;
};


/*
 * Initialize the delays
 */
void
RBEcho::initdelays ()
{
    oldl = 0.0;
    oldr = 0.0;

    if(Plrdelay>0) {
        ltime = delay + lrdelay;
        rtime = delay - lrdelay;
    } else {
        ltime = delay - lrdelay;
        rtime = delay + lrdelay;
    }

    if(ltime > 2.0f) ltime = 2.0f;
    if(ltime<0.01f) ltime = 0.01f;

    if(rtime > 2.0f) rtime = 2.0f;
    if(rtime<0.01f) rtime = 0.01f;


};

/*
 * Effect output
 */
void
RBEcho::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType ldl, rdl;
    DspFloatType avg, ldiff, rdiff, tmp;


    for (i = 0; i < PERIOD; i++) {

        //LowPass Filter
        ldl = lfeedback * hidamp + oldl * (1.0f - hidamp);
        rdl = rfeedback * hidamp + oldr * (1.0f - hidamp);
        oldl = ldl + DENORMAL_GUARD;
        oldr = rdl + DENORMAL_GUARD;

        ldl = ldelay->delay_simple((ldl + smpsl[i]), delay, 0, 1, 0);
        rdl = rdelay->delay_simple((rdl + smpsr[i]), delay, 0, 1, 0);


        if(Preverse) {
            rvl = ldelay->delay_simple(oldl, delay, 1, 0, 1)*ldelay->envelope();
            rvr = rdelay->delay_simple(oldr, delay, 1, 0, 1)*rdelay->envelope();
            ldl = ireverse*ldl + reverse*rvl;
            rdl = ireverse*rdl + reverse*rvr;

        }


        lfeedback = lpanning * fb * ldl;
        rfeedback = rpanning * fb * rdl;

        if(Pes) {
            ldl *= cosf(lrcross);
            rdl *= sinf(lrcross);

            avg = (ldl + rdl) * 0.5f;
            ldiff = ldl - avg;
            rdiff = rdl - avg;

            tmp = avg + ldiff * pes;
            ldl = 0.5 * tmp;

            tmp = avg + rdiff * pes;
            rdl = 0.5f * tmp;


        }
        efxoutl[i] = (ipingpong*ldl + pingpong *ldelay->delay_simple(0.0f, ltime, 2, 0, 0)) * lpanning;
        efxoutr[i] = (ipingpong*rdl + pingpong *rdelay->delay_simple(0.0f, rtime, 2, 0, 0)) * rpanning;

    };

};


/*
 * Parameter control
 */
void
RBEcho::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;

};

void
RBEcho::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    lpanning = ((DspFloatType)Ppanning) / 64.0f;
    rpanning = 2.0f - lpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
    rpanning = 1.0f - 1.0f/(rpanning + 1.0f);
    lpanning *= 1.1f;
    rpanning *= 1.1f;
};

void
RBEcho::setreverse (int Preverse)
{
    this->Preverse = Preverse;
    reverse = (DspFloatType) Preverse / 127.0f;
    ireverse = 1.0f - reverse;
};

void
RBEcho::setdelay (int Pdelay)
{
    this->Pdelay = Pdelay;
    fdelay= 60.0f/((DspFloatType) Pdelay);
    if (fdelay < 0.01f) fdelay = 0.01f;
    if (fdelay > (DspFloatType) MAX_DELAY) fdelay = (DspFloatType) MAX_DELAY;  //Constrains 10ms ... MAX_DELAY
    delay = subdiv * fdelay;
    initdelays ();
};

void
RBEcho::setlrdelay (int Plrdelay)
{
    DspFloatType tmp;
    this->Plrdelay = Plrdelay;
    lrdelay = delay * fabs(((DspFloatType)Plrdelay - 64.0f) / 65.0f);

    tmp = fabs( ((DspFloatType) Plrdelay - 64.0f)/32.0f);
    pingpong = 1.0f - 1.0f/(5.0f*tmp*tmp + 1.0f);
    pingpong *= 1.05159f;
    ipingpong = 1.0f - pingpong;
    initdelays ();
};

void
RBEcho::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = D_PI * (DspFloatType)Plrcross / 128.0f;

};

void
RBEcho::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = (DspFloatType)Pfb / 128.0f;
};

void
RBEcho::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = f_exp(-D_PI * 500.0f * ((DspFloatType) Phidamp)/fSAMPLE_RATE);
};

void
RBEcho::setpreset (int npreset)
{
    const int PRESET_SIZE = 10;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {64, 64, 90, 64, 64, 64, 64, 0, 1, 96},
        //Echo 2
        {64, 64, 90, 64, 64, 64, 64, 0, 2 ,96},
        //Echo 3
        {64, 64, 90, 64, 64, 64, 64, 0, 3 ,96}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(32,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
RBEcho::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setdelay (value);
        break;
    case 3:
        setlrdelay (value);
        break;
    case 4:
        setlrcross (value);
        break;
    case 5:
        setfb (value);
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        setreverse (value);
        break;
    case 8:
        Psubdiv = value;
        subdiv = 1.0f/((DspFloatType)(value + 1));
        delay = subdiv * fdelay;
        initdelays ();
        break;
    case 9:
        Pes = value;
        pes = 8.0f * (DspFloatType)Pes / 127.0f;
        break;
    };
};

int
RBEcho::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Pdelay);
        break;
    case 3:
        return (Plrdelay);
        break;
    case 4:
        return (Plrcross);
        break;
    case 5:
        return (Pfb);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return (Preverse);
        break;
    case 8:
        return (Psubdiv);
        break;
    case 9:
        return (Pes);
        break;

    };
    return (0);			//in case of bogus parameter number
};


RecChord::RecChord ()
{

    memset (Ch, 0, sizeof (Ch));


    Ch[0][0] = 1;
    Ch[6][0] = 1;
    Ch[12][0] = 1;
    Ch[18][0] = 1;
    Ch[24][0] = 1;
    Ch[31][0] = 1;
    Ch[0][2] = 2;
    Ch[6][2] = 1;
    Ch[12][2] = 1;
    Ch[18][2] = 2;
    Ch[24][2] = 2;
    Ch[31][7] = 1;
    Ch[0][4] = 1;
    Ch[6][4] = 1;
    Ch[12][3] = 1;
    Ch[18][3] = 1;
    Ch[24][4] = 1;
    Ch[0][7] = 1;
    Ch[6][7] = 1;
    Ch[12][5] = 2;
    Ch[18][5] = 2;
    Ch[24][7] = 1;
    Ch[0][9] = 2;
    Ch[6][9] = 1;
    Ch[12][7] = 1;
    Ch[18][6] = 1;
    Ch[24][9] = 1;
    Ch[30][0] = 1;
    Ch[6][11] = 2;
    Ch[18][8] = 2;
    Ch[24][10] = 1;
    Ch[18][9] = 1;
    Ch[18][11] = 2;
    Ch[1][0] = 1;
    Ch[7][0] = 1;
    Ch[13][0] = 1;
    Ch[19][0] = 1;
    Ch[25][0] = 1;
    Ch[32][0] = 1;
    Ch[1][2] = 2;
    Ch[7][2] = 2;
    Ch[13][2] = 1;
    Ch[19][4] = 1;
    Ch[25][1] = 1;
    Ch[32][2] = 2;
    Ch[1][4] = 1;
    Ch[7][4] = 1;
    Ch[13][3] = 1;
    Ch[19][7] = 1;
    Ch[25][4] = 1;
    Ch[32][5] = 1;
    Ch[1][7] = 1;
    Ch[7][6] = 2;
    Ch[13][5] = 2;
    Ch[19][10] = 1;
    Ch[25][7] = 1;
    Ch[32][7] = 1;
    Ch[1][9] = 1;
    Ch[7][8] = 1;
    Ch[13][7] = 1;
    Ch[25][10] = 1;
    Ch[1][11] = 2;
    Ch[7][10] = 2;
    Ch[13][10] = 1;


    Ch[2][0] = 1;
    Ch[8][0] = 1;
    Ch[14][0] = 1;
    Ch[20][0] = 1;
    Ch[26][0] = 1;
    Ch[33][0] = 1;
    Ch[2][2] = 2;
    Ch[8][2] = 2;
    Ch[14][2] = 2;
    Ch[20][5] = 1;
    Ch[26][1] = 2;
    Ch[33][2] = 1;
    Ch[2][4] = 1;
    Ch[8][3] = 1;
    Ch[14][3] = 1;
    Ch[20][7] = 1;
    Ch[26][4] = 1;
    Ch[33][4] = 2;
    Ch[2][7] = 1;
    Ch[8][5] = 2;
    Ch[14][5] = 1;
    Ch[20][10] = 1;
    Ch[26][5] = 2;
    Ch[33][7] = 1;
    Ch[2][9] = 2;
    Ch[8][7] = 1;
    Ch[14][7] = 1;
    Ch[26][7] = 1;
    Ch[33][9] = 2;
    Ch[2][11] = 1;
    Ch[14][10] = 1;
    Ch[26][8] = 1;
    Ch[26][10] = 1;
    Ch[3][0] = 1;
    Ch[9][0] = 1;
    Ch[15][0] = 1;
    Ch[21][0] = 1;
    Ch[27][0] = 1;
    Ch[3][2] = 2;
    Ch[9][2] = 2;
    Ch[15][2] = 2;
    Ch[21][2] = 2;
    Ch[27][1] = 2;
    Ch[3][4] = 1;
    Ch[9][3] = 1;
    Ch[15][3] = 1;
    Ch[21][4] = 1;
    Ch[27][3] = 1;
    Ch[3][6] = 1;
    Ch[9][5] = 2;
    Ch[15][5] = 2;
    Ch[21][6] = 1;
    Ch[27][4] = 1;
    Ch[3][7] = 1;
    Ch[9][7] = 1;
    Ch[15][7] = 1;
    Ch[21][10] = 1;
    Ch[27][6] = 2;
    Ch[3][9] = 2;
    Ch[9][9] = 1;
    Ch[15][11] = 1;
    Ch[27][7] = 1;
    Ch[3][11] = 1;
    Ch[27][9] = 2;
    Ch[27][10] = 1;
    Ch[4][0] = 1;
    Ch[10][0] = 1;
    Ch[16][0] = 1;
    Ch[22][0] = 1;
    Ch[28][0] = 1;
    Ch[4][2] = 1;
    Ch[10][3] = 1;
    Ch[16][2] = 1;
    Ch[22][2] = 1;
    Ch[28][2] = 2;
    Ch[4][4] = 1;
    Ch[10][5] = 2;
    Ch[16][3] = 1;
    Ch[22][4] = 1;
    Ch[28][4] = 1;
    Ch[4][7] = 1;
    Ch[10][7] = 1;
    Ch[16][5] = 2;
    Ch[22][7] = 1;
    Ch[28][5] = 2;
    Ch[4][9] = 2;
    Ch[10][10] = 1;
    Ch[16][7] = 1;
    Ch[22][9] = 2;
    Ch[28][8] = 1;
    Ch[16][11] = 1;
    Ch[22][10] = 1;
    Ch[28][9] = 2;
    Ch[28][11] = 1;
    Ch[5][0] = 1;
    Ch[11][0] = 1;
    Ch[17][0] = 1;
    Ch[23][0] = 1;
    Ch[29][0] = 1;
    Ch[5][2] = 1;
    Ch[11][1] = 2;
    Ch[17][3] = 1;
    Ch[23][2] = 2;
    Ch[29][2] = 2;
    Ch[5][4] = 1;
    Ch[11][3] = 1;
    Ch[17][5] = 2;
    Ch[23][4] = 1;
    Ch[29][4] = 1;
    Ch[5][7] = 1;
    Ch[11][5] = 2;
    Ch[17][6] = 1;
    Ch[23][6] = 1;
    Ch[29][6] = 2;
    Ch[5][9] = 2;
    Ch[11][6] = 1;
    Ch[17][8] = 2;
    Ch[23][7] = 1;
    Ch[29][8] = 1;
    Ch[5][11] = 1;
    Ch[11][8] = 2;
    Ch[23][9] = 2;
    Ch[29][10] = 1;
    Ch[11][10] = 1;
    Ch[23][10] = 1;



    memset (ChN, 0, sizeof (ChN));

    {
        static const char *stnom[] = {
            "", "6", "Maj7", "lyd", "Maj(9)", "Maj7(9)", "6/9", "+", "m",
            "m6", "m7", "m7(b5)", "m9", "m7(9)", "m7(11)", "m(Maj7)",
            "m(Maj7)(9)", "dim", "dim7", "7", "7(Sus4)", "7(b5)", "7(9)",
            "7(#11)", "7(13)", "7(b9)", "7(b13)", "7(#9)", "+Maj7", "+7",
            "1+8", "1+5", "(Sus4)", "2"
        };
        for (int i = 0; i <= 33; i++) {
            strcpy (ChN[i].Nom, stnom[i]);
            ChN[i].tipo = i;
        }
    }

    ChN[1].num = 1;
    ChN[6].num = 1;
    ChN[9].num = 1;
    ChN[32].num = 1;


    {
        static const char *stnom[] = {
            "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb",
            "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };

        for (int i = 0; i <= 11; i++)
            strcpy (NC[i].Nom, stnom[i]);
        for (int i = 0; i <= 23; i++)
            strcpy (NCE[i].Nom, stnom[i]);
    }
    NCE[0].note = 0;
    NCE[1].note = 1;
    NCE[2].note = 2;
    NCE[3].note = 3;
    NCE[4].note = 4;
    NCE[5].note = 5;
    NCE[6].note = 6;
    NCE[7].note = -5;
    NCE[8].note = -4;
    NCE[9].note = -3;
    NCE[10].note = -2;
    NCE[11].note = -1;
    NCE[12].note = 0;
    NCE[13].note = 1;
    NCE[14].note = 2;
    NCE[15].note = 3;
    NCE[16].note = 4;
    NCE[17].note = 5;
    NCE[18].note = 6;
    NCE[19].note = -5;
    NCE[20].note = -4;
    NCE[21].note = -3;
    NCE[22].note = -2;
    NCE[23].note = -1;


    IniciaChords ();
    memset (NombreAcorde, 0, sizeof (NombreAcorde));
}


RecChord::~RecChord()
{
}


void
RecChord::cleanup ()
{
    int i;
    memset (NombreAcorde, 0, sizeof (NombreAcorde));
    for (i = 0; i < POLY; i++) {
        note_active[i] = 0;
        rnote[i] = 0;
        gate[i] = 0;
    }

    cc = 1;

}

void
RecChord::IniciaChords ()
{
    int i, j;
    int notas = 1;
    int numno[6];

    NumChord3 = 0;
    NumChord4 = 0;
    NumChord5 = 0;

    memset (Chord3, 0, sizeof (Chord3));
    memset (Chord4, 0, sizeof (Chord4));
    memset (Chord5, 0, sizeof (Chord5));

    for (i = 0; i <= 33; i++) {
        notas = 1;
        memset (numno, 0, sizeof (numno));

        for (j = 1; j <= 11; j++) {
            if (Ch[i][j] == 1) {
                notas++;
                numno[notas] = j;
            }
        }



        switch (notas) {
        case 3:
            strcpy (Chord3[NumChord3].Nom, ChN[i].Nom);
            Chord3[NumChord3].di1 = numno[2];
            Chord3[NumChord3].di2 = numno[3] - numno[2];
            Chord3[NumChord3].fund = 1;
            Chord3[NumChord3].tipo = i;
            NumChord3++;
            if (ChN[i].num == 1)
                break;
            strcpy (Chord3[NumChord3].Nom, ChN[i].Nom);
            Chord3[NumChord3].di1 = numno[3] - numno[2];
            Chord3[NumChord3].di2 = 12 - numno[3];
            Chord3[NumChord3].fund = 3;
            Chord3[NumChord3].tipo = i;
            NumChord3++;
            strcpy (Chord3[NumChord3].Nom, ChN[i].Nom);
            Chord3[NumChord3].di1 = 12 - numno[3];
            Chord3[NumChord3].di2 = numno[2];
            Chord3[NumChord3].fund = 2;
            Chord3[NumChord3].tipo = i;
            NumChord3++;
            break;
        case 4:
            strcpy (Chord4[NumChord4].Nom, ChN[i].Nom);
            Chord4[NumChord4].di1 = numno[2];
            Chord4[NumChord4].di2 = numno[3] - numno[2];
            Chord4[NumChord4].di3 = numno[4] - numno[3];
            Chord4[NumChord4].fund = 1;
            Chord4[NumChord4].tipo = i;
            NumChord4++;
            if (ChN[i].num == 1)
                break;
            strcpy (Chord4[NumChord4].Nom, ChN[i].Nom);
            Chord4[NumChord4].di1 = numno[3] - numno[2];
            Chord4[NumChord4].di2 = numno[4] - numno[3];
            Chord4[NumChord4].di3 = 12 - numno[4];
            Chord4[NumChord4].fund = 4;
            Chord4[NumChord4].tipo = i;
            NumChord4++;
            strcpy (Chord4[NumChord4].Nom, ChN[i].Nom);
            Chord4[NumChord4].di1 = numno[4] - numno[3];
            Chord4[NumChord4].di2 = 12 - numno[4];
            Chord4[NumChord4].di3 = numno[2];
            Chord4[NumChord4].fund = 3;
            Chord4[NumChord4].tipo = i;
            NumChord4++;
            strcpy (Chord4[NumChord4].Nom, ChN[i].Nom);
            Chord4[NumChord4].di1 = 12 - numno[4];
            Chord4[NumChord4].di2 = numno[2];
            Chord4[NumChord4].di3 = numno[3] - numno[2];
            Chord4[NumChord4].fund = 2;
            Chord4[NumChord4].tipo = i;
            NumChord4++;
            strcpy (Chord4[NumChord4].Nom, ChN[i].Nom);
            Chord4[NumChord4].di1 = numno[2] + (12 - numno[4]);
            Chord4[NumChord4].di2 = numno[3] - numno[2];
            Chord4[NumChord4].di3 = 12 - numno[3];
            Chord4[NumChord4].fund = 4;
            Chord4[NumChord4].tipo = i;
            NumChord4++;
            break;

        case 5:
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = numno[2];
            Chord5[NumChord5].di2 = numno[3] - numno[2];
            Chord5[NumChord5].di3 = numno[4] - numno[3];
            Chord5[NumChord5].di4 = numno[5] - numno[4];
            Chord5[NumChord5].fund = 1;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            if (ChN[i].num == 1)
                break;
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = numno[3] - numno[2];
            Chord5[NumChord5].di2 = numno[4] - numno[3];
            Chord5[NumChord5].di3 = numno[5] - numno[4];
            Chord5[NumChord5].di4 = 12 - numno[5];

            Chord5[NumChord5].fund = 5;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = numno[4] - numno[3];
            Chord5[NumChord5].di2 = numno[5] - numno[4];
            Chord5[NumChord5].di3 = 12 - numno[5];
            Chord5[NumChord5].di4 = numno[2];
            Chord5[NumChord5].fund = 4;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = numno[5] - numno[4];
            Chord5[NumChord5].di2 = 12 - numno[5];
            Chord5[NumChord5].di3 = numno[2];
            Chord5[NumChord5].di4 = numno[3] - numno[2];
            Chord5[NumChord5].fund = 3;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = 12 - numno[5];
            Chord5[NumChord5].di2 = numno[2];
            Chord5[NumChord5].di3 = numno[3] - numno[2];
            Chord5[NumChord5].di4 = numno[4] - numno[3];
            Chord5[NumChord5].fund = 2;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            strcpy (Chord5[NumChord5].Nom, ChN[i].Nom);
            Chord5[NumChord5].di1 = numno[2] + (12 - numno[5]);
            Chord5[NumChord5].di2 = numno[3] - numno[2];
            Chord5[NumChord5].di3 = numno[4] - numno[3];
            Chord5[NumChord5].di4 = 12 - numno[4];
            Chord5[NumChord5].fund = 5;
            Chord5[NumChord5].tipo = i;
            NumChord5++;
            break;

        }

    }



};



void
RecChord::MiraChord ()
{

    int i, j;
    int anote[POLY];
    int nnotes = 0;
    int temp;
    int di1, di2, di3, di4;
    int labaja;
    char AName[20];


    for (i = 0; i < POLY; i++) {
        if (note_active[i]) {
            anote[nnotes] = rnote[i];
            nnotes++;
        }
    }



    if ((nnotes < 3) || (nnotes > 5))
        return;

    labaja = anote[0];
    for (i = 1; i < nnotes - 1; i++)
        if (anote[i] < labaja)
            labaja = anote[i];
    for (i = 0; i < nnotes; i++)
        if (anote[i] - labaja > 12)
            anote[i] -= 12;
    for (i = 1; i < nnotes; i++) {
        for (j = 0; j < nnotes - 1; j++) {

            if (anote[j] > anote[j + 1]) {
                temp = anote[j];
                anote[j] = anote[j + 1];
                anote[j + 1] = temp;
            }
        }
    }



    bass = anote[0] % 12;

sigue:

    if (nnotes == 3) {

        di1 = anote[1] - anote[0];
        di2 = anote[2] - anote[1];

        j = -1;

        while (j <= NumChord3)

        {
            j++;
            if ((Chord3[j].di1 == di1) && (Chord3[j].di2 == di2)) {
                ctipo = Chord3[j].tipo;
                int elke = anote[Chord3[j].fund - 1];
                fundi = elke % 12;
                sprintf (AName, "%s%s", NC[fundi].Nom, Chord3[j].Nom);

                if (bass != fundi) {

                    memset (AName, 0, sizeof (AName));
                    sprintf (AName, "%s%s/%s", NC[fundi].Nom, Chord3[j].Nom,
                             NCE[bass + plus (fundi)].Nom);
                }

                if (strcmp (AName, NombreAcorde) != 0) {
                    strcpy (NombreAcorde, AName);
                    cc = 1;
                }
                return;
            }
        }

    }

    if (nnotes == 4) {

        di1 = anote[1] - anote[0];
        di2 = anote[2] - anote[1];
        di3 = anote[3] - anote[2];
        j = -1;

        while (j <= NumChord4)

        {
            j++;
            if ((Chord4[j].di1 == di1) && (Chord4[j].di2 == di2)
                    && (Chord4[j].di3 == di3)) {
                ctipo = Chord4[j].tipo;
                int elke = anote[Chord4[j].fund - 1];
                fundi = elke % 12;
                sprintf (AName, "%s%s", NC[fundi].Nom, Chord4[j].Nom);
                if (bass != fundi) {
                    memset (AName, 0, sizeof (AName));
                    sprintf (AName, "%s%s/%s", NC[fundi].Nom, Chord4[j].Nom,
                             NCE[bass + plus (fundi)].Nom);
                }

                if (strcmp (AName, NombreAcorde) != 0) {
                    strcpy (NombreAcorde, AName);
                    cc = 1;
                }
                return;
            }

        }



    }

    if (nnotes == 5) {

        di1 = anote[1] - anote[0];
        di2 = anote[2] - anote[1];
        di3 = anote[3] - anote[2];
        di4 = anote[4] - anote[3];
        j = -1;

        while (j < NumChord5)

        {
            j++;
            if ((Chord5[j].di1 == di1) && (Chord5[j].di2 == di2)
                    && (Chord5[j].di3 == di3) && (Chord5[j].di4 == di4)) {
                ctipo = Chord5[j].tipo;
                int elke = anote[Chord5[j].fund - 1];
                fundi = elke % 12;
                sprintf (AName, "%s%s", NC[fundi].Nom, Chord5[j].Nom);
                if (bass != fundi) {
                    memset (AName, 0, sizeof (AName));
                    sprintf (AName, "%s%s/%s", NC[fundi].Nom, Chord5[j].Nom,
                             NCE[bass + plus (fundi)].Nom);
                }

                if (strcmp (AName, NombreAcorde) != 0) {
                    strcpy (NombreAcorde, AName);
                    cc = 1;
                }
                return;

            }

        }

    }



    if (nnotes == 5) {
        bass = anote[0] % 12;

        for (i = 1; i <= 4; i++)
            anote[i - 1] = anote[i];
        nnotes = 4;
        goto sigue;
    }



    if (nnotes == 5) {
        nnotes = 4;
        goto sigue;
    }

    if (nnotes == 4) {
        nnotes = 3;
        goto sigue;
    }



};




int
RecChord::plus (int nota)
{
    switch (nota) {
    case 2:
        return (12);
        break;
    case 4:
        return (12);
        break;
    case 7:
        return (12);
        break;
    case 9:
        return (12);
        break;
    case 11:
        return (12);
        break;
    default:
        return (0);
    }

};


void
RecChord::Vamos (int voz, int interval)
{

    int n1, h1;
    int hm1, hp1;
    int hm2, hp2;
    int hm3, hp3;
    int ninterval = interval;
    int tengo=0;
    int nota;
    int harmo;
    int typo;

    nota = reconota % 12;

    nota -= fundi;
    if (nota < 0)
        nota += 12;


    harmo = (12 + nota + interval) % 12;
    if (harmo > 12)
        harmo %= 12;



    hm1 = harmo - 1;
    if (hm1 < 0)
        hm1 += 12;
    hp1 = harmo + 1;
    if (hp1 > 12)
        hp1 %= 12;

    hm2 = harmo - 2;
    if (hm2 < 0)
        hm2 += 12;
    hp2 = harmo + 2;
    if (hp2 > 12)
        hp2 %= 12;

    hm3 = harmo - 3;
    if (hm3 < 0)
        hm3 += 12;
    hp3 = harmo + 3;
    if (hp3 > 12)
        hp3 %= 12;



    n1 = Ch[ctipo][nota];
    h1 = Ch[ctipo][harmo];

    typo = 3;

    if ((n1 == 1) && (h1 == 1))
        typo = 0;
    if ((n1 == 2) && (h1 == 2))
        typo = 1;
    if (h1 == 1)
        typo = 2;



    switch (typo)

    {
    case 0:
        ninterval = interval;
        break;
    case 1:
        ninterval = interval;
        break;
    case 2:
        ninterval = interval;
        break;
    case 3:
        tengo = 0;

        if ((Ch[ctipo][hp1] != 0) && (tengo == 0)) {
            ninterval = interval + 1;
            tengo = 1;
        }
        if ((Ch[ctipo][hm1] != 0) && (tengo == 0)) {
            ninterval = interval - 1;
            tengo = 1;
        }

        if ((Ch[ctipo][hp2] != 0) && (tengo == 0)) {
            ninterval = interval + 2;
            tengo = 1;
        }
        if ((Ch[ctipo][hm2] != 0) && (tengo == 0)) {
            ninterval = interval - 2;
            tengo = 1;
        }

        if ((Ch[ctipo][hp3] != 0) && (tengo == 0)) {
            ninterval = interval + 3;
            tengo = 1;
        }
        if ((Ch[ctipo][hm3] != 0) && (tengo == 0)) {
            ninterval = interval - 1;
            tengo = 1;
        }
        break;
    }

    if (ninterval < -12)
        ninterval += 12;
    if (ninterval > 12)
        ninterval %= 12;



    r__ratio[voz] = powf (2, ((DspFloatType)ninterval / 12.0f));

}


Recognize::Recognize (DspFloatType *efxoutl_, DspFloatType *efxoutr_, DspFloatType trig)
{

    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    static const char *englishNotes[12] =
    { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
    notes = englishNotes;
    ultima = -1;
    note = 0;
    nfreq = 0;
    afreq = 0;
    trigfact = trig;

    Sus = new Sustainer(efxoutl,efxoutr);
    Sus->changepar(1,64);
    Sus->changepar(2,127);


    lpfl = new AnalogFilter (2, 3000, 1, 0);
    lpfr = new AnalogFilter (2, 3000, 1, 0);
    hpfl = new AnalogFilter (3, 300, 1, 0);
    hpfr = new AnalogFilter (3, 300, 1, 0);


    schmittInit (24);

}

Recognize::~Recognize ()

{
}


void
Recognize::schmittInit (int size)
{
    blockSize = SAMPLE_RATE / size;
    schmittBuffer =
    (signed short int *) malloc (blockSize * sizeof (signed short int));
    schmittPointer = schmittBuffer;
};


void
Recognize::schmittS16LE (signed short int *indata)
{
    int i, j;

    for (i = 0; i < PERIOD; i++) {
        *schmittPointer++ = indata[i];
        if (schmittPointer - schmittBuffer >= blockSize) {
            int endpoint, startpoint, t1, t2, A1, A2, tc, schmittTriggered;

            schmittPointer = schmittBuffer;

            for (j = 0, A1 = 0, A2 = 0; j < blockSize; j++) {
                if (schmittBuffer[j] > 0 && A1 < schmittBuffer[j])
                    A1 = schmittBuffer[j];
                if (schmittBuffer[j] < 0 && A2 < -schmittBuffer[j])
                    A2 = -schmittBuffer[j];
            }
            t1 = lrintf ((DspFloatType)A1 * trigfact + 0.5f);
            t2 = -lrintf((DspFloatType)A2 * trigfact + 0.5f);
            startpoint = 0;
            for (j = 1; schmittBuffer[j] <= t1 && j < blockSize; j++);
            for (; !(schmittBuffer[j] >= t2 &&
            schmittBuffer[j + 1] < t2) && j < blockSize; j++);
            startpoint = j;
            schmittTriggered = 0;
            endpoint = startpoint + 1;
            for (j = startpoint, tc = 0; j < blockSize; j++) {
                if (!schmittTriggered) {
                    schmittTriggered = (schmittBuffer[j] >= t1);
                } else if (schmittBuffer[j] >= t2 && schmittBuffer[j + 1] < t2) {
                    endpoint = j;
                    tc++;
                    schmittTriggered = 0;
                }
            }
            if (endpoint > startpoint) {
                afreq =
                fSAMPLE_RATE *((DspFloatType)tc / (DspFloatType) (endpoint - startpoint));
                displayFrequency (afreq);

            }
        }
    }
};


void
Recognize::schmittFree ()
{
    free (schmittBuffer);
};


void
Recognize::setlpf (int value)
{
    DspFloatType fr = (DspFloatType)value;
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Recognize::sethpf (int value)
{
    DspFloatType fr = (DspFloatType)value;

    hpfl->setfreq (fr);
    hpfr->setfreq (fr);


}



void
Recognize::schmittFloat (DspFloatType *indatal, DspFloatType *indatar)
{
    int i;
    signed short int buf[PERIOD];

    lpfl->filterout (indatal);
    hpfl->filterout (indatal);
    lpfr->filterout (indatar);
    hpfr->filterout (indatar);

    Sus->out(indatal,indatar);

    for (i = 0; i < PERIOD; i++) {
        buf[i] = (short) ((indatal[i]+indatar[i]) * 32768);
    }
    schmittS16LE (buf);
};



void
Recognize::displayFrequency (DspFloatType freq)
{
    int i;
    int offset=4;
    int noteoff = 0;
    int octave = 4;

    DspFloatType ldf, mldf;
    DspFloatType lfreq;

    if (freq < 1E-15)
        freq = 1E-15f;
    lfreq = logf (freq);
    while (lfreq < lfreqs[0] - LOG_D_NOTE * .5f)
        lfreq += LOG_2;
    while (lfreq >= lfreqs[0] + LOG_2 - LOG_D_NOTE * .5f)
        lfreq -= LOG_2;
    mldf = LOG_D_NOTE;
    for (i = 0; i < 12; i++) {
        ldf = fabsf (lfreq - lfreqs[i]);
        if (ldf < mldf) {
            mldf = ldf;
            note = i;
        }
    }

    nfreq = freqs[note];

    while (nfreq / freq > D_NOTE_SQRT) {
        nfreq *=0.5f;
        octave--;
        if(octave < -1) {
            noteoff = 1;
            break;
        }
    }
    while (freq / nfreq > D_NOTE_SQRT) {
        nfreq *= 2.0f;
        octave++;
        if (octave > 7) {
            noteoff = 1;
            break;
        }
    }



    if (!noteoff) {
//    reconota = 24 + (octave * 12) + note - 3;
        offset = lrintf(nfreq / 20.0);
        if (fabsf(lafreq-freq)>offset) {
            lafreq = nfreq;
            reconota = 24 + (octave * 12) + note - 3;

//    printf("%f\n",lafreq);
        }
    }


};


/*TODO: EarlyReflections,Prdelay,Perbalance */

Reverb::Reverb (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;
    inputbuf = new DspFloatType[PERIOD];
    //filterpars=NULL;


    //defaults
    Ppreset = 0;
    Pvolume = 48;
    Ppan = 64;
    Ptime = 64;
    Pidelay = 40;
    Pidelayfb = 0;
    Prdelay = 0;
    Plpf = 127;
    Phpf = 0;
    Perbalance = 64;
    Plohidamp = 80;
    Ptype = 1;
    Proomsize = 64;
    roomsize = 1.0f;
    rs = 1.0f;
    rs_coeff = rs / (DspFloatType) REV_COMBS;

    for (int i = 0; i < REV_COMBS * 2; i++) {
        comblen[i] = 800 + (int) (RND * 1400);
        combk[i] = 0;
        lpcomb[i] = 0;
        combfb[i] = -0.97f;
        comb[i] = NULL;
    };

    for (int i = 0; i < REV_APS * 2; i++) {
        aplen[i] = 500 + (int) (RND * 500);
        apk[i] = 0;
        ap[i] = NULL;
    };

    lpf =  new AnalogFilter (2, 22000, 1, 0);;
    hpf =  new AnalogFilter (3, 20, 1, 0);
    idelay = NULL;

    setpreset (Ppreset);
    cleanup ();			//do not call this before the comb initialisation
};


Reverb::~Reverb ()
{
};

/*
 * Cleanup the effect
 */
void
Reverb::cleanup ()
{
    int i, j;
    for (i = 0; i < REV_COMBS * 2; i++) {
        lpcomb[i] = 0.0;
        for (j = 0; j < comblen[i]; j++)
            comb[i][j] = 0.0;
    };

    for (i = 0; i < REV_APS * 2; i++)
        for (j = 0; j < aplen[i]; j++)
            ap[i][j] = 0.0;

    if (idelay != NULL)
        for (i = 0; i < idelaylen; i++)
            idelay[i] = 0.0;

    hpf->cleanup ();
    lpf->cleanup ();


};

/*
 * Process one channel; 0=left,1=right
 */
void
Reverb::processmono (int ch, DspFloatType * output)
{
    int i, j;
    DspFloatType fbout, tmp;
    //TODO: implement the high part from lohidamp

    for (j = REV_COMBS * ch; j < REV_COMBS * (ch + 1); j++) {

        int ck = combk[j];
        int comblength = comblen[j];
        DspFloatType lpcombj = lpcomb[j];

        for (i = 0; i < PERIOD; i++) {
            fbout = comb[j][ck] * combfb[j];
            fbout = fbout * (1.0f - lohifb) + (lpcombj * lohifb);
            lpcombj = fbout;

            comb[j][ck] = inputbuf[i] + fbout;
            output[i] += fbout;

            if ((++ck) >= comblength)
                ck = 0;
        };

        combk[j] = ck;
        lpcomb[j] = lpcombj;
    };

    for (j = REV_APS * ch; j < REV_APS * (1 + ch); j++) {
        int ak = apk[j];
        int aplength = aplen[j];
        for (i = 0; i < PERIOD; i++) {
            tmp = ap[j][ak];
            ap[j][ak] = 0.7f * tmp + output[i];
            output[i] = tmp - 0.7f * ap[j][ak];
            if ((++ak) >= aplength)
                ak = 0;
        };
        apk[j] = ak;
    };
};

/*
 * Effect output
 */
void
Reverb::out (DspFloatType * smps_l, DspFloatType * smps_r)
{
    int i;

    for (i = 0; i < PERIOD; i++) {
        inputbuf[i] = (smps_l[i] + smps_r[i]) * .5f;
        //Initial delay r
        if (idelay != NULL) {
            DspFloatType tmp = inputbuf[i] + idelay[idelayk] * idelayfb;
            inputbuf[i] = idelay[idelayk];
            idelay[idelayk] = tmp;
            idelayk++;
            if (idelayk >= idelaylen)
                idelayk = 0;
        };
    };


    lpf->filterout (inputbuf);
    hpf->filterout (inputbuf);

    processmono (0, efxoutl);	//left
    processmono (1, efxoutr);	//right



    DspFloatType lvol = rs_coeff * pan * 2.0f;
    DspFloatType rvol = rs_coeff * (1.0f - pan) * 2.0f;

    for (int i = 0; i < PERIOD; i++) {
        efxoutl[i] *= lvol;
        efxoutr[i] *= rvol;

    };
};


/*
 * Parameter control
 */
void
Reverb::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();


};

void
Reverb::setpan (int Ppan)
{
    this->Ppan = Ppan;
    pan = (DspFloatType) Ppan / 127.0f;
};

void
Reverb::settime (int Ptime)
{
    int i;
    DspFloatType t;
    this->Ptime = Ptime;
    t = powf (60.0f, (DspFloatType) Ptime / 127.0f) - 0.97f;

    for (i = 0; i < REV_COMBS * 2; i++) {
        combfb[i] =
            -expf ((DspFloatType) comblen[i] / fSAMPLE_RATE * logf (0.001f) /
                   t);
        //the feedback is negative because it removes the DC
    };
};

void
Reverb::setlohidamp (int Plohidamp)
{
    DspFloatType x;

    if (Plohidamp < 64)
        Plohidamp = 64;		//remove this when the high part from lohidamp will be added

    this->Plohidamp = Plohidamp;
    if (Plohidamp == 64) {
        lohidamptype = 0;
        lohifb = 0.0;
    } else {
        if (Plohidamp < 64)
            lohidamptype = 1;
        if (Plohidamp > 64)
            lohidamptype = 2;
        x = fabsf ((DspFloatType) (Plohidamp - 64) / 64.1f);
        lohifb = x * x;
    };
};

void
Reverb::setidelay (int Pidelay)
{
    DspFloatType delay;
    this->Pidelay = Pidelay;
    delay = powf (50.0f * (DspFloatType)Pidelay / 127.0f, 2.0f) - 1.0f;

    if (idelay != NULL)
        delete (idelay);
    idelay = NULL;

    idelaylen = lrintf (fSAMPLE_RATE * delay / 1000.0f);
    if (idelaylen > 1) {
        idelayk = 0;
        idelay = new DspFloatType[idelaylen];
        for (int i = 0; i < idelaylen; i++)
            idelay[i] = 0.0;
    };
};

void
Reverb::setidelayfb (int Pidelayfb)
{
    this->Pidelayfb = Pidelayfb;
    idelayfb = (DspFloatType)Pidelayfb / 128.0f;
};

void
Reverb::sethpf (int value)
{
    Phpf = value;
    DspFloatType fr = (DspFloatType)Phpf;
    hpf->setfreq (fr);

};

void
Reverb::setlpf (int value)
{
    Plpf = value;
    DspFloatType fr = (DspFloatType)Plpf;
    lpf->setfreq (fr);

};

void
Reverb::settype (int Ptype)
{
    const int NUM_TYPES = 2;
    int combtunings[NUM_TYPES][REV_COMBS] = {
        //this is unused (for random)
        {0, 0, 0, 0, 0, 0, 0, 0},
        //Freeverb by Jezar at Dreampoint
        {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617}
    };
    int aptunings[NUM_TYPES][REV_APS] = {
        //this is unused (for random)
        {0, 0, 0, 0},
        //Freeverb by Jezar at Dreampoint
        {225, 341, 441, 556}
    };

    if (Ptype >= NUM_TYPES)
        Ptype = NUM_TYPES - 1;
    this->Ptype = Ptype;

    DspFloatType tmp;
    for (int i = 0; i < REV_COMBS * 2; i++) {
        if (Ptype == 0)
            tmp = 800.0f + (DspFloatType)(RND*1400.0f);
        else
            tmp = (DspFloatType)combtunings[Ptype][i % REV_COMBS];
        tmp *= roomsize;
        if (i > REV_COMBS)
            tmp += 23.0f;
        tmp *= fSAMPLE_RATE / 44100.0f;	//adjust the combs according to the samplerate
        if (tmp < 10)
            tmp = 10;

        comblen[i] = lrintf(tmp);
        combk[i] = 0;
        lpcomb[i] = 0;
        if (comb[i] != NULL)
            delete comb[i];
        comb[i] = new DspFloatType[comblen[i]];
    };

    for (int i = 0; i < REV_APS * 2; i++) {
        if (Ptype == 0)
            tmp = 500.0f + (DspFloatType)(RND*500.0f);
        else
            tmp = (DspFloatType)aptunings[Ptype][i % REV_APS];
        tmp *= roomsize;
        if (i > REV_APS)
            tmp += 23.0f;
        tmp *= fSAMPLE_RATE / 44100.0f;	//adjust the combs according to the samplerate
        if (tmp < 10)
            tmp = 10;
        aplen[i] = lrintf(tmp);
        apk[i] = 0;
        if (ap[i] != NULL)
            delete ap[i];
        ap[i] = new DspFloatType[aplen[i]];
    };
    settime (Ptime);
    cleanup ();
};

void
Reverb::setroomsize (int Proomsize)
{
    if (Proomsize == 0)
        Proomsize = 64;		//this is because the older versions consider roomsize=0
    this->Proomsize = Proomsize;
    roomsize = ((DspFloatType)Proomsize - 64.0f) / 64.0f;
    if (roomsize > 0.0)
        roomsize *= 2.0f;
    roomsize = powf (10.0f, roomsize);
    rs = sqrtf (roomsize);
    rs_coeff = rs / (DspFloatType) REV_COMBS;
    settype (Ptype);
};

void
Reverb::setpreset (int npreset)
{
    const int PRESET_SIZE = 12;
    const int NUM_PRESETS = 13;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Cathedral1
        {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64},
        //Cathedral2
        {80, 64, 69, 35, 0, 0, 0, 25040, 21, 71, 0, 64},
        //Cathedral3
        {80, 64, 69, 24, 0, 0, 0, 25040, 2417, 78, 1, 85},
        //Hall1
        {90, 64, 51, 10, 0, 0, 0, 25040, 81, 78, 1, 64},
        //Hall2
        {90, 64, 53, 20, 0, 0, 0, 25040, 2417, 71, 1, 64},
        //Room1
        {100, 64, 33, 0, 0, 0, 0, 25040, 21, 106, 0, 30},
        //Room2
        {100, 64, 21, 26, 0, 0, 0, 1223, 21, 77, 1, 45},
        //Basement
        {110, 64, 14, 0, 0, 0, 0, 25040, 27, 71, 0, 25},
        //Tunnel
        {85, 80, 84, 20, 42, 0, 0, 652, 21, 78, 1, 105},
        //Echoed1
        {95, 64, 26, 60, 71, 0, 0, 14722, 21, 64, 1, 64},
        //Echoed2
        {90, 64, 40, 88, 71, 0, 0, 14722, 21, 88, 1, 64},
        //VeryLong1
        {90, 64, 93, 15, 0, 0, 0, 14722, 21, 77, 0, 95},
        //VeryLong2
        {90, 64, 111, 30, 0, 0, 0, 14722, 5058, 74, 1, 80}
    };


    if(npreset>NUM_PRESETS-1) {

        Fpre->ReadPreset(8,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Reverb::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpan (value);
        break;
    case 2:
        settime (value);
        break;
    case 3:
        setidelay (value);
        break;
    case 4:
        setidelayfb (value);
        break;
//      case 5: setrdelay(value);
//              break;
//      case 6: seterbalance(value);
//              break;
    case 7:
        setlpf (value);
        break;
    case 8:
        sethpf (value);
        break;
    case 9:
        setlohidamp (value);
        break;
    case 10:
        settype (value);
        break;
    case 11:
        setroomsize (value);
        break;
    };
};

int
Reverb::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppan);
        break;
    case 2:
        return (Ptime);
        break;
    case 3:
        return (Pidelay);
        break;
    case 4:
        return (Pidelayfb);
        break;
//      case 5: return(Prdelay);
//              break;
//      case 6: return(Perbalance);
//              break;
    case 7:
        return (Plpf);
        break;
    case 8:
        return (Phpf);
        break;
    case 9:
        return (Plohidamp);
        break;
    case 10:
        return (Ptype);
        break;
    case 11:
        return (Proomsize);
        break;
    };
    return (0);			//in case of bogus "parameter"
};

Sustainer::Sustainer (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    Pvolume = 64;
    Psustain = 64;
    fsustain = 0.5f;
    level = 0.5f;

    DspFloatType tmp = 0.01f;  //10 ms decay time on peak detectors
    prls = 1.0f - (cSAMPLE_RATE/(cSAMPLE_RATE + tmp));

    tmp = 0.05f; //50 ms att/rel on compressor
    calpha =  cSAMPLE_RATE/(cSAMPLE_RATE + tmp);
    cbeta = 1.0f - calpha;
    cthresh = 0.25f;
    cratio = 0.25f;

    timer = 0;
    hold = (int) (SAMPLE_RATE*0.0125);  //12.5ms
    cleanup ();
};

Sustainer::~Sustainer ()
{

};

/*
 * Cleanup the effect
 */
void
Sustainer::cleanup ()
{
    compeak = 0.0f;
    compenv = 0.0f;
    oldcompenv = 0.0f;
    cpthresh = cthresh; //dynamic threshold
};




/*
 * Effect output
 */
void
Sustainer::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;
    DspFloatType auxtempl = 0.0f;
    DspFloatType auxtempr = 0.0f;
    DspFloatType auxcombi = 0.0f;

    for (i = 0; i<PERIOD; i++) {  //apply compression to auxresampled
        auxtempl = input * smpsl[i];
        auxtempr = input * smpsr[i];
        auxcombi = 0.5f * (auxtempl + auxtempr);
        if(fabs(auxcombi) > compeak) {
            compeak = fabs(auxcombi);   //First do peak detection on the signal
            timer = 0;
        }
        if(timer>hold) {
            compeak *= prls;
            timer--;
        }
        timer++;
        compenv = cbeta * oldcompenv + calpha * compeak;       //Next average into envelope follower
        oldcompenv = compenv;

        if(compenv > cpthresh) {                              //if envelope of signal exceeds thresh, then compress
            compg = cpthresh + cpthresh*(compenv - cpthresh)/compenv;
            cpthresh = cthresh + cratio*(compg - cpthresh);   //cpthresh changes dynamically
            tmpgain = compg/compenv;
        } else {
            tmpgain = 1.0f;
        }

        if(compenv < cpthresh) cpthresh = compenv;
        if(cpthresh < cthresh) cpthresh = cthresh;

        smpsl[i] = auxtempl * tmpgain * level;
        smpsr[i] = auxtempr * tmpgain * level;
    };
    //End compression
};


/*
 * Parameter control
 */


void
Sustainer::setpreset (int npreset)
{
    const int PRESET_SIZE = 2;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Moderate
        {79, 54},
        //Extreme
        {16, 127},
        //Mild
        {120, 15},

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(36,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Sustainer::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        Pvolume = value;
        level = dB2rap(-30.0f * (1.0f - ((DspFloatType) Pvolume/127.0f)));
        break;
    case 1:
        Psustain = value;
        fsustain =  (DspFloatType) Psustain/127.0f;
        cratio = 1.25f - fsustain;
        input = dB2rap (42.0f * fsustain - 6.0f);
        cthresh = 0.25 + fsustain;
        break;

    };
};

int
Sustainer::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Psustain);
        break;
    };
    return (0);			//in case of bogus parameter number
};



/*
  Synthfilter.C  - Approximate digital model of an analog JFET phaser.
  Analog modeling implemented by Ryan Billing aka Transmogrifox.
  November, 2009
  Credit to:
  ///////////////////
  ZynAddSubFX - a software synthesizer
  Phaser.C - Phaser effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul
  Modified for rakarrack by Josep Andreu
  DSP analog modeling theory & practice largely influenced by various CCRMA publications, particularly works by Julius O. Smith.
  ////////////////////
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.
  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/


#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Synthfilter::Synthfilter (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lyn1 = new DspFloatType[MAX_SFILTER_STAGES];
    ryn1 = new DspFloatType[MAX_SFILTER_STAGES];
    lx1hp = new DspFloatType[MAX_SFILTER_STAGES];
    rx1hp = new DspFloatType[MAX_SFILTER_STAGES];
    ly1hp = new DspFloatType[MAX_SFILTER_STAGES];
    ry1hp = new DspFloatType[MAX_SFILTER_STAGES];

    Plpstages = 4;
    Phpstages = 2;

    inv_period = 1.0f/fPERIOD;

    delta = cSAMPLE_RATE;
    Rmin = 185.0f;		// 2N5457 typical on resistance at Vgs = 0
    Rmax = 22000.0f;		// Resistor
    C = 0.00000005f;		// 50 nF
    Chp = 0.00000005f;
    Clp = 0.00000005f;
    att = delta * 5.0f;		//200ms
    rls = delta * 5.0f;		//200ms


    Ppreset = 0;
    setpreset (Ppreset);
    cleanup ();
};

Synthfilter::~Synthfilter ()
{
};


/*
 * Effect output
 */
void
Synthfilter::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j;
    DspFloatType lfol, lfor, lgain, rgain,rmod, lmod, d;
    lgain = 0.0;
    rgain = 0.0;

    lfo.effectlfoout (&lfol, &lfor);
    lmod = lfol*width + depth + env*sns;
    rmod = lfor*width + depth + env*sns;

    if (lmod > ONE_)
        lmod = ONE_;
    else if (lmod < ZERO_)
        lmod = ZERO_;
    if (rmod > ONE_)
        rmod = ONE_;
    else if (rmod < ZERO_)
        rmod = ZERO_;

    lmod = 1.0f - lmod;
    rmod = 1.0f - rmod;
    lmod*=lmod;
    rmod*=rmod;

    DspFloatType xl = (lmod - oldlgain) * inv_period;
    DspFloatType xr = (rmod - oldrgain) * inv_period;
    DspFloatType gl = oldlgain;	// Linear interpolation between LFO samples
    DspFloatType gr = oldrgain;

    for (i = 0; i < PERIOD; i++) {

        DspFloatType lxn = bandgain*smpsl[i];
        DspFloatType rxn = bandgain*smpsr[i]; //extra gain

        gl += xl;
        gr += xr;   //linear interpolation of LFO

        //Envelope detection
        envdelta = (fabsf (efxoutl[i]) + fabsf (efxoutr[i])) - env;    //envelope follower from Compressor.C
        if (delta > 0.0)
            env += att * envdelta;
        else
            env += rls * envdelta;

        //End envelope power detection

        if (Plpstages<1) {
            lxn += fbl;
            rxn += fbr;
        }


        //Left channel Low Pass Filter
        for (j = 0; j < Plpstages; j++) {
            d = 1.0f + fabs(lxn)*distortion;  // gain decreases as signal amplitude increases.

            //low pass filter:  alpha*x[n] + (1-alpha)*y[n-1]
            // alpha = lgain = dt/(RC + dt)
            lgain = delta/( (Rmax * gl * d + Rmin) * Clp + delta);
            lyn1[j] = lgain * lxn + (1.0f - lgain) * lyn1[j];
            lyn1[j] += DENORMAL_GUARD;
            lxn = lyn1[j];
            if (j==0) lxn += fbl;  //Insert feedback after first filter stage
        };


        //Left channel High Pass Filter
        for (j = 0; j < Phpstages; j++) {

            //high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = lgain = RC/(RC + dt)
            lgain = (Rmax * gl + Rmin) * Chp/( (Rmax * gl  + Rmin) * Chp + delta);
            ly1hp[j] = lgain * (lxn + ly1hp[j] - lx1hp[j]);

            ly1hp[j] += DENORMAL_GUARD;
            lx1hp[j] = lxn;
            lxn = ly1hp[j];

        };


        //Right channel Low Pass Filter
        for (j = 0; j < Plpstages; j++) {
            d = 1.0f + fabs(rxn)*distortion;  //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.

            rgain = delta/((Rmax*gr*d + Rmin)*Clp + delta);
            ryn1[j] = rgain * rxn + (1.0f - rgain) * ryn1[j];
            ryn1[j] += DENORMAL_GUARD;
            rxn = ryn1[j];
            if (j==0) rxn += fbr;  //Insert feedback after first filter stage
        };

        //Right channel High Pass Filter
        for (j = 0; j < Phpstages; j++) {
            d = 1.0f + fabs(rxn)*distortion;  // gain decreases as signal amplitude increases.

            //high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = rgain = RC/(RC + dt)
            rgain = (Rmax * gr  + Rmin) * Chp/( (Rmax * gr + Rmin) * Chp + delta);
            ry1hp[j] = rgain * (rxn + ry1hp[j] - rx1hp[j]);

            ry1hp[j] += DENORMAL_GUARD;
            rx1hp[j] = rxn;
            rxn = ry1hp[j];
        };

        fbl = lxn * fb;
        fbr = rxn * fb;

        efxoutl[i] = lxn;
        efxoutr[i] = rxn;

    };

    oldlgain = lmod;
    oldrgain = rmod;

    if (Poutsub != 0)
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] *= -1.0f;
            efxoutr[i] *= -1.0f;
        };

};

/*
 * Cleanup the effect
 */
void
Synthfilter::cleanup ()
{
    fbl = 0.0f;
    fbr = 0.0f;
    oldlgain = 0.0f;
    oldrgain = 0.0f;
    env = 0.0f;
    envdelta = 0.0f;
    for (int i = 0; i <MAX_SFILTER_STAGES; i++) {
        lyn1[i] = 0.0f;
        ryn1[i] = 0.0f;

        ly1hp[i] = 0.0f;
        lx1hp[i] = 0.0f;
        ry1hp[i] = 0.0f;
        rx1hp[i] = 0.0f;
    };
};

/*
 * Parameter control
 */
void
Synthfilter::setwidth (int Pwidth)
{
    this->Pwidth = Pwidth;
    width = ((DspFloatType)Pwidth / 127.0f);
};


void
Synthfilter::setfb (int Pfb)
{
    this->Pfb = Pfb;
    fb = (DspFloatType) Pfb;
    if (fb<0.0f) fb /= 18.0f;
    else if (fb>0.0f) fb/=65.0f;
    if(Plpstages<=2) fb *= 0.3;  //keep filter stable when phase shift is small

};

void
Synthfilter::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    // outvolume is needed in calling program
    outvolume = (DspFloatType)Pvolume / 127.0f;
};

void
Synthfilter::setdistortion (int Pdistortion)
{
    this->Pdistortion = Pdistortion;
    distortion = (DspFloatType)Pdistortion / 127.0f;
};


void
Synthfilter::setdepth (int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = (DspFloatType)(Pdepth - 32) / 95.0f;  //Pdepth input should be 0-127. .
};


void
Synthfilter::setpreset (int npreset)
{
    const int PRESET_SIZE = 16;
    const int NUM_PRESETS = 7;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Low Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0},
        //High Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 0, 6, 0, 32, -32, 500, 100, 0},
        //Band Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 4, 4, 0, 32, -32, 500, 100, 0},
        //Lead Synth
        {0, 89, 31, 0, 1, 95, 38, -16, 1, 2, 1, 114, -32, 92, 215, 29},
        //Water
        {20, 69, 88, 0, 6, 0, 76, -50, 6, 2, 1, 0, 19, 114, 221, 127},
        //Pan Filter
        {0, 20, 100, 0, 5, 127, 127, -64, 2, 0, 0, 57, 0, 340, 288, 110},
        //Multi
        {64, 45, 88, 0, 1, 127, 81, 0, 4, 2, 0, 67, 0, 394, 252, 61}


    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(27,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Synthfilter::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setdistortion (value);
        break;
    case 2:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 5:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 6:
        setwidth (value);
        break;
    case 7:
        setfb (value);
        break;
    case 8:
        Plpstages = value;
        if (Plpstages >= MAX_SFILTER_STAGES)
            Plpstages = MAX_SFILTER_STAGES ;
        if(Plpstages<=2) fb = (DspFloatType) Pfb * 0.25/65.0f;  //keep filter stable when phase shift is small
        cleanup ();
        break;
    case 9:
        Phpstages = value;
        if (Phpstages >= MAX_SFILTER_STAGES)
            Phpstages = MAX_SFILTER_STAGES ;
        cleanup ();
        break;
    case 10:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case 11:
        setdepth (value);
        break;
    case 12:
        Penvelope = value;
        sns = (DspFloatType) Penvelope/8.0f;
        break;
    case 13:
        Pattack = value;
        if(Pattack < 5) Pattack = 5;
        att = delta * 1000.0f/((DspFloatType) Pattack);
        break;
    case 14:
        Prelease = value;
        if(Prelease < 5) Prelease = 5;
        rls = delta * 1000.0f/((DspFloatType) Prelease);
        break;
    case 15:
        Pbandwidth = value;
        Chp = C * (1.0f + ((DspFloatType) value)/64.0f);  // C*3
        Clp = C * (1.0f - ((DspFloatType) value)/190.0f); // C/3
        break;
    };

    if(Phpstages && Plpstages)
        bandgain = powf(((DspFloatType)(Phpstages*Plpstages + 3)), (1.0f - (DspFloatType) Pbandwidth/127.0f));
    else bandgain = 1.0f;

};

int
Synthfilter::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pdistortion);
        break;
    case 2:
        return (lfo.Pfreq);
        break;
    case 3:
        return (lfo.Prandomness);
        break;
    case 4:
        return (lfo.PLFOtype);
        break;
    case 5:
        return (lfo.Pstereo);
        break;
    case 6:
        return (Pwidth);
        break;
    case 7:
        return (Pfb);
        break;
    case 8:
        return (Plpstages);
        break;
    case 9:
        return (Phpstages);
        break;
    case 10:
        return (Poutsub);
        break;
    case 11:
        return (Pdepth);
        break;
    case 12:
        return (Penvelope);
        break;
    case 13:
        return (Pattack);
        break;
    case 14:
        return (Prelease);
        break;
    case 15:
        return (Pbandwidth);
        break;

    default:
        return (0);
    };

};



Valve::Valve (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    lpfl = new AnalogFilter (2, 22000.0f, 1.0f, 0);
    lpfr = new AnalogFilter (2, 22000.0f, 1.0f, 0);
    hpfl = new AnalogFilter (3, 20.0f, 1.0f, 0);
    hpfr = new AnalogFilter (3, 20.0f, 1.0f, 0);
    harm = new HarmEnhancer (rm, 20.0f,20000.0f,1.0f);


    //default values
    Ppreset = 0;
    Pvolume = 50;
    Plrcross = 40;
    Pdrive = 90;
    Plevel = 64;
    Pnegate = 0;
    Plpf = 127;
    Phpf = 0;
    Q_q = 64;
    Ped = 0;
    Pstereo = 0;
    Pprefiltering = 0;
    q = 0.0f;
    dist = 0.0f;
    setlpf(127);
    sethpf(1);
    atk = 1.0f - 40.0f/fSAMPLE_RATE;

    for(int i=0; i<10; i++) rm[i]=0.0;
    rm[0]=1.0f;
    rm[2]= -1.0f;
    rm[4]=1.0f;
    rm[6]=-1.0f;
    rm[8]=1.0f;
    harm->calcula_mag(rm);

    setpreset (Ppreset);
    init_coefs();
    cleanup ();
};

Valve::~Valve ()
{
};

/*
 * Cleanup the effect
 */
void
Valve::cleanup ()
{
    lpfl->cleanup ();
    hpfl->cleanup ();
    lpfr->cleanup ();
    hpfr->cleanup ();
    otml = 0.0f;
    itml=0.0f;
    otmr=0.0f;
    itmr=0.0f;




};


/*
 * Apply the filters
 */

void
Valve::applyfilters (DspFloatType * efxoutl, DspFloatType * efxoutr)
{
    lpfl->filterout (efxoutl);
    hpfl->filterout (efxoutl);

    if (Pstereo != 0) {
        //stereo
        lpfr->filterout (efxoutr);
        hpfr->filterout (efxoutr);
    };

};


DspFloatType
Valve::Wshape(DspFloatType x)
{

    if(x<factor) return(x);
    if(x>factor) return(factor+(x-factor)/powf(1.0f+((x-factor)/(1.0f-factor)),2.0f));
    if(x>1.0f) return((factor+1.0f)*.5f);
    return(0.0);
}



/*
 * Effect output
 */
void
Valve::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i;

    DspFloatType l, r, lout, rout, fx;


    if (Pstereo != 0) {
        //Stereo
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] = smpsl[i] * inputvol;
            efxoutr[i] = smpsr[i] * inputvol;
        };
    } else {
        for (i = 0; i < PERIOD; i++) {
            efxoutl[i] =
                (smpsl[i]  +  smpsr[i] ) * inputvol;
        };
    };

    harm->harm_out(efxoutl,efxoutr);


    if (Pprefiltering != 0)
        applyfilters (efxoutl, efxoutr);

    if(Ped) {
        for (i =0; i<PERIOD; i++) {
            efxoutl[i]=Wshape(efxoutl[i]);
            if (Pstereo != 0) efxoutr[i]=Wshape(efxoutr[i]);
        }
    }

    for (i =0; i<PERIOD; i++) { //soft limiting to 3.0 (max)
        fx = efxoutl[i];
        if (fx>1.0f) fx = 3.0f - 2.0f/sqrtf(fx);
        efxoutl[i] = fx;
        fx = efxoutr[i];
        if (fx>1.0f) fx = 3.0f - 2.0f/sqrtf(fx);
        efxoutr[i] = fx;
    }

    if (q == 0.0f) {
        for (i =0; i<PERIOD; i++) {
            if (efxoutl[i] == q) fx = fdist;
            else fx =efxoutl[i] / (1.0f - powf(2.0f,-dist * efxoutl[i] ));
            otml = atk * otml + fx - itml;
            itml = fx;
            efxoutl[i]= otml;
        }
    } else {
        for (i = 0; i < PERIOD; i++) {
            if (efxoutl[i] == q) fx = fdist + qcoef;
            else fx =(efxoutl[i] - q) / (1.0f - powf(2.0f,-dist * (efxoutl[i] - q))) + qcoef;
            otml = atk * otml + fx - itml;
            itml = fx;
            efxoutl[i]= otml;

        }
    }


    if (Pstereo != 0) {

        if (q == 0.0f) {
            for (i =0; i<PERIOD; i++) {
                if (efxoutr[i] == q) fx = fdist;
                else fx = efxoutr[i] / (1.0f - powf(2.0f,-dist * efxoutr[i] ));
                otmr = atk * otmr + fx - itmr;
                itmr = fx;
                efxoutr[i]= otmr;

            }
        } else {
            for (i = 0; i < PERIOD; i++) {
                if (efxoutr[i] == q) fx = fdist + qcoef;
                else fx = (efxoutr[i] - q) / (1.0f - powf(2.0f,-dist * (efxoutr[i] - q))) + qcoef;
                otmr = atk * otmr + fx - itmr;
                itmr = fx;
                efxoutr[i]= otmr;

            }
        }

    }



    if (Pprefiltering == 0)
        applyfilters (efxoutl, efxoutr);

    if (Pstereo == 0) memcpy (efxoutr , efxoutl, PERIOD * sizeof(DspFloatType));


    DspFloatType level = dB2rap (60.0f * (DspFloatType)Plevel / 127.0f - 40.0f);

    for (i = 0; i < PERIOD; i++) {
        lout = efxoutl[i];
        rout = efxoutr[i];


        l = lout * (1.0f - lrcross) + rout * lrcross;
        r = rout * (1.0f - lrcross) + lout * lrcross;

        lout = l;
        rout = r;

        efxoutl[i] = lout * 2.0f * level * panning;
        efxoutr[i] = rout * 2.0f * level * (1.0f -panning);

    };



};


/*
 * Parameter control
 */
void
Valve::init_coefs()
{
    coef = 1.0 / (1.0f - powf(2.0f,dist * q ));
    qcoef = q * coef;
    fdist = 1.0f / dist;
    inputvol = powf (4.0f, ((DspFloatType)Pdrive - 32.0f) / 127.0f);
    if (Pnegate != 0)
        inputvol *= -1.0f;
};

void
Valve::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;

    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Valve::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
};


void
Valve::setlrcross (int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (DspFloatType)Plrcross / 127.0f * 1.0f;
};

void
Valve::setlpf (int value)
{
    Plpf = value;
    DspFloatType fr = (DspFloatType)Plpf;

    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Valve::sethpf (int value)
{
    Phpf = value;
    DspFloatType fr = (DspFloatType)Phpf;

    hpfl->setfreq (fr);
    hpfr->setfreq (fr);

    //Prefiltering of 51 is approx 630 Hz. 50 - 60 generally good for OD pedal.
};

void
Valve::setpresence(int value)
{

    DspFloatType freq=5.0f*(100.0f-(DspFloatType)value);
    DspFloatType nvol=(DspFloatType)(value*.01f);

    harm->set_freqh(1, freq);
    harm->set_vol(1,  nvol);



}



void
Valve::setpreset (int npreset)
{
    const int PRESET_SIZE = 13;
    const int NUM_PRESETS = 3;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Valve 1
        {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 84},
        //Valve 2
        {0, 64, 64, 127, 64, 0, 5078, 61, 1, 0, 112, 0, 30},
        //Valve 3
        {0, 64, 35, 80, 64, 1, 3134, 358, 1, 1, 100, 1, 30}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(19,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
    cleanup ();
};


void
Valve::changepar (int npar, int value)
{
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        setlrcross (value);
        break;
    case 3:
        Pdrive = value;
        dist = (DspFloatType) Pdrive / 127.0f * 40.0f + .5f;
        break;
    case 4:
        Plevel = value;
        break;
    case 5:
        if (value > 1)
            value = 1;
        Pnegate = value;
        break;
    case 6:
        setlpf (value);
        break;
    case 7:
        sethpf (value);
        break;
    case 8:
        if (value > 1)
            value = 1;
        Pstereo = value;
        break;
    case 9:
        Pprefiltering = value;
        break;
    case 10:
        Q_q = value;
        q = (DspFloatType)Q_q /127.0f - 1.0f;
        factor = 1.0f - ((DspFloatType)Q_q / 128.0f);
        break;
    case 11:
        Ped = value;
        break;
    case 12:
        Presence=value;
        setpresence(value);
        break;

        init_coefs();

    };
};

int
Valve::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return (Plrcross);
        break;
    case 3:
        return (Pdrive);
        break;
    case 4:
        return (Plevel);
        break;
    case 5:
        return (Pnegate);
        break;
    case 6:
        return (Plpf);
        break;
    case 7:
        return (Phpf);
        break;
    case 8:
        return (Pstereo);
        break;
    case 9:
        return (Pprefiltering);
        break;
    case 10:
        return (Q_q);
        break;
    case 11:
        return (Ped);
    case 12:
        return (Presence);
        break;
    };
    return (0);			//in case of bogus parameter number
};


Vibe::Vibe (DspFloatType * efxoutl_, DspFloatType * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

//Swing was measured on operating device of: 10K to 250k.
//400K is reported to sound better for the "low end" (high resistance)
//Because of time response, Rb needs to be driven further.
//End resistance will max out to around 10k for most LFO freqs.
//pushing low end a little lower for kicks and giggles
    Ra = 500000.0f;  //Cds cell dark resistance.
    Ra = logf(Ra);		//this is done for clarity
    Rb = 600.0f;         //Cds cell full illumination
    b = exp(Ra/logf(Rb)) - CNST_E;
    dTC = 0.085f;
    dRCl = dTC;
    dRCr = dTC;   //Right & left channel dynamic time contsants
    minTC = logf(0.005f/dTC);
    alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
    alphar = alphal;
    dalphal = dalphar = alphal;
    lampTC = cSAMPLE_RATE/(0.012 + cSAMPLE_RATE);  //guessing twiddle factor
    ilampTC = 1.0f - lampTC;
    lstep = 0.0f;
    rstep = 0.0f;

    Pstereo = 1; //1 if you want to process in stereo, 0 to do mono proc
    Pdepth = 127;
    Ppanning = 64;
    lpanning = 1.0f;
    rpanning = 1.0f;
    fdepth = 1.0f;
    oldgl = 0.0f;
    oldgr = 0.0f;
    gl = 0.0f;
    gr = 0.0f;
    for(int jj = 0; jj<8; jj++) oldcvolt[jj] = 0.0f;
    cperiod = 1.0f/fPERIOD;

    init_vibes();
    cleanup();

}

Vibe::~Vibe ()
{
}


void
Vibe::cleanup ()
{


};

void
Vibe::out (DspFloatType *smpsl, DspFloatType *smpsr)
{

    int i,j;
    DspFloatType lfol, lfor, xl, xr, fxl, fxr;
    //DspFloatType vbe,vin;
    DspFloatType cvolt, ocvolt, evolt, input;
    DspFloatType emitterfb = 0.0f;
    DspFloatType outl, outr;

    input = cvolt = ocvolt = evolt = 0.0f;

    lfo.effectlfoout (&lfol, &lfor);

    lfol = fdepth + lfol*fwidth;
    if (lfol > 1.0f)
        lfol = 1.0f;
    else if (lfol < 0.0f)
        lfol = 0.0f;
    lfol = 2.0f - 2.0f/(lfol + 1.0f); //emulate lamp turn on/off characteristic by typical curves

    if(Pstereo) {
        lfor = fdepth + lfor*fwidth;
        if (lfor > 1.0f) lfor = 1.0f;
        else if (lfor < 0.0f) lfor = 0.0f;
        lfor = 2.0f - 2.0f/(lfor + 1.0f);   //
    }

    for (i = 0; i < PERIOD; i++) {
        //Left Lamp
        gl = lfol*lampTC + oldgl*ilampTC;
        oldgl = gl;

        //Left Cds
        stepl = gl*alphal + dalphal*oldstepl;
        oldstepl = stepl;
        dRCl = dTC*f_exp(stepl*minTC);
        alphal = cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
        dalphal = 1.0f - cSAMPLE_RATE/(0.5f*dRCl + cSAMPLE_RATE);     //different attack & release character
        xl = CNST_E + stepl*b;
        fxl = f_exp(Ra/logf(xl));

        //Right Lamp
        if(Pstereo) {
            gr = lfor*lampTC + oldgr*ilampTC;
            oldgr = gr;

            //Right Cds
            stepr = gr*alphar + dalphar*oldstepr;
            oldstepr = stepr;
            dRCr = dTC*f_exp(stepr*minTC);
            alphar = cSAMPLE_RATE/(dRCr + cSAMPLE_RATE);
            dalphar = 1.0f - cSAMPLE_RATE/(0.5f*dRCr + cSAMPLE_RATE);      //different attack & release character
            xr = CNST_E + stepr*b;
            fxr = f_exp(Ra/logf(xr));
        }

        if(i%16 == 0)  modulate(fxl, fxr);

        //Left Channel
        input = bjt_shape(fbl + smpsl[i]);


        /*
        //Inline BJT Shaper bleow
            vin = 7.5f*(1.0f + fbl+smpsl[i]);
            if(vin<0.0f) vin = 0.0f;
            if(vin>15.0f) vin = 15.0f;
            vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
            input = vin - vbe;
            input = input*0.1333333333f -0.90588f;  //some magic numbers to return gain to unity & zero the DC
        */

        emitterfb = 25.0f/fxl;
        for(j=0; j<4; j++) { //4 stages phasing
            /*
            //Inline filter implementation below
                DspFloatType y0 = 0.0f;
                y0 = input*ecvc[j].n0 + ecvc[j].x1*ecvc[j].n1 - ecvc[j].y1*ecvc[j].d1;
                ecvc[j].y1 = y0 + DENORMAL_GUARD;
                ecvc[j].x1 = input;
                DspFloatType x0 = 0.0f;
                DspFloatType data = input + emitterfb*oldcvolt[j];
                x0 = data*vc[j].n0 + vc[j].x1*vc[j].n1 - vc[j].y1*vc[j].d1;
                vc[j].y1 = x0 + DENORMAL_GUARD;
                vc[j].x1 = data;
                cvolt=y0+x0;
                ocvolt= cvolt*vcvo[j].n0 + vcvo[j].x1*vcvo[j].n1 - vcvo[j].y1*vcvo[j].d1;
                vcvo[j].y1 = ocvolt + DENORMAL_GUARD;
                vcvo[j].x1 = cvolt;
                oldcvolt[j] = ocvolt;
                evolt = input*vevo[j].n0 + vevo[j].x1*vevo[j].n1 - vevo[j].y1*vevo[j].d1;
                vevo[j].y1 = evolt + DENORMAL_GUARD;
                vevo[j].x1 = input;
                vin = 7.5f*(1.0f + ocvolt+evolt);
                if(vin<0.0f) vin = 0.0f;
                if(vin>15.0f) vin = 15.0f;
                vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
                input = vin - vbe;
                input = input*0.1333333333f -0.90588f;  //some magic numbers to return gain to unity & zero the DC
            */

// Orig code, Comment below if instead using inline
            cvolt = vibefilter(input,ecvc,j) + vibefilter(input + emitterfb*oldcvolt[j],vc,j);
            ocvolt = vibefilter(cvolt,vcvo,j);
            oldcvolt[j] = ocvolt;
            evolt = vibefilter(input, vevo,j);

            input = bjt_shape(ocvolt + evolt);
            //Close comment here if using inline

        }
        fbl = fb*ocvolt;
        outl = lpanning*input;

        //Right channel

        if(Pstereo) {
            /*
            //Inline BJT shaper
             vin = 7.5f*(1.0f + fbr+smpsr[i]);
             if(vin<0.0f) vin = 0.0f;
             if(vin>15.0f) vin = 15.0f;
             vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
             input = vin - vbe;
             input = input*0.1333333333f -0.90588f;  //some magic numbers to return gain to unity & zero the DC
             */


//Orig code
            input = bjt_shape(fbr + smpsr[i]);
            //Close Comment here if using Inline instead

            emitterfb = 25.0f/fxr;
            for(j=4; j<8; j++) { //4 stages phasing

                /*
                //This is the inline version
                    DspFloatType y0 = 0.0f;
                    y0 = input*ecvc[j].n0 + ecvc[j].x1*ecvc[j].n1 - ecvc[j].y1*ecvc[j].d1;
                    ecvc[j].y1 = y0 + DENORMAL_GUARD;
                    ecvc[j].x1 = input;
                    DspFloatType x0 = 0.0f;
                    DspFloatType data = input + emitterfb*oldcvolt[j];
                    x0 = data*vc[j].n0 + vc[j].x1*vc[j].n1 - vc[j].y1*vc[j].d1;
                    vc[j].y1 = x0 + DENORMAL_GUARD;
                    vc[j].x1 = data;
                    cvolt=y0+x0;
                    ocvolt= cvolt*vcvo[j].n0 + vcvo[j].x1*vcvo[j].n1 - vcvo[j].y1*vcvo[j].d1;
                    vcvo[j].y1 = ocvolt + DENORMAL_GUARD;
                    vcvo[j].x1 = cvolt;
                    oldcvolt[j] = ocvolt;
                    evolt = input*vevo[j].n0 + vevo[j].x1*vevo[j].n1 - vevo[j].y1*vevo[j].d1;
                    vevo[j].y1 = evolt + DENORMAL_GUARD;
                    vevo[j].x1 = input;
                    vin = 7.5f*(1.0f + ocvolt+evolt);
                    if(vin<0.0f) vin = 0.0f;
                    if(vin>15.0f) vin = 15.0f;
                    vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
                    input = vin - vbe;
                    input = input*0.1333333333f -0.90588f;  //some magic numbers to return gain to unity & zero the DC
                */

//  Comment block below if using inline code instead
                cvolt = vibefilter(input,ecvc,j) + vibefilter(input + emitterfb*oldcvolt[j],vc,j);
                ocvolt = vibefilter(cvolt,vcvo,j);
                oldcvolt[j] = ocvolt;
                evolt = vibefilter(input, vevo,j);

                input = bjt_shape(ocvolt + evolt);
// Close comment here if inlining
            }

            fbr = fb*ocvolt;
            outr = rpanning*input;

            efxoutl[i] = outl*fcross + outr*flrcross;
            efxoutr[i] = outr*fcross + outl*flrcross;
        }  else {  //if(Pstereo)
            efxoutl[i] = outl;
            efxoutr[i] = outl;
        }

    };

};

DspFloatType
Vibe::vibefilter(DspFloatType data, fparams *ftype, int stage)
{
    DspFloatType y0 = 0.0f;
    y0 = data*ftype[stage].n0 + ftype[stage].x1*ftype[stage].n1 - ftype[stage].y1*ftype[stage].d1;
    ftype[stage].y1 = y0 + DENORMAL_GUARD;
    ftype[stage].x1 = data;
    return y0;
};

DspFloatType
Vibe::bjt_shape(DspFloatType data)
{
    DspFloatType vbe, vout;
    DspFloatType vin = 7.5f*(1.0f + data);
    if(vin<0.0f) vin = 0.0f;
    if(vin>15.0f) vin = 15.0f;
    vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
    vout = vin - vbe;
    vout = vout*0.1333333333f - 0.90588f;  //some magic numbers to return gain to unity & zero the DC
    return vout;
}

void
Vibe::init_vibes()
{
    k = 2.0f*fSAMPLE_RATE;
    DspFloatType tmpgain = 1.0f;
    R1 = 4700.0f;
    Rv = 4700.0f;
    C2 = 1e-6f;
    beta = 150.0f;  //transistor forward gain.
    gain = -beta/(beta + 1.0f);

//Univibe cap values 0.015uF, 0.22uF, 470pF, and 0.0047uF
    C1[0] = 0.015e-6f;
    C1[1] = 0.22e-6f;
    C1[2] = 470e-12f;
    C1[3] = 0.0047e-6f;
    C1[4] = 0.015e-6f;
    C1[5] = 0.22e-6f;
    C1[6] = 470e-12f;
    C1[7] = 0.0047e-6f;

    for(int i =0; i<8; i++) {
//Vo/Ve driven from emitter
        en1[i] = k*R1*C1[i];
        en0[i] = 1.0f;
        ed1[i] = k*(R1 + Rv)*C1[i];
        ed0[i] = 1.0f + C1[i]/C2;

// Vc~=Ve/(Ic*Re*alpha^2) collector voltage from current input.
//Output here represents voltage at the collector

        cn1[i] = k*gain*Rv*C1[i];
        cn0[i] = gain*(1.0f + C1[i]/C2);
        cd1[i] = k*(R1 + Rv)*C1[i];
        cd0[i] = 1.0f + C1[i]/C2;

//Contribution from emitter load through passive filter network
        ecn1[i] = k*gain*R1*(R1 + Rv)*C1[i]*C2/(Rv*(C2 + C1[i]));
        ecn0[i] = 0.0f;
        ecd1[i] = k*(R1 + Rv)*C1[i]*C2/(C2 + C1[i]);
        ecd0[i] = 1.0f;

// %Represents Vo/Vc.  Output over collector voltage
        on1[i] = k*Rv*C2;
        on0[i] = 1.0f;
        od1[i] = k*Rv*C2;
        od0[i] = 1.0f + C2/C1[i];

//%Bilinear xform stuff
        tmpgain =  1.0f/(cd1[i] + cd0[i]);
        vc[i].n1 = tmpgain*(cn0[i] - cn1[i]);
        vc[i].n0 = tmpgain*(cn1[i] + cn0[i]);
        vc[i].d1 = tmpgain*(cd0[i] - cd1[i]);
        vc[i].d0 = 1.0f;

        tmpgain =  1.0f/(ecd1[i] + ecd0[i]);
        ecvc[i].n1 = tmpgain*(ecn0[i] - ecn1[i]);
        ecvc[i].n0 = tmpgain*(ecn1[i] + ecn0[i]);
        ecvc[i].d1 = tmpgain*(ecd0[i] - ecd1[i]);
        ecvc[i].d0 = 1.0f;

        tmpgain =  1.0f/(od1[i] + od0[i]);
        vcvo[i].n1 = tmpgain*(on0[i] - on1[i]);
        vcvo[i].n0 = tmpgain*(on1[i] + on0[i]);
        vcvo[i].d1 = tmpgain*(od0[i] - od1[i]);
        vcvo[i].d0 = 1.0f;

        tmpgain =  1.0f/(ed1[i] + ed0[i]);
        vevo[i].n1 = tmpgain*(en0[i] - en1[i]);
        vevo[i].n0 = tmpgain*(en1[i] + en0[i]);
        vevo[i].d1 = tmpgain*(ed0[i] - ed1[i]);
        vevo[i].d0 = 1.0f;

// bootstrap[i].n1
// bootstrap[i].n0
// bootstrap[i].d1
    }


};

void
Vibe::modulate(DspFloatType ldrl, DspFloatType ldrr)
{
    DspFloatType tmpgain;
    DspFloatType R1pRv;
    DspFloatType C2pC1;
    Rv = 4700.0f + ldrl;
    R1pRv = R1 + Rv;

    int lrchoice;
    if(Pstereo) lrchoice = 8;
    else lrchoice = 4;

    for(int i =0; i<8; i++) {
        if(i==4) {
            Rv = 4700.0f + ldrr;
            R1pRv = R1 + Rv;
        }

        C2pC1 = C2 + C1[i];
//Vo/Ve driven from emitter
        ed1[i] = k*(R1pRv)*C1[i];
//ed1[i] = R1pRv*kC1[i];

// Vc~=Ve/(Ic*Re*alpha^2) collector voltage from current input.
//Output here represents voltage at the collector
        cn1[i] = k*gain*Rv*C1[i];
//cn1[i] = kgainCl[i]*Rv;
//cd1[i] = (R1pRv)*C1[i];
        cd1[i]=ed1[i];

//Contribution from emitter load through passive filter network
        ecn1[i] = k*gain*R1*cd1[i]*C2/(Rv*(C2pC1));
//ecn1[i] = iC2pC1[i]*kgainR1C2*cd1[i]/Rv;
        ecd1[i] = k*cd1[i]*C2/(C2pC1);
//ecd1[i] = iC2pC1[i]*k*cd1[i]*C2/(C2pC1);

// %Represents Vo/Vc.  Output over collector voltage
        on1[i] = k*Rv*C2;
        od1[i] = on1[i];

//%Bilinear xform stuff
        tmpgain =  1.0f/(cd1[i] + cd0[i]);
        vc[i].n1 = tmpgain*(cn0[i] - cn1[i]);
        vc[i].n0 = tmpgain*(cn1[i] + cn0[i]);
        vc[i].d1 = tmpgain*(cd0[i] - cd1[i]);

        tmpgain =  1.0f/(ecd1[i] + ecd0[i]);
        ecvc[i].n1 = tmpgain*(ecn0[i] - ecn1[i]);
        ecvc[i].n0 = tmpgain*(ecn1[i] + ecn0[i]);
        ecvc[i].d1 = tmpgain*(ecd0[i] - ecd1[i]);
        ecvc[i].d0 = 1.0f;

        tmpgain =  1.0f/(od1[i] + od0[i]);
        vcvo[i].n1 = tmpgain*(on0[i] - on1[i]);
        vcvo[i].n0 = tmpgain*(on1[i] + on0[i]);
        vcvo[i].d1 = tmpgain*(od0[i] - od1[i]);

        tmpgain =  1.0f/(ed1[i] + ed0[i]);
        vevo[i].n1 = tmpgain*(en0[i] - en1[i]);
        vevo[i].n0 = tmpgain*(en1[i] + en0[i]);
        vevo[i].d1 = tmpgain*(ed0[i] - ed1[i]);

    }


};

void
Vibe::setpanning (int value)
{
    Ppanning = value;
    rpanning = ((DspFloatType)Ppanning) / 64.0f;
    lpanning = 2.0f - rpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
    rpanning = 1.0f - 1.0f/(rpanning + 1.0f);
    lpanning *= 1.3f;
    rpanning *= 1.3f;
};


void
Vibe::setvolume (int value)
{
    Pvolume = value;
    outvolume = (DspFloatType)Pvolume / 127.0f;
};



void
Vibe::setpreset (int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 8;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Classic
        {35, 120, 10, 0, 64, 64, 64, 64, 3, 64, 0},
        //Stereo Classic
        {35, 120, 10, 0, 48, 64, 64, 64, 3, 64, 1},
        //Wide Vibe
        {127, 80, 10, 0, 0, 64, 64, 64, 0, 64, 1},
        //Classic Chorus
        {35, 360, 10, 0, 48, 64, 0, 64, 3, 64, 0},
        //Vibe Chorus
        {75, 330, 10, 0, 50, 64, 0, 64, 17, 64, 0},
        //Lush Chorus
        {55, 260, 10, 0, 64, 70, 0, 49, 20, 48, 0},
        //Sick Phaser
        {110, 75, 10, 0, 32, 64, 64, 14, 0, 30, 1},
        //Warble
        {127, 360, 10, 0, 0, 64, 0, 0, 0, 37, 0}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(45,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

};

void
Vibe::changepar (int npar, int value)
{

    switch (npar) {

    case 0:
        Pwidth = value;
        fwidth = ((DspFloatType) Pwidth)/90.0f;
        break;
    case 1:
        lfo.Pfreq = value;
        lfo.updateparams ();
        break;
    case 2:
        lfo.Prandomness = value;
        lfo.updateparams ();
        break;
    case 3:
        lfo.PLFOtype = value;
        lfo.updateparams ();
        break;
    case 4:
        lfo.Pstereo = value;
        lfo.updateparams ();
        break;
    case 5: // pan
        setpanning(value);
        break;
    case 6:
        setvolume(value);
        break;
    case 7: //fb
        Pfb = value;
        fb = ((DspFloatType) (Pfb - 64))/65.0f;
        break;
    case 8: //depth
        Pdepth = value;
        fdepth = ((DspFloatType) Pdepth)/127.0f;
        break;
    case 9: //lrcross
        Plrcross = value;
        flrcross = ((DspFloatType) (Plrcross - 64))/64.0f;
        fcross = 1.0f - fabs(flrcross);
        break;
    case 10: //Stereo
        Pstereo = value;
        break;

    }

};

int
Vibe::getpar (int npar)
{

    switch (npar)

    {
    case 0:
        return (Pwidth);
        break;
    case 1:
        return (lfo.Pfreq);
        break;
    case 2:
        return (lfo.Prandomness);
        break;
    case 3:
        return (lfo.PLFOtype);
        break;
    case 4:
        return (lfo.Pstereo);
        break;
    case 5:
        return (Ppanning); //pan
        break;
    case 6:
        return(Pvolume);
        break;
    case 7:
        return(Pfb);
        break;
    case 8:
        return(Pdepth);
        break;
    case 9:
        return(Plrcross);
        break;
    case 10: //Stereo
        return(Pstereo);
        break;


    }

    return (0);

};

Vocoder::Vocoder (DspFloatType * efxoutl_, DspFloatType * efxoutr_, DspFloatType *auxresampled_,int bands, int DS, int uq, int dq)
{

    adjust(DS);

    VOC_BANDS = bands;
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;
    auxresampled = auxresampled_;
    //default values
    Ppreset = 0;
    Pvolume = 50;
    Plevel = 0;
    Pinput = 0;
    Ppanning = 64;
    Plrcross = 100;

    filterbank = (fbank *) malloc(sizeof(fbank) * VOC_BANDS);
    tmpl = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);
    tmpr = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);
    tsmpsl = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);
    tsmpsr = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);
    tmpaux = (DspFloatType *) malloc (sizeof (DspFloatType) * nPERIOD);



    Pmuffle = 10;
    DspFloatType tmp = 0.01f;  //10 ms decay time on peak detectors
    alpha = ncSAMPLE_RATE/(ncSAMPLE_RATE + tmp);
    beta = 1.0f - alpha;
    prls = beta;
    gate = 0.005f;


    tmp = 0.05f; //50 ms att/rel on compressor
    calpha =  ncSAMPLE_RATE/(ncSAMPLE_RATE + tmp);
    cbeta = 1.0f - calpha;
    cthresh = 0.25f;
    cpthresh = cthresh; //dynamic threshold
    cratio = 0.25f;


    DspFloatType center;
    DspFloatType qq;

    A_Resample = new Resample(dq);
    U_Resample = new Resample(dq);
    D_Resample = new Resample(uq);


    for (int i = 0; i < VOC_BANDS; i++) {
        center = (DspFloatType) i * 20000.0f/((DspFloatType) VOC_BANDS);
        qq = 60.0f;

        filterbank[i].l = new AnalogFilter (4, center, qq, 0);
        filterbank[i].l->setSR(nSAMPLE_RATE);
        filterbank[i].r = new AnalogFilter (4, center, qq, 0);
        filterbank[i].r->setSR(nSAMPLE_RATE);
        filterbank[i].aux = new AnalogFilter (4, center, qq, 0);
        filterbank[i].aux->setSR(nSAMPLE_RATE);
    };

    vlp = new AnalogFilter (2, 4000.0f, 1.0f, 1);
    vhp = new AnalogFilter (3, 200.0f, 0.707f, 1);

    vlp->setSR(nSAMPLE_RATE);
    vhp->setSR(nSAMPLE_RATE);

    setbands(VOC_BANDS, 200.0f, 4000.0f);
    setpreset (Ppreset);

};

Vocoder::~Vocoder ()
{
};

/*
 * Cleanup the effect
 */
void
Vocoder::cleanup ()
{
    for(int k=0; k<VOC_BANDS; k++) {
        filterbank[k].l->cleanup();
        filterbank[k].r->cleanup();
        filterbank[k].aux->cleanup();
        filterbank[k].speak = 0.0f;
        filterbank[k].gain = 0.0f;
        filterbank[k].oldgain = 0.0f;

    }
    vhp->cleanup();
    vlp->cleanup();

    compeak = compg = compenv = oldcompenv = 0.0f;

};


void
Vocoder::adjust(int DS)
{

    DS_state=DS;


    switch(DS) {

    case 0:
        nPERIOD = PERIOD;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD*96000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD*48000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD*44100.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD*32000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD*22050.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD*16000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD*12000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD*8000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD*4000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        break;

    }

    ncSAMPLE_RATE = 1.0f / nfSAMPLE_RATE;

    u_up= (DspFloatType)nPERIOD / (DspFloatType)PERIOD;
    u_down= (DspFloatType)PERIOD / (DspFloatType)nPERIOD;


}




/*
 * Effect output
 */
void
Vocoder::out (DspFloatType * smpsl, DspFloatType * smpsr)
{
    int i, j;

    DspFloatType tempgain;
    DspFloatType maxgain=0.0f;
    DspFloatType auxtemp, tmpgain;


    if(DS_state != 0) {
        A_Resample->mono_out(auxresampled,tmpaux,PERIOD,u_up,nPERIOD);
    } else
        memcpy(tmpaux,auxresampled,sizeof(DspFloatType)*nPERIOD);


    for (i = 0; i<nPERIOD; i++) {  //apply compression to auxresampled
        auxtemp = input * tmpaux[i];
        if(fabs(auxtemp > compeak)) compeak = fabs(auxtemp);   //First do peak detection on the signal
        compeak *= prls;
        compenv = cbeta * oldcompenv + calpha * compeak;       //Next average into envelope follower
        oldcompenv = compenv;

        if(compenv > cpthresh) {                              //if envelope of signal exceeds thresh, then compress
            compg = cpthresh + cpthresh*(compenv - cpthresh)/compenv;
            cpthresh = cthresh + cratio*(compg - cpthresh);   //cpthresh changes dynamically
            tmpgain = compg/compenv;
        } else {
            tmpgain = 1.0f;
        }



        if(compenv < cpthresh) cpthresh = compenv;
        if(cpthresh < cthresh) cpthresh = cthresh;

        tmpaux[i] = auxtemp * tmpgain;

        tmpaux[i]=vlp->filterout_s(tmpaux[i]);
        tmpaux[i]=vhp->filterout_s(tmpaux[i]);

    };


    //End compression

    auxtemp = 0.0f;

    if(DS_state != 0) {
        U_Resample->out(smpsl,smpsr,tsmpsl,tsmpsr,PERIOD,u_up);
    } else {
        memcpy(tsmpsl,smpsl,sizeof(DspFloatType)*nPERIOD);
        memcpy(tsmpsr,smpsr,sizeof(DspFloatType)*nPERIOD);
    }


    memset(tmpl,0,sizeof(DspFloatType)*nPERIOD);
    memset(tmpr,0,sizeof(DspFloatType)*nPERIOD);



    for (j = 0; j < VOC_BANDS; j++) {

        for (i = 0; i<nPERIOD; i++) {
            auxtemp = tmpaux[i];

            if(filterbank[j].speak < gate) filterbank[j].speak = 0.0f;  //gate
            if(auxtemp>maxgain) maxgain = auxtemp; //vu meter level.

            auxtemp = filterbank[j].aux->filterout_s(auxtemp);
            if(fabs(auxtemp) > filterbank[j].speak) filterbank[j].speak = fabs(auxtemp);  //Leaky Peak detector

            filterbank[j].speak*=prls;

            filterbank[j].gain = beta * filterbank[j].oldgain + alpha * filterbank[j].speak;
            filterbank[j].oldgain = filterbank[j].gain;


            tempgain = (1.0f-ringworm)*filterbank[j].oldgain+ringworm*auxtemp;

            tmpl[i] +=filterbank[j].l->filterout_s(tsmpsl[i])*tempgain;
            tmpr[i] +=filterbank[j].r->filterout_s(tsmpsr[i])*tempgain;

        };


    };


    for (i = 0; i<nPERIOD; i++) {
        tmpl[i]*=lpanning*level;
        tmpr[i]*=rpanning*level;
    };


    if(DS_state != 0) {
        D_Resample->out(tmpl,tmpr,efxoutl,efxoutr,nPERIOD,u_down);
    } else {
        memcpy(efxoutl,tmpl,sizeof(DspFloatType)*nPERIOD);
        memcpy(efxoutr,tmpr,sizeof(DspFloatType)*nPERIOD);
    }

    vulevel = (DspFloatType)CLAMP(rap2dB(maxgain), -48.0, 15.0);



};

void
Vocoder::setbands (int numbands, DspFloatType startfreq, DspFloatType endfreq)
{
    DspFloatType start = startfreq;   //useful variables
    DspFloatType endband = endfreq;
    DspFloatType fnumbands = (DspFloatType) numbands;
    DspFloatType output[VOC_BANDS + 1];
    int k;

    //calculate intermediate values
    DspFloatType pwer = logf(endband/start)/log(2.0f);

    for(k=0; k<=VOC_BANDS; k++) output[k] = start*f_pow2(((DspFloatType) k)*pwer/fnumbands);
    for(k=0; k<VOC_BANDS; k++) {
        filterbank[k].sfreq = output[k] + (output[k+1] - output[k])*0.5f;
        filterbank[k].sq = filterbank[k].sfreq/(output[k+1] - output[k]);

        filterbank[k].l->setfreq_and_q (filterbank[k].sfreq, filterbank[k].sq);
        filterbank[k].r->setfreq_and_q (filterbank[k].sfreq, filterbank[k].sq);
        filterbank[k].aux->setfreq_and_q (filterbank[k].sfreq, filterbank[k].sq);
    }
    cleanup();

}

/*
 * Parameter control
 */
void
Vocoder::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (DspFloatType)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Vocoder::setpanning (int Ppanning)
{
    this->Ppanning = Ppanning;
    lpanning = ((DspFloatType)Ppanning + 0.5f) / 127.0f;
    rpanning = 1.0f - lpanning;
};


void
Vocoder::init_filters()
{
    DspFloatType ff, qq;

    for (int ii = 0; ii < VOC_BANDS; ii++) {
        ff = filterbank[ii].sfreq;
        qq = filterbank[ii].sq;
        filterbank[ii].l->setfreq_and_q (ff, qq);
        filterbank[ii].r->setfreq_and_q (ff, qq);
        filterbank[ii].aux->setfreq_and_q (ff, qq);
    };

}

void
Vocoder::adjustq(DspFloatType q)
{

    for (int ii = 0; ii < VOC_BANDS; ii++) {
        filterbank[ii].l->setq (q);
        filterbank[ii].r->setq (q);
        filterbank[ii].aux->setq (q);
    };

}

void
Vocoder::setpreset (int npreset)
{
    const int PRESET_SIZE = 7;
    const int NUM_PRESETS = 4;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Vocoder 1
        {0, 64, 10, 70, 70, 40, 0},
        //Vocoder 2
        {0, 64, 14, 80, 70, 40, 32},
        //Vocoder 3
        {0, 64, 20, 90, 70, 40, 64},
        //Vocoder 4
        {0, 64, 30, 100, 70, 40, 127}
    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(35,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;
};


void
Vocoder::changepar (int npar, int value)
{
    DspFloatType tmp = 0;
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setpanning (value);
        break;
    case 2:
        Pmuffle = value;
        tmp = (DspFloatType) Pmuffle;
        tmp *= 0.0001f + tmp/64000;
        alpha = ncSAMPLE_RATE/(ncSAMPLE_RATE + tmp);
        beta = 1.0f - alpha;
        break;
    case 3:
        Pqq = value;
        tmp = (DspFloatType) value;
        adjustq(tmp);
        break;
    case 4:
        Pinput = value;
        input = dB2rap (75.0f * (DspFloatType)Pinput / 127.0f - 40.0f);
        break;
    case 5:
        Plevel = value;
        level = dB2rap (60.0f * (DspFloatType)Plevel / 127.0f - 40.0f);
        break;

    case 6:
        Pring = value;
        ringworm = (DspFloatType) Pring/127.0f;
        break;
    };
};

int
Vocoder::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppanning);
        break;
    case 2:
        return(Pmuffle);
        break;
    case 3:
        return(Pqq);
        break;
    case 4:
        return (Pinput);
        break;
    case 5:
        return (Plevel);
        break;
    case 6:
        return (Pring);
        break;

    };
    return (0);			//in case of bogus parameter number
};


beattracker:: beattracker ()
{

    rmsfilter = new RBFilter (0, 15.0f, 0.15f, 1);
    peaklpfilter = new RBFilter (0, 25.0f, 0.5f, 0);
    peaklpfilter2 = new RBFilter (0, 25.0f, 0.5f, 0);
    peakhpfilter = new RBFilter (1, 45.0f, 0.5f, 0);

    index = (int *) malloc (sizeof (int) * PERIOD);

//Trigger Filter Settings
    peakpulse = peak = envrms = 0.0f;
    peakdecay = 10.0f/fSAMPLE_RATE;
    targatk = 12.0f/fSAMPLE_RATE;   ///for smoothing filter transition
    atk = 200.0f/fSAMPLE_RATE;
    trigtime = SAMPLE_RATE/12; //time to take next peak
    onset = 0;
    trigthresh = 0.15f;

    tscntr = 0;
    tsidx = 0;

    oldbpm = 0.0f;
    oldmost = 0.0f;

    cleanup ();

};

beattracker::~beattracker ()
{

};

void
beattracker::cleanup ()
{

    peakpulse = peak = envrms = 0.0f;
    peakdecay = 10.0f/fSAMPLE_RATE;
    targatk = 12.0f/fSAMPLE_RATE;   ///for smoothing filter transition
    atk = 200.0f/fSAMPLE_RATE;
    trigtime = SAMPLE_RATE/20; //time to take next peak
    onset = 0;
    trigthresh = 0.15f;
    oldbpm = 0.0f;
    oldmost = 0.0f;
    tscntr = 0;
    tsidx = 0;
    for( int i = 0; i<17; i++) {
        avbpm[i] = ( ((DspFloatType) i) + 0.5f) * 10.0f;  //center the initial average in the bin
        statsbin[i] = 1.0f;  //set all to 1.0 and let fly from there
    }
    statsbin[12] = 1.1f;  //slightly bias to 120bpm
    maxptr = 12;
    oldbpm = 120.0f;
    bpm_change_cntr = 0;
};

void
beattracker::detect (DspFloatType * smpsl, DspFloatType * smpsr)
{
    DspFloatType tmp;
    int idx = 0;
    int i = 0;

    for ( i = 0; i < PERIOD; i++) { //Detect dynamics onset
        index[i] = 0; //initializes all elements to zero

        tmp = 15.0f*fabs(smpsl[i] + smpsr[i]);
        envrms = rmsfilter->filterout_s(tmp);
        if ( tmp > peak) peak =  atk + tmp;
        if ( envrms < peak) peak -= peakdecay;
        if(peak<0.0f) peak = 0.0f;

        peakpulse = peaklpfilter2->filterout_s(fabs(peakhpfilter->filterout_s(peak)));


        if( peakpulse > trigthresh ) {
            if (trigtimeout==0) {
                index[idx] = i;
                timeseries[tsidx] = tscntr;
                tsidx++;
                tsidx%=20;
                tscntr = 0;
                idx++;

                trigtimeout = trigtime;

                //calculate tempo on every beat detected
                calc_tempo();
            }

        } else {
            if (--trigtimeout<0) {
                trigtimeout = 0;
            }

        }
        tscntr++;
    }


};
/*
DspFloatType
beattracker::get_tempo()  //returns tempo in DspFloatType beats per minute
{
if((oldbpm>600.0f) || (oldbpm<0.0f)) oldbpm = 0.0f;
DspFloatType time = 0;
if(tsidx>0) time = ((DspFloatType) timeseries[tsidx-1])/fSAMPLE_RATE;
else  time = ((DspFloatType) timeseries[19])/fSAMPLE_RATE;
//below really needs to be more clever to pull out 1/8, 1/4, 1/2, triplets or other time signatures.
//for now it's good enough to work with relatively steady patterns in 4/4
DspFloatType divisor = 0;
int intdiv = 0;
if(time<1.0f) {
intdiv = lrintf(1.0f/time);
divisor = (DspFloatType) intdiv;
time*=divisor;
}
else {
intdiv = lrintf(time);
divisor = 1.0f/((DspFloatType) intdiv);
time*=divisor;
}
DspFloatType bpm = 120.0f/time;
//below is some code to look at the outputs, a crude statistical analysis
//oldbpm = 0.95f*oldbpm + 0.05f*bpm;
/
//oldbpm = avg;
//avg = oldmost;
DspFloatType low = avg*0.95f;
DspFloatType high = avg*1.05f;
if ( (bpm>low) && (bpm<high) ) {
oldmost = 0.1f*oldmost + 0.9f*bpm;
}
//
//uncomment below to see what is happening
//printf("time: %f bpm: %f avg: %f most: %f time: 1/%f \n", time, bpm, oldbpm, oldmost,divisor);
return(bpm);
};
*/

void
beattracker::calc_tempo()  //returns tempo in DspFloatType beats per minute
{
    if((oldbpm>600.0f) || (oldbpm<0.0f)) oldbpm = 0.0f;

    DspFloatType time = 0;
    if(tsidx>0) time = ((DspFloatType) timeseries[tsidx-1])/fSAMPLE_RATE;
    else  time = ((DspFloatType) timeseries[19])/fSAMPLE_RATE;

    DspFloatType bpm = 30.0f/time;

    while(bpm < 80.0f) bpm*=2.0f;
    while(bpm > 160.0f) bpm*=0.5f;  //normalize by powers of 2 to 80<bpm<160

    int binptr = lrintf(floorf(0.1f*bpm));  //get a bin address
    statsbin[binptr] += 1.0f;  //increase count
    avbpm[binptr] = 0.6f*avbpm[binptr] + 0.4f*bpm;
    if(avbpm[binptr] < 0.1f) avbpm[binptr] = 0.01f;  //create a floor to avoid denormal computations

    DspFloatType maxbin = 0.0f;
    maxptr = binptr;
    for(int i = 8; i<15; i++) {
        statsbin[i] *= 0.9f;  //weight 80 through 160 bins
        if(statsbin[i] > maxbin) {
            maxbin = statsbin[i];
            maxptr = i;
        }

    }

    if(fabs(oldbpm - avbpm[maxptr]) > 10.0f) {//prevent bpm from changing too abruptly and frequently
        if( ++bpm_change_cntr > 4) {
            oldbpm = avbpm[maxptr];  //wait 4 beats before allowing change > 10 bpm
            bpm_change_cntr = 0;
        }
    } else oldbpm = avbpm[maxptr];

};

DspFloatType
beattracker::get_tempo()
{

    return(oldbpm);
};