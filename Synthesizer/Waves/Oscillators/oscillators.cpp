#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <random>
#include <chrono>
#include <list>
#include <pthread.h>
#include <unistd.h>


#include "audiosystem.h"
#include "Undenormal.hpp"
#include "Std/StdObject.h"
#include "Std/StdRandom.h"
#include "samples/kfr_sample.hpp"
#include "samples/kfr_sample_dsp.hpp"
#include "SndFile.hpp"

#include "Resamplers.hpp"
#include "Decimators.hpp"

#include "Amplifiers.hpp"
#include "ClipFunctions.hpp"
#include "Modulation.hpp"
#include "FunctionGenerator.hpp"
#include "FunctionLFO.hpp"
#include "Envelopes.hpp"
#include "ADSR.h"
#include "Filters.h"

// Effects
#include "AudioEffectsSuite.hpp"
#include "AudioDSP_Delay.h"
#include "AudioDSP_DelayLine.h"
#include "AudioDSP_Lfo.h"
#include "Chorus.hpp"
#include "Flanger.hpp"

#include "MusicFunctions.hpp"
#include "Threads.hpp"

#define ITERATE(index,start,end) for(size_t index = start; index < end; index += 1)
#define STEP(index,start,end,step) for(size_t index = start; index < end; index += step)

Std::RandomMersenne noise;

double sampleRate = 44100.0f;
double sampleRate2 = 44100.0f/2.0f;
double invSampleRate = 1.0/sampleRate;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "minBLEP.hpp"
#include "PolyBLEP2x.hpp"

//////////////////////////////////////////////
// Blit Experiment
// Square = blit(x) - blit(x+duty)
//////////////////////////////////////////////


struct BlitSaw
{
    //! Class constructor.
    BlitSaw( double frequency = 220.0 ) : lowpass(RBJFilter::LOWPASS,sampleRate/2,2*sampleRate)
    {
        nHarmonics_ = 0;
        offset = 0;
        reset();
        setFrequency( frequency );
        block.setFc(10.0f/sampleRate);
        gain = 1;
    }

    //! Class destructor.
    ~BlitSaw() = default;

    //! Resets the oscillator state and phase to 0.
    void reset()
    {
        phase_ = 0.0f;
        state_ = 0.0;
        y = 0.0;
    }

    //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
    void setFrequency( double frequency )
    {
        p_ = (2*sampleRate) / frequency;
        C2_ = 1 / p_;
        rate_ = M_PI * C2_;
        updateHarmonics();
    }

    //! Set the number of harmonics generated in the signal.
    /*!
    This function sets the number of harmonics contained in the
    resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
    algorithm.  The default value of 0 sets the algorithm for maximum
    harmonic content (harmonics up to half the sample rate).  This
    parameter is not checked against the current sample rate and
    fundamental frequency.  Thus, aliasing can result if one or more
    harmonics for a given fundamental frequency exceeds fs / 2.  This
    behavior was chosen over the potentially more problematic solution
    of automatically modifying the M parameter, which can produce
    audible clicks in the signal.
    */
    void setHarmonics( unsigned int nHarmonics = 0 )
    {
        nHarmonics_ = nHarmonics;
        this->updateHarmonics();

        // I found that the initial DC offset could be minimized with an
        // initial state setting as given below.  This initialization should
        // only happen before starting the oscillator for the first time
        // (but after setting the frequency and number of harmonics).  I
        // struggled a bit to decide where best to put this and finally
        // settled on here.  In general, the user shouldn't be messing with
        // the number of harmonics once the oscillator is running because
        // this is automatically taken care of in the setFrequency()
        // function.  (GPS - 1 October 2005)
        state_ = -0.5 * a_;
    }

    
    
    //! Return the last computed output value.
    double lastOut( void ) const { return y; };

    void setGain(float g) {
        gain = g;
    }
    //! Compute and return one output sample.
    double Tick( void )
    {
        //double x = phase_;
        //phase_ += offset;
        double tmp, denominator = sin( phase_ );
        if ( fabs(denominator) <= std::numeric_limits<double>::epsilon() )
            tmp = a_;
        else {
            tmp =  sin( m_ * phase_ );
            tmp /= p_ * denominator;
        }

        tmp += state_ - C2_;
        state_ = tmp * 0.995;
        //phase_   = x;
        phase_ += rate_;
        if ( phase_ >= M_PI ) phase_ -= M_PI;

        float out = lowpass.Tick(clamp(gain*tmp,-1,1));            
        y = decimator.Calc(out,y);        
        y -= block.process(y);
        return y;
    }

    float getPhase() { return phase_; }

    void setPhaseOffset(float o) {
        phase_ = o;    
    }

    void updateHarmonics( void )
    {
        if ( nHarmonics_ <= 0 ) {
            unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
            m_ = 2 * maxHarmonics + 1;
        }
        else
            m_ = 2 * nHarmonics_ + 1;

        a_ = m_ / p_;
    }

    OnePole     block;
    RBJFilter   lowpass;
    Decimateur5 decimator;
    unsigned int nHarmonics_;
    unsigned int m_;
    double rate_;
    double phase_;
    double offset;
    double p_;
    double C2_;
    double a_;
    double state_;
    double y;
    double gain;
};
struct BlitSquare
{
    OnePole block;
    BlitSaw s1,s2;
    float _out = 0;
    float _duty = 0.5;

    BlitSquare()
    {
        block.setFc(10.0f/sampleRate);
        _out = 0;
        _duty = 0.5;
        setFrequency(440.0f);
        s1.setGain(1);
        s2.setGain(1);
    }
    void setFrequency(float f)
    {
        s1.setFrequency(f);
        s2.setFrequency(f);        
    }
    void setDuty(float d)
    {
        _duty = d;
    }
    void reset() {
        s1.reset();
        s2.reset();
    }
    float Tick() {
        float r1 = s1.Tick();        
        s2.setPhaseOffset(s1.getPhase() + _duty*M_PI);
        float r2 = s2.Tick();                
        _out = r2-r1;
        double x = _out;
        x -= block.process(x);
        return 4*x;
    }
};
struct BlitTriangle
{
    OnePole b1,b2;    
    BlitSquare s1;
    float _out = 0;
    
    BlitTriangle() 
    {
        b1.setFc(10.0f/sampleRate);
        b2.setFc(10.0f/sampleRate);
        setFrequency(440);
    }
    void setFrequency(float f)
    {        
        s1.setFrequency(f);                
    }
    void setDuty(float d)
    {    
        s1.setDuty(d);
    }
    void reset() {
        s1.reset();
        _out = 0;
    }
    float Tick() {
        float r1 = s1.Tick();                        
        // there's a tremendous amount of weird dc noise in this thing
        r1   -= b1.process(r1);
        // not really sure why it works but it does I think m_ = harmonic * harmonic like the fourier expansion
        _out += (r1/s1.s1.m_);                
        float x = 2*_out;        
        return x-b2.process(x);
    }
};

//////////////////////////////////////////////
// Some moogs
//////////////////////////////////////////////
struct MoogVCF
{
    //Init
    double fc;
    double fs;
    double res;
    double out1,out2,out3,out4;
    double in1,in2,in3,in4;
    
    MoogVCF(double sr, double Fc, double R)
    {
        fs = sr;
        fc = Fc/sr;
        res= R;
        out1=out2=out3=out4=0;
        in1=in2=in3=in4=0;
    }
    void setCutoff(double f) { fc = f/fs; }
    void setResonance(double r) { res = r; }

    double Tick(double I, double A=1, double X=0, double Y=0) {
        double f = fc * 1.16;
        double fb = res * (1.0 - 0.15 * f * f);
        double input = I;
        input -= out4 * fb;
        input *= 0.35013 * (f*f)*(f*f);
        out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
        in1  = input;
        out2 = out1 + 0.3 * in2 + (1 - f) * out2;  // Pole 2
        in2  = out1;
        out3 = out2 + 0.3 * in3 + (1 - f) * out3;  // Pole 3
        in3  = out2;
        out4 = out3 + 0.3 * in4 + (1 - f) * out4;  // Pole 4
        in4  = out3;
        return out4;
    }
};
struct MoogFilter2
{
    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    double f, p, q;             //filter coefficients
    double b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
    double t1, t2;              //temporary buffers
    double fs,fc,res;

    // Set coefficients given frequency & resonance [0.0...1.0]
    MoogFilter2(double sr, double cutoff, double r)
    {
        fs = sr;
        fc = cutoff/sr;
        res = r;
        calc();
        b0=b1=b2=b3=b4=0;
    }
    void calc()
    {
        q = 1.0f - fc;
        p = fc + 0.8f * fc * q;
        f = p + p - 1.0f;
        q = res * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
    }
    void setCutoff(double f) { fc = f/fs; }
    void setResonance(double r) { res = r; }
    double Tick(double I, double A=1, double X = 0, double Y=0)
    {
        Undenormal denormals;
        calc();
        double in = I - q*b4;       
        t1 = b1; //std::tanh(b1);  
        b1 = (in + b0) * p - b1 * f;
        t2 = b2; //std::tanh(b2);  
        b2 = (b1 + t1) * p - b2 * f;
        t1 = b3; //std::tanh(b3); 
        b3 = (b2 + t2) * p - b3 * f;
        b4 = (b3 + t1) * p - b4 * f;
        b4 = b4 - b4 * b4 * b4 * 0.166667f;
        b0 = in;
        return b4;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Generators
////////////////////////////////////////////////////////////////////////////////////////////////
struct SinewaveGenerator
{
    float phase;
    float f,sr;
    SinewaveGenerator(float freq, float sample_rate) {
        phase = 0;
        f     = freq/sample_rate;
        sr    = sample_rate;        
    }
    float Tick(float I=1,float A=1,float X=1, float Y=1)
    {
        float x = A * std::sin(2*M_PI*std::fmod(I*((f+X) + phase+Y),1));
        phase = std::fmod(phase + f,1);
        return x;

    }
};
struct SquarewaveGenerator
{
    float phase;
    float f,sr,duty;
    SquarewaveGenerator(float freq, float sample_rate) {
        phase = 0;
        f     = freq/sample_rate;
        sr    = sample_rate;        
        duty  = 0.5;
    }
    float Tick(float I=1,float A=1,float X=1, float Y=1)
    {
        float r;
        float d = duty;
        float p = phase;
        phase = std::fmod(phase + X,1);
        duty = std::fmod(duty + Y,1);
        if(phase < duty) r = 0;
        else r = 1;
        phase = p;
        duty  = d;
        phase = std::fmod(phase + f,1);
        return r;
    }
};
struct SawtoothGenerator
{
    float phase;
    float f,sr;
    SawtoothGenerator(float freq, float sample_rate) {
        phase = 0;
        f     = freq/sample_rate;
        sr    = sample_rate;                
    }
    float Tick(float I=1,float A=1,float X=1, float Y=1)
    {
        float r = phase;                
        phase = std::fmod(phase + X + Y,1);        
        phase = r;        
        phase = std::fmod(phase + f,1);
        return r;
    }
};
struct ReverseSawtoothGenerator
{
    float phase;
    float f,sr;
    ReverseSawtoothGenerator(float freq, float sample_rate) {
        phase = 0;
        f     = freq/sample_rate;
        sr    = sample_rate;                
    }
    float Tick(float I=1,float A=1,float X=1, float Y=1)
    {
        float r = phase;                
        phase = std::fmod(phase + X + Y,1);        
        phase = r;        
        phase = std::fmod(phase + f,1);
        return 1.0-r;
    }
};

///////////////////////////////////////////////////////
// Wave
// Load Samples from files
// Resample on pitch change
///////////////////////////////////////////////////////
struct Wave
{
    float * waveTable;
    size_t  tableSize;
    float   f,fs,phase;
    
    Wave(size_t n, float sr) {
        waveTable = new float[tableSize = n];
        fs = sr;
        phase = 0;
        
    }
    ~Wave() {
        if(waveTable) delete [] waveTable;
    }
    void setFrequency(float freq) {
        f = freq/fs;
    }
    void setPhase(float p) {
        phase = std::fmod(p,1);
    }
    void makeSine() {        
        float inc   = 1.0f/tableSize;
        for(size_t i = 0; i < tableSize; i++)
        {
            waveTable[i] = std::sin(2.0*M_PI*i*inc);            
        }        
    }
    float operator()(float phaseIn) {
        int   index = int(phaseIn*tableSize) % tableSize;
        float x1 = waveTable[index];
        float frac = x1 - floor(x1);
        int   index2 = (index+1) % tableSize;
        float x2 = waveTable[index2];                
        float out =  x1 + frac*(x2-x1);        
        return out;
    }
    float Inc(float phaseIn) {
        return (*this)(phaseIn);
    }
    float Tick(float I=1, float A=1, float X=0, float Y=0)
    {
        float p = phase;
        //phase = std::fmod(I*(f+X) + Y,1);
        int   index = int(phase*tableSize) % tableSize;
        float x1 = waveTable[index];
        float frac = x1 - floor(x1);
        int   index2 = (index+1) % tableSize;
        float x2 = waveTable[index2];                
        float out = x1 + (frac*(x2-x1));
        phase = std::fmod(phase + f, 1);
        return A*out;
    }    
};
struct MultiWave
{
    // large number of sine waves with different frequencies/amplitudes
};
struct MultiTable
{
    // multiple wavetables with amplitude (scale is number of waves)
};
struct AdditiveWave
{
    Wave  * wave;
    float * F;
    float * A;
    float * P;
    size_t  N;
    float fs;
    float f0 = 440.0f;
    float Amp = 1;
    float Offset = 0;
    bool Bipolar = false;

    AdditiveWave(size_t n, float sr, size_t tableSize=4096)
    {
        wave = new Wave(tableSize, sr);
        wave->makeSine();
        fs = sr;
        F = new float[n];
        A = new float[n];
        P = new float[n];        
        for(size_t i = 0; i < n; i++) {
            F[i] = (i+1);
            A[i] = 0; //float(n-i)/float(n);
            P[i] = 0;
        }
        A[0] = 1.0f;
        f0 = 440.0f;
        N = n;
    }
    ~AdditiveWave() {
        if(wave) delete wave;
        if(F) delete [] F;
        if(A) delete [] A;
        if(P) delete [] P;
    }
    void setSine() {
        memset(A,0,sizeof(float)*N);
        A[0] = 1.0f;
        Offset =0;
        Amp = 1;
        Bipolar=false;
    }
    void setSaw() {        
        for(size_t i = 1; i < N; i++)            
        {
            F[i-1] = i;
            A[i-1] = pow(-1,i)/(float)i;            
            P[i-1] = 0;
        }
        Amp = 1;
        Offset=0;
        Bipolar=false;
    }
    void setSquare()
    {
        memset(A,0,sizeof(float)*N);
        for(size_t i = 1; i < N; i+=2)
        {
            F[i-1] = i;
            A[i-1] = (4.0f/M_PI)*1.0f/(float)i;
            P[i-1] = 0;
        }
        Amp = 4.0f/M_PI;
        Offset = 0;
        Bipolar=false;
    }
    void setPulse(float duty)
    {
        memset(A,0,sizeof(float)*N);
        for(size_t i = 1; i < N; i++)
        {
            F[i] = i;
            A[i] = sin(M_PI*(float)i*duty)/(M_PI*(float)i*duty);
            P[i] = 0.25;
        }
        Amp = 2*duty;
        Offset = duty;
        Bipolar=true;
    }
    void setTriangle()
    {                
        memset(A,0,sizeof(float)*N);            
        for(size_t i = 0; i < N; i++)
        {      
            float n = (2*i)+1;       
            F[i] = n;                     
            A[i] = pow(-1.0f,i)/float(n*n);                             
            P[i] = 0;
        }         
        Amp = 8/(M_PI*M_PI);
        Offset = 0;
        Bipolar=false;
    }
    void setRandom()
    {
        for(size_t i = 1; i < N; i++)
            A[i] = noise.rand()/i;
    }
    void setFrequency(float f) {
        f0 = f;
    }
    float Tick(float I=1, float Gain=1, float X=0, float Y=0)
    {
        float r = 0;
        for(size_t i = 0; i < N; i++)
        {
            float f = f0*F[i];
            float p = P[i];            
            if(f > fs/2) continue;
            r += A[i]*wave->Inc(p);
            P[i] = fmod(P[i]+f/fs,1);
        }
        float out = Gain*(Offset + Amp*r);
        if(Bipolar) return 2*out-1;
        return out;
    }    
};
// Wave has the richest waveform ( sawtooth at 1 hz)
// Filter and resample when pitch changes
struct WaveResampler
{
    sample_vector<float> cycle;   
    sample_vector<float> table; 
    Decimateur5 decimator;
    RBJFilter lowpass;
    float phase,freq,inc;
    float sr;
    float last;
        
    WaveResampler(float sampleRate) : lowpass(RBJFilter::FilterType::LOWPASS,15000,sampleRate*2)
    {
        sr    = 2*sampleRate;
        cycle.resize(4096);                
        phase = 0.0f;          
        last  = 0;
    }
    void setFrequency(float f) {
        freq = f;
        inc   = freq/sr;                
    }
    float Tick(float I=1,float A=1,float X=0,float Y=0) {
        float r = phase*cycle.size();
        int   i = (int)r;        
        float v1 = cycle[i];
        float frac = v1 - floor(v1);
        int   n = (i+1) % cycle.size();
        float v2 = cycle[n];        
        phase = std::fmod(phase + inc,1);
        float x = v1 + frac*(v2-v1);  
        /*
        float q = 0;
        for(size_t i = 1; i < 20; i+=2)
            q  += std::sin(2.0f*M_PI*i*phase)/(float)i;
        x = (4.0/M_PI)*q;
        */
        
        x = lowpass.Tick(x);        
        float out = decimator.Calc(last,x);
        last = x;
        return out;
    }
};

// Sampler   = WaveResampler + Filters + Envelopes
// WaveCycle = resampled wave 
// WaveCycleVector = Cycle[N]
// WaveCycleMatrix = Cycle[M][N]


struct DPWSaw
{
    float freq,fs,inc;
    float phase,lastPhase;
    float lastValue,position;
    float scaleFactor;
    DPWSaw()
    {
        freq = 440.0f;
        fs   = sampleRate;
        inc  = freq/fs;
        lastValue = phase = lastPhase = position = 0.0f;
        scaleFactor = sampleRate / (4.0f * freq);
    }
    void setFrequency(float f) {
        freq = f;
        inc  = f/fs;
        scaleFactor = sampleRate / (4.0f * freq);
    }
    float Tick()
    {                                    
        position += phase - lastPhase;
        lastPhase = phase;

        position = fmod(position, 1.0f);

        float value = position * 2 - 1;
        value = value * value;
        
        float out = scaleFactor * (value - lastValue);
        lastValue = value;

        phase = fmod(phase + inc,1.0f);
        return out;
    }   
};

struct DPWPulse
{
    float freq,fs,inc;
    float phase,lastPhase;
    float lastValueA,lastValueB,position;
    float positionA,positionB;
    float scaleFactor;

    DPWPulse()
    {
        freq = 440.0f;
        fs   = sampleRate;
        inc  = freq/fs;
        lastValueA = lastValueB = phase = lastPhase = position = 0.0f;
        positionA = 0.5f;
        positionB = 0.5f;
        scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
        phase = 0.5;
    }
    void setFrequency(float f) {
        freq = f;
        inc  = f/fs;
        scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
    }
    void setDuty(float d) {
        phase = clamp(d,0.01f,0.99f);
    }
    float Tick() {
                        
        positionB += phase - lastPhase;
        lastPhase = phase;

        positionA = fmod(positionA, 1.0f);
        positionB = fmod(positionB, 1.0f);

        float valueA = positionA * 2.0f - 1.0f;
        float valueB = positionB * 2.0f - 1.0f;
        valueA = valueA * valueA;
        valueB = valueB * valueB;
        float out = ((valueA - lastValueA) -(valueB - lastValueB)) * scaleFactor;
        lastValueA = valueA;
        lastValueB = valueB;

        positionA += freq * invSampleRate;
        positionB += freq * invSampleRate;

        return out;        
    }
};

struct DPWTriangle
{
    float freq,fs,inc;
    float phase,lastPhase;
    float lastValue,position;    
    float scaleFactor;

    DPWTriangle()
    {
        freq = 440.0f;
        fs   = sampleRate;
        inc  = freq/fs;
        lastValue = phase = lastPhase = position = 0.0f;
        position = 0.0f;        
        scaleFactor =  sampleRate / (2.0f * freq);
        phase = 0.5;
    }
    void setFrequency(float f) {
        freq = f;
        inc  = f/fs;
        scaleFactor =  sampleRate / (2.0f * freq);
    }
    void setDuty(float d) {
        phase = clamp(d,0.01f,0.99f);
    }
    float Tick()
    {        
        position += phase - lastPhase;
        lastPhase = phase;
        position = fmod(position, 1.0f);                
        float out = std::abs(position - 0.5) * 4 - 1;                
        position += freq * invSampleRate;        
        return out;
    }
};

ADSR        adsr(0.01,0.3,0.5,0.2,44100.0f);
Wave        osc(4096,sampleRate);
AdditiveWave        awave(64,sampleRate);
minBLEP     minblep(440.0f,sampleRate);
PolyBLEP2X  polyblep(sampleRate,PolyBLEP2X::SAWTOOTH,440.0f);
MoogVCF     vcf(sampleRate,1000,0);
BlitTriangle  blit;
DPWSaw saw;
DPWPulse pulse;
DPWTriangle tri;

#include "StereoDelay.hpp"
#include "BasicDelay.h"
BasicDelay delay(sampleRate,2,500,0.9,0.6);
StereoDelay stereo;

int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{    
    
    float * output = (float*)outputBuffer;
    float * input  = (float*)inputBuffer;
    float * p = output;
    float temp[2][1024];
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        float in = minblep.Tick();
        temp[0][i] = in;
        temp[1][i] = in;
    }
    //delay.processBlock(0,framesPerBuffer,output);
    //delay.processBlock(1,framesPerBuffer,output);
    //stereo.processStereo(temp[0],temp[1],framesPerBuffer);
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        
        *p++ = temp[0][i];
        *p++ = temp[1][i];
    }
    return 0;
}            


void midi_msg_print(MidiMsg * msg) {
    printf("%d %d %d\n",msg->msg,msg->data1,msg->data2);
}

double last_frequency;
double last_q=0;
double fcutoff=0;

void note_on(MidiMsg * msg) {
    double freq = midi_to_freq(msg->data1);
    double velocity = msg->data2/127.0f;
    
    osc.setFrequency(freq);
    awave.setFrequency(freq);
    blit.setFrequency(freq);
    saw.setFrequency(freq);
    pulse.setFrequency(freq);
    tri.setFrequency(freq);
    polyblep.setFrequency(freq);
    vcf.setCutoff(freq);
    minblep.setFrequency(freq);
    blit.setFrequency(freq);
    adsr.noteOn();            
    last_frequency = freq;
}

void note_off(MidiMsg * msg) {
    double freq = midi_to_freq(msg->data1);
    double velocity = msg->data2/127.0f;
    adsr.noteOff();
}

void control_change(MidiMsg * msg) {
    midi_msg_print(msg);
    if(msg->data1 == 102)
    {
        double fc = sampleRate*std::log(1 + (double)msg->data2/127.0f)/std::log(2);
        fcutoff=fc;
        vcf.setCutoff(fc);
    }
    if(msg->data1 == 103)
    {
        double q = std::log(1 + (double)msg->data2/127.0f)/std::log(2);
        vcf.setResonance(q);
        last_q   = q;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lua REPL
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LuaJIT.hpp"
LuaJIT * lua;

int test(lua_State *L)
{
    printf("test\n");
    return 0;
}

void connectLua()
{
    lua = new LuaJIT("main.lua");
    lua->CreateCFunction("test",test);
}

void repl() {
    std::string cmd;
    std::cin >> cmd;
    lua->DoCmd(cmd);
}


int main()
{
    //set_audio_func(audio_callback);
    Init();
    
    connectLua();
    noise.seed_engine();     
    osc.makeSine();   
    awave.setTriangle();

    stereo.setDelay(StereoDelay::Channel::LEFT,250);
    stereo.setDelay(StereoDelay::Channel::RIGHT,250);
    stereo.setFeedback(StereoDelay::Channel::LEFT,90);
    stereo.setFeedback(StereoDelay::Channel::RIGHT,90);
    stereo.setMix(StereoDelay::Channel::LEFT,50);
    stereo.setMix(StereoDelay::Channel::RIGHT,50);
    
    
    int num_midi = GetNumMidiDevices();
    ITERATE(i,0,num_midi)
    {
        printf("midi device #%lu: %s\n", i, GetMidiDeviceName(i));
    }
    int num_audio = GetNumAudioDevices();
    int pulse = 9;

    ITERATE(i, 0, num_audio)    
    {
        if(!strcmp(GetAudioDeviceName(i),"pulse")) pulse = i;
        printf("audio device #%lu: %s\n", i, GetAudioDeviceName(i));
    }
    
    set_note_on_func(note_on);
    set_note_off_func(note_off);
    set_audio_func(audio_callback);
    set_repl_func(repl);
    set_control_change_func(control_change);
    
    InitMidiDevice(1,3,-1);
    InitAudioDevice(pulse,pulse,2,44100,256);
    RunAudio();
    StopAudio();
}

