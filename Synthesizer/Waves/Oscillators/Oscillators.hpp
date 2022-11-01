/*
 * Oscillators
 * sin
 * Fourier Additive
 * Cordic
 * Quadrature complex sin/cos
 * Direct
 * Cycle = 1 wave zero-zero + resampler
 * CycleVector = WaveTable = Cycle[M]
 * CycleMatrix = Cycle[M][N]
 * WaveTable = WaveCycle[N]
 * WaveVector = WaveTable[M]
 * WaveMatrix = WaveTable[M][N]
 * Granulator/Graunular
 * Stk Blit
 * MinBLEP
 * PolyBLEPS
 * Discrete Summation Formulas
 * Walsh Synthesis
 * Additive Saw Synthesis
 * saw * inverted saw (abs) = pwm
 * square * triangle = ?
 * saw * square = triangle
 * saw * triangle = ?
 * Additive Square
 * Stk Physical Models
 * Stk FM
 * Gamma 
 * DaisySP
 * Thelonious
 * SoundTailor
 * SoundPipe
 * SoundKit 
 * JackSynth
 * Lupo
 * libfmsynth
 * granular synths
 * dx7
 * graph-fm
 */
#pragma once

#include <cmath>

// curve     = 1/1.5*((atan(2*sigmoid(x)*pi-2))+0.3)


// sawlike   = erf(tan(2*pi*x))
// squarish  = erf(1/sin(2*pi*x))
// compressed= erf(sin(2*pi*x))
// soft clip = erf(2*asin(x))
// soft clip = erf(2*atan(x))
// soft clip = sin(atan(pi*x))
// hard clip = sin(atan(pi*4*x))
// harder clip = erf(2*atan(2*x))
// tanh clip = erf(tanh(x))
// sinh clip = etf(sinh(x))
// clip = erf((2*x) / (1 + abs(x)))
// squashed = erf((2*x) / (1 + abs(10*x)))*4.9
// sigmoid  = erf(2*sigmoid(10*x)-1)
// hard clip = erf*2*asin(10*x)

// crazy = sin(tan(pi*x*2) + tan(pi*x*4))
// half rectifiedish = sin(2*pi*x)**4
// slow curve = tanh(2*log(abs(x+1)))
// hard curve = tanh(10*log(abs(x+1)))
// half = abs(sin(2*pi*x))
// rectifiedish = sin(2*pi*x)*abs(sin(2*pi*x))

// sin(2*pi*sin(2*pi*sin(2*pi*x)))
// sin distortions = 2*(tanh(log(abs(sin(2*PI*sin(2*PI*sin(x)))+1)

struct SineWave
{
    float f,sr,phase,wc;
    float bias;

    SineWave(float freq, float fs = 44100.0f) {
        f  = freq;
        sr = fs;
        wc = freq/fs;
        phase = 0.0f;
        bias = 0;
    }
    void SetFrequency(float freq) {
        f = freq;
        wc = f/fs;
    }
    void SetPhase(float p) {
        while(p < 0) p += 1.0f;
        while(p > 1) p -= 1.0f;
        phase = p;
    }
    T Tick(float I=1,float A=1,float X=0, float Y=0) {
        float r = std::sin(2*M_PI*I*(f*(X+0.5)+(phase*Y)));
        phase = std::fmod(phase + wc,1.0f);
        return A*r + bias;
    }
};

struct CheapSine
{
    float r;
    float p;

    CheapSine(float rate=0.01f) : r(rate), p(0 { }

    T Tick(float I, float A = 1, float X = 0, float Y = 0)
    {
        p += r + Y;
        if(p > 1) p -= 2;
        float out = p*(1-std::abs(X+p));
        p -= Y;
        return A*p;
    }
};


struct WidthSine
{
    float frq;
    float fs;
    float fb;
    float sine,saw,pulse;
    float f,pw,pm;
    float bias;
    WidthSine(float freq, float sr = 44100.0f) : frq(freq),fs(sr)) {
        f= freq/sr;
        fb = 1.0f;
        pw = 0.5;
        pm = 0.5;
        bias = 0;
    }

    float sinewave(float t, float ph, float fm) 
    {
        float afact = 1 - std::sin(2*M_PI*f)
        std::sin((t*f+ph)*2*M_PI + 0.5*M_PI*fm*afact);
    }
    float Tick(float I=1, float A=1, float fm = 0, float ph = 0) {
        //afact(f) = 1 - sin(f*2PI)
        //t := time (0 to ...)
        //ph := phase shift (0 to 1)
        //fm := freq mod (0 to 1)
        // sine(t,f,ph,fm) = sin((t*f+ph)*2PI + 0.5PI*fm*afact(f))
        // fb := feedback (0 to 1) (1 max saw)
        //saw(t,f,ph,fm,fb) = sine(t,f,ph,fb*sine(t-1,f,ph,fm))
        //pm := pulse mod (0 to 1) (1 max pulse)
        //pw := pulse width (0 to 1) (1 square)
        //pulse(t,f,ph,fm,fb,pm,pw) = saw(t,f,ph,fm,fb) - (t,f,ph+0.5*pw,fm,fb) * pm
        
        sine     = sinewave(t,ph,fm);
        saw      = sinewave(t,ph,fb*sine(t-1,ph,fm));
        pulse    = sinewave(t-1,ph,fm) - fb*sinewave(t,ph+0.5*pw,fm) * pm;
        return A*sinewave(I*t+bias,ph,fm);
    }
    float Bias(float I, float B, float A=1, float fm = 0, float ph = 0) {
        float t = bias;
        bias = B;
        float r = TIck(I,A,fm,ph);
        bias = t;
        return r;
    }
    float Polar(float I, float B, float A=1, float fm = 0, float ph = 0) {
        if(I < 0) return -Tick(I,A,fm,ph);
        return Tick(I,A,fm,ph);
    }
    float Circular(float I, float A, float X, float Y) {
        return Tick(I,A,A*std::cos(X),A*std::cos(Y));
    }
    float Exponential(float I, float A, float X, float Y) {        
        float e1 = std::exp(1);
        return Tick(I,A,A*std::exp(X)/e1,A*std::exp(Y)/e1);
    }
    float Quadratic(float I, float A, float B, float C)
    {
        if(A == 0) return Tick(I,A,B,C);
        if(B == 0) return Tick(I,A,B,C);
        float x = -B - std::sqrt(B*B-4*A*C)/(2*A);
        float y = A + std::sqrt(A*A-4*B*C)/(2*B);
        return Tick(I,A,x,y);
    }
};

struct BlitSaw
{

/* Leaky integrator/first order lowpass which 
 * shapes the impulse train into a nice
 * -6dB/octave spectrum
 *
 * The cutoff frequency needs to be pretty lowish
 * or the sawtooth will suffer phase distortion
 * at the low end.
 */

    typedef struct
    {
        double x1, y1;
        double a, b;
    } lowpass_t;

    /* initializes a lowpass, sets cutoff/leakiness */

    void init_lowpass(lowpass_t *lp, double cutoff)
    {
        double Omega;

        lp->x1 = lp->y1 = 0.0;

        Omega = std::atan(pi * cutoff);
        lp->a = -(1.0 - Omega) / (1.0 + Omega);
        lp->b = (1.0 - lp->b) / 2.0;
    }

    double update_lowpass(lowpass_t *lp, double x)
    {
        double y;

        y = lp->b * (x + lp->x1) - lp->a * lp->y1;

        lp->x1 = x;
        lp->y1 = y;

        return y;
    }

    double phase;    /* phase accumulator */
    double aNQ;      /* attenuation at nyquist */
    double curcps;   /* current frequency, updated once per cycle */
    double curper;   /* current period, updated once per cycle */
    lowpass_t leaky; /* leaky integrator */
    double N;        /* # partials */
    double a;        /* dsf parameter which controls roll-off */
    double aN;       /* former to the N */


    /* initializes a blit structure
    *
    * The aNQ parameter is the desired attenuation
    * at nyquist. A low value yields a duller
    * sawtooth but gets rid of those annoying CLICKS
    * when sweeping the frequency up real high. |aNQ|
    * must be strictly less than 1.0! Find a setting
    * which works for you.
    *
    * The cutoff parameter controls the leakiness of
    * the integrator.
    */

    BlitSaw(double aNQ, double cutoff)
    {
        phase  = 0.0;
        aNQ = aNQ;
        curcps = 0.0;
        curper = 0.0;
        init_lowpass(&leaky, cutoff);
    }

    /* Returns a sawtooth computed from a leaky integration
    * of a DSF bandlimited impulse train.
    *
    * cps (cycles per sample) is the fundamental
    * frequency: 0 -> 0.5 == 0 -> nyquist
    */

    double update_blit(double cps)
    {
        double P2, beta, Nbeta, cosbeta, n, d, blit, saw;
        
         if(phase >= 1.0 || curcps == 0.0)
            {
            /* New cycle, update frequency and everything
            * that depends on it
            */

            if(phase >= 1.0)
                phase -= 1.0;

            curcps = cps;        /* this cycle\'s frequency */
            curper = 1.0 / cps;  /* this cycle\'s period */

            P2 = curper / 2.0;
            N = 1.0 + std::floor(P2); /* # of partials incl. dc */

            /* find the roll-off parameter which gives
            * the desired attenuation at nyquist
            */

            a   = std::pow(aNQ, 1.0 / P2);
            aN  = std::pow(a,   N);
            }

        beta = twopi * phase;

        Nbeta = N * beta;
        cosbeta = std::cos(beta);

        /* The dsf blit is scaled by 1 / period to give approximately the same
        * peak-to-peak over a wide range of frequencies.
        */

        n = 1.0 -
            aN * std::cos(Nbeta) -
            a * (cosbeta - aN * std::cos(Nbeta - beta));
        d = curper * (1.0 + a * (-2.0 * cosbeta + a));

        phase += curcps; /* update phase */

        blit = n / d - curcps; /* This division can only fail if |a| == 1.0
            * Subtracting the fundamental frq rids of DC
            */

        saw = update_lowpass(&leaky, blit); /* shape blit spectrum into a saw */

        return saw;
    }

    
    float Tick(float I=1, float A=1,float Fm=1,float Pm=1)
    {
        float p = phase;
        float f = curcps;
        float cps = f + f*Fm;      
        phase = phase + Pm;  
        float r = A*update_blit(cps);
        phase = p + f;
        curcps = f;

        return r;
    }
    /* ?
    float Square()
    {
        float p = phase;        
        float pwm = 0.5;
        phase = p + M_PI/2.0*pwm;
        float isaw = 1.0+Tick();        
        phase = p;
        float saw = Tick();
        float r = saw + isaw;
        return r - 1.0;
    }
    float Triangle()
    {
        float p = phase;
        float saw = Tick();        
        phase = p;
        float square = Tick();
        return saw * square;        
    }
    */
};

/* DSF
Discrete Summation Formula ala Moorer

computes equivalent to sum{k=0:N-1}(a^k * sin(beta + k*theta))
modified from Emanuel Landeholm's C code
output should never clip past [-1,1]

If using for BLIT synthesis for virtual analog:
N = maxN;
a = attn_at_Nyquist ^ (1/maxN); %hide top harmonic popping in and out when sweeping
frequency
beta = pi/2;
num = 1 - a^N * cos(N*theta) - a*( cos(theta) - a^N * cos(N*theta - theta) ); %don't waste
time on beta

You can also get growing harmonics if a > 1, but the min statement in the code must be
removed, and the scaling will be weird.

code

function output = dsf( freq, a, H, samples, beta)
%a = rolloff coeffecient
%H = number of harmonic overtones (fundamental not included)
%beta = harmonic phase shift

samplerate = 44.1e3;
freq = freq/samplerate; %normalize frequency

bandlimit = samplerate / 2; %Nyquist
maxN = 1 + floor( bandlimit / freq ); %prevent aliasing
N = min(H+2,maxN);

theta = 2*pi * phasor(freq, samples);

epsilon = 1e-6;
a = min(a, 1-epsilon); %prevent divide by zero

num = sin(beta) - a*sin(beta-theta) - a^N*sin(beta + N*theta) + a^(N+1)*sin(beta+(N-1)*theta);
den = (1 + a * ( a - 2*cos(theta) ));

output = 2*(num ./ den - 1) * freq; %subtract by one to remove DC, scale by freq to normalize
output = output * maxN/N;           %OPTIONAL: rescale to give louder output as rolloff increases

function out = phasor(normfreq, samples);
out = mod( (0:samples-1)*normfreq , 1);
out = out * 2 - 1;                  %make bipolar
*/

double DSF (double x,  // input
            double a,  // a<1.0
            double N,  // N<SmplFQ/2,
            double fi) // phase
{
  double s1 = std::pow(a,N-1.0)*std::sin((N-1.0)*x-fi);
  double s2 = std::pow(a,N)*std::sin(N*x+fi);
  double s3 = a*std::sin(x+fi);
  double s4 =1.0 - (2*a*std::cos(x)) +(a*a);
  if (s4==0)
     return 0;
  else
     return (std::sin(fi) - s3 - s2 +s1)/s4;
}