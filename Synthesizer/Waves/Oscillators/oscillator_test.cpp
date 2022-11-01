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

// conflict with freeverb
//#include "Resampler.hpp"
#include "Resamplers.hpp"
#include "Decimators.hpp"

#include "Amplifiers.hpp"
#include "ClipFunctions.hpp"
#include "Modulation.hpp"
#include "FunctionGenerator.hpp"
#include "FunctionLFO.hpp"
#include "Envelopes.hpp"


#include "ADSR.h"

// Filters
#include "Filters.h"


//#include "Filters.hpp"
// cppfilters.hpp
// Filters.h = RBJ
// Nigelquad.h
// Massberg.h

//#include "Moog.h"
//#include "moog.hpp"
//#include "MoogFilter1.h"
//#include "MoogFilter2.h"
//#include "MoogFilterI.h"
//#include "MoogFilterII.h"
//#include "MoogHalfLadder.hpp"
//#include "MoogLadder.hpp"
//#include "MoogLadderFilter.hpp"
//#include "MoogLike.hpp"
//#include "MoogVCF.hpp"
//#include "moogladder.h"
//#include "StilsonMoog.hpp"
//#include "MS20.hpp"
//#include "FaustFilters.h"
//#include "HybridCurtisVCF.h";
//#include "StateVariableFilter.h"
//#include "StateVariableFilters.hpp"

// Will Pickles synthlab
// CAnalog.hpp = Will PIckles
// vafilters.hpp
// fxobjects.h



/*  Oscillators + Noise
#include "PolyBLEP2x.hpp"
#include "minBLEP.hpp"
#include "minBLEP2x.hpp"
#include "Blit.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "BlitTriangle.h"
#include "Blit2x.h"
#include "BlitSaw2x.h"
#include "BlitSquare2x.h"
#include "BlitTriangle2x.h"

 FunctionGenerators
 FunctionLFO
 LFO
 FM/PM WaveTable
 WaveTable
 Fourier Additive WaveTable
 Wave
 WaveVector
 WaveMatrix
 WaveCycles
*/

// Effects
#include "AudioEffectsSuite.hpp"
#include "AudioDSP_Delay.h"
#include "AudioDSP_DelayLine.h"
#include "AudioDSP_Lfo.h"
#include "Chorus.hpp"
#include "StereoDelay.hpp"
#include "Flanger.hpp"

#define ITERATE(index,start,end) for(size_t index = start; index < end; index += 1)
#define STEP(index,start,end,step) for(size_t index = start; index < end; index += step)

Std::RandomMersenne noise;

double sampleRate = 44100.0f;
double sampleRate2 = 44100.0f/2.0f;
double invSampleRate = 1.0/sampleRate;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Threads
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <unistd.h>

struct Mutex {
    pthread_mutex_t mutex;

    Mutex() {
        pthread_mutex_init(&mutex,NULL);
    }
    ~Mutex() {
        pthread_mutex_destroy(&mutex);
    }

    void lock() {
        pthread_mutex_lock(&mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }
};


typedef void* (*ThreadFunction)(void*);
struct Thread
{        
    pthread_t thread;

    Thread(ThreadFunction cb,void * data) { 
        pthread_create(&thread,NULL,cb,data);            
    }
    ~Thread() {            
        pthread_join(thread,NULL);            
    }
    void Start() {
        
    }
    void Stop() {
        
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double freq_to_midi(double f) {
    return 12.0*log2(f/440.0) + 69;
}
double midi_to_freq(double m) {
    return pow(2.0, (m-69)/12)*440.0;
}

double semitone(int semi, double f)
{
    double m = freq_to_midi(f);
    return midi_to_freq(m + semi);
}
double octave(int octave, double f) {
    double m = freq_to_midi(f);
    return midi_to_freq(m + octave*12);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "minBLEP.hpp"
#include "PolyBLEP2x.hpp"





class Blit
{
 public:
  //! Default constructor that initializes BLIT frequency to 220 Hz.
  Blit( double frequency = 220.0 );

  //! Class destructor.
  ~Blit();

  //! Resets the oscillator state and phase to 0.
  void reset();

  //! Set the phase of the signal.
  /*!
    Set the phase of the signal, in the range 0 to 1.
  */
  void setPhase( double phase ) { phase_ = M_PI * phase; };

  //! Get the current phase of the signal.
  /*!
    Get the phase of the signal, in the range [0 to 1.0).
  */
  double getPhase() const { return phase_ / M_PI; };

  //! Set the impulse train rate in terms of a frequency in Hz.
  void setFrequency( double frequency );

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
  void setHarmonics( unsigned int nHarmonics = 0 );
  
  //! Compute and return one output sample.
  double Tick( double I = 0, double A = 1, double X = 0, double Y = 0);

 protected:

    double Sawtooth();
    double Square();
    double Triangle();

  void updateHarmonics( void );

  RBJFilter *lowpass;
  Decimateur5 decimator[4];

  unsigned int nHarmonics_;
  unsigned int m_;
  double rate_;
  double phase_;
  double p_;
  double y;
  double saw,square,triangle;
  double duty;
  double freq;
  double C2_;
  double a_;
  double state_;
  double lastBlitOutput_;
  double lastBPBlitOutput_;
  double dcbState_;
  
};

inline double Blit::Sawtooth( void )
{  
  double tmp = lastBlitOutput_;
  tmp += state_ - C2_;
  state_ = tmp * 0.995;  
  return tmp;
}

inline double Blit::Square( void )
{
    

    // A fully  optimized version of this would replace the two sin calls
    // with a pair of fast sin oscillators, for which stable fast 
    // two-multiply algorithms are well known. In the spirit of STK,
    // which favors clarity over performance, the optimization has 
    // not been made here.
    
    // Now apply DC blocker.
    double temp = lastBPBlitOutput_ - dcbState_ + 0.999 * y;
    dcbState_   = lastBPBlitOutput_;    
    return temp;
}

inline double Blit:: Triangle()
{
    double period = (2*sampleRate)/freq;    
    double g = 0.99f / (period *(duty)*(1-duty));
    float out = triangle + g * (square + duty);
    return out;
}

inline double Blit :: Tick(double I, double A, double X, double Y)
{
  // The code below implements the SincM algorithm of Stilson and
  // Smith with an additional scale factor of P / M applied to
  // normalize the output.

  // A fully optimized version of this code would replace the two sin
  // calls with a pair of fast sin oscillators, for which stable fast
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has not
  // been made here.

  // Avoid a divide by zero at the sinc peak, which has a limiting
  // value of 1.0.
    double tmp, denominator = sin( phase_ );
    if ( fabs(denominator) <= std::numeric_limits<double>::epsilon() )
        tmp = 1.0;
    else {
        tmp =  sin( m_ * phase_ );
        tmp /= m_ * denominator;
    }

    double old = lastBPBlitOutput_;
    

    if ( fabs( denominator )  < std::numeric_limits<double>::epsilon() ) {

    // Inexact comparison safely distinguishes betwen *close to zero*, and *close to M_PI*.
    if ( phase_ < 0.1f || phase_ > 2*M_PI - 0.1f )
        lastBPBlitOutput_ = a_;
    else
        lastBPBlitOutput_ = -a_;
    }
    else {
        lastBPBlitOutput_ =  sin( m_ * phase_ );
        lastBPBlitOutput_ /= p_ * denominator;
    }

    lastBPBlitOutput_ += old;

    phase_ = fmod(phase_ + rate_, M_PI);
    

    lastBlitOutput_ = tmp;

    saw      = decimator[0].Calc(saw,Sawtooth());
    square   = decimator[1].Calc(square,Square());
    triangle = decimator[2].Calc(triangle,Triangle());
    y = tmp;
    return saw;
}



inline Blit:: Blit( double frequency )
{
  lowpass = new RBJFilter(RBJFilter::FilterType::LOWPASS,15000.0f,2*sampleRate);
  nHarmonics_ = 0;  
  dcbState_ = 0.0;
  lastBlitOutput_ = 0;
  duty = 0.5;  
  this->setFrequency( frequency );
  this->reset();
}

inline Blit :: ~Blit()
{
    if(lowpass) delete lowpass;
}

inline void Blit :: reset()
{
  phase_ = 0.0;
  y = 0.0;
}

inline void Blit :: setFrequency( double frequency )
{

  freq = frequency;
  p_ = (2*sampleRate) / frequency;
  C2_ = 1 / p_;
  rate_ = M_PI / p_;
  this->updateHarmonics();
}

inline void Blit :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;  
  this->updateHarmonics();
  state_ = -0.5 * a_;
}

inline void Blit :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

  a_ = m_ / p_;
}


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

struct Wave
{
    double * waveTable;
    size_t  tableSize;
    double   f,fs,phase;
    
    Wave(size_t n, double sr) {
        waveTable = new double[tableSize = n];
        fs = sr;
        phase = 0;
        
    }
    ~Wave() {
        if(waveTable) delete [] waveTable;
    }
    void setFrequency(double freq) {
        f = freq/fs;
    }
    void setPhase(double p) {
        phase = std::fmod(p,1);
    }
    void makeSine(double fn) {        
        double inc   = 1.0f/tableSize;
        for(size_t i = 0; i < tableSize; i++)
        {
            waveTable[i] = std::sin(2.0*M_PI*(fn/fs+(double)i*inc));
            
        }        
    }
    double operator[](double phaseIn) {
        int   index = phaseIn*tableSize;
        double x1 = waveTable[index];
        double frac = x1 - index;
        int   index2 = (index+1) % tableSize;
        double x2 = waveTable[index2];                
        double out =  (1.0-frac)*x1 + frac*x2;        
        return out;
    }
    double Tick(double I=1, double A=1, double X=0, double Y=0)
    {
        double p = phase;
        phase = std::fmod(I*(f+X) + Y,1);

        int   index = int(phase*tableSize) % tableSize;
        double x1 = waveTable[index];
        double frac = x1 - index;
        int   index2 = (index+1) % tableSize;
        double x2 = waveTable[index2];                
        double out =  (1.0-frac)*x1 + frac*x2;
        phase = std::fmod(p + f, 1);
        return A*out;
    }
    
};

struct AdditiveWave
{
    Wave *wave;
    double * F;
    double * A;
    double * P;
    size_t  N;
    double fs;

    AdditiveWave(size_t n, double sr, size_t tableSize=4096)
    {
        wave = new Wave(tableSize, sr);
        wave->makeSine(1.0f);
        fs = sr;
        F = new double[n];
        A = new double[n];
        P = new double[n];        
        for(size_t i = 0; i < n; i++) {
            F[i] = noise.rand();
            A[i] = noise.rand();
            P[i] = noise.rand();
        }
        N = n;
    }
    ~AdditiveWave() {
        if(wave) delete wave;
        if(F) delete [] F;
        if(A) delete [] A;
        if(P) delete [] P;
    }
    double Tick(double I=1, double Gain=1, double X=0, double Y=0)
    {
        double r = 0;
        for(size_t i = 0; i < N; i++)
        {
            double f = F[i];
            double p = P[i];
            P[i] = std::fmod(F[i] + X,1);
            r += A[i]*(*wave)[P[i]+Y];
            F[i] = f;
            P[i] = p;
        }
        return Gain*r/N;
    }
};

ADSR        adsr(0.01,0.3,0.5,0.2,44100.0f);
Wave        osc(4096,sampleRate);
minBLEP     minblep(440.0f,sampleRate);
PolyBLEP2X  polyblep(sampleRate,PolyBLEP2X::SAWTOOTH,440.0f);
StereoDelay delay;
MoogVCF     vcf(sampleRate,1000,0);
Blit        blit(440.0f);

int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{    
    
    float * output = (float*)outputBuffer;
    float * input  = (float*)inputBuffer;
    sample_vector<double> buffer(framesPerBuffer);
    double temp[2][1024];
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        buffer[i] = vcf.Tick(adsr.Tick() * blit.Tick());
        //buffer[i] = adsr.Tick() * blit.Tick();
    }    
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        temp[0][i] = buffer[i];
        temp[1][i] = buffer[i];
    }
    /*
    delay.processStereo((double*)&temp[0][0],(double*)&temp[1][0],framesPerBuffer);
    */
    float* p = output;
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        *p++ = temp[0][i];
        *p++ = temp[1][i];
    }
    //buffer = DSP::dcremove(buffer,0.00025f);
    //memcpy(output, buffer.data(), framesPerBuffer * sizeof(double));
    
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

void repl() {

}


int main()
{
    //set_audio_func(audio_callback);
    Init();

    noise.seed_engine();        
    int num_midi = GetNumMidiDevices();
    ITERATE(i,0,num_midi)
    {
        printf("midi device #%lu: %s\n", i, GetMidiDeviceName(i));
    }
    int num_audio = GetNumAudioDevices();
    int pulse = 10;

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
    
    InitMidiDevice(1,5,5);
    InitAudioDevice(pulse,pulse,2,44100,256);
    RunAudio();
    StopAudio();
}

