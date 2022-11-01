
#include "KfrDsp.hpp"
#include "Filters.hpp"

using namespace DSP1;


#define ITERATE(index,start,end) for(size_t index = start; index < end; index += 1)
#define STEP(index,start,end,step) for(size_t index = start; index < end; index += step)

Std::RandomMersenne noise;

float sampleRate = 44100.0f;
float sampleRate2 = 44100.0f/2.0f;
float invSampleRate = 1.0/sampleRate;


/*
template<int N>
struct KfrDelay
{
    kfr::delay<float,N> delay;    
}
*/

/*
#include "spuce/allpass.h"

namespace Spuce {
    using AllpassFilter = spuce::allpass<double>;
}
*/


// now in minBLEP.hpp 
// if you bandlimit saw it is a phasor is it still antialised?
#include "minblep.h"
#include "minblep.c"


struct FirstOrder
{
    double fc,sr,q;
    double x,y,x1,y1;

    FirstOrder(double Fs, double Fc, double Q)
    {
        fc = Fc/Fs;
        sr = Fs;
        q  = Q;                
    }
    void setCutoff(float f) {
        fc = f/sr;                   
    }
    void setQ(float Q) {        
        q = Q;        
    }
    double operator()(double I, double A = 1, double X = 0, double Y = 0) {
        return Tick(I,A,X,Y);
    }
    double Tick(double I, double A=1, double X = 0, double Y = 0)
    {
        
        //a0 = 1.0;
        //a1 = -2.0 * r * std::cos(M_PI * newFcutoff);
        double ft = fc;
        fc = fmod(fc + X + Y,1);
        double K  = tan(M_PI*fc);          
        double a0 = (1+K);
        double a1 = (1-K)/a0;
        double b0 = K/a0;
        double b1 = K/a0;        
        x = I; //saturate(I,0.9,2);
        y = b0*x + b1*x1 - a1*y1;
        x1 = x; //saturate(x,0.95,1.2);
        y1 = y; //saturate(y,0.95,1.2);
        fc = ft;
        return A*y; //saturate(y,0.9,2);
    }
};

struct Cascade18db
{
    FirstOrder f1,f2,f3;

    Cascade18db(double Fs, double Fc, double Q) :
    f1(Fs,Fc,Q),
    f2(Fs,Fc,Q),
    f3(Fs,Fc,Q)
    {
        
    }
    void setCutoff(float f) {                              
        f1.setCutoff(f);
        f2.setCutoff(f);
        f3.setCutoff(f);
    }
    void setQ(float q) {
        f1.setQ(q);
        f2.setQ(q);
        f3.setQ(q);
    }
    double operator()(double I, double A=1, double X = 0, double Y=0) {
        return Tick(I,A,X,Y);
    }
    double Tick(double I, double A=1, double X = 0, double Y = 0)
    {
        
        //double r = 1 - f3.q * std::sin(M_PI * f3.fc);
        //double out = f3(f2(f1(I - r*f1.y1) - r*f2.y1) - r*f3.y1);
        //out = f3(I)*f2(I)*f2(I);
        //out = f3(I)+f2(I)+f2(I);                
        double out = f3(f2(f1(I,A,X,Y),A,X,Y),A,X,Y);        
        return out;
    }
};

class mertIIR
{
public:
    // Public methods
    mertIIR(float Fs, float Fc, float Q);
    ~mertIIR();
    float Tick(float newSample);
    void updateCoefficients(double newFcutoff, double newRes);
    void setCutoff(float f) {
        fc = f/sr;
    }
    void setQ(float Q) {
        q = 1-Q;
    }
    double fc,q,sr;
    // Public member variables
    double b0, b1, b2;
    double a0, a1, a2;
    double s1, s2;

private:
};

// Class constructor - just intializes coeffecients and filter memory
mertIIR::mertIIR(float Fs, float Fc, float Q)
{
    b0 = 0.0;
    b1 = 0.0;
    b2 = 0.0;
    
    a0 = 0.0;
    a1 = 0.0;
    a2 = 0.0;
    
    s1 = 0.0;
    s2 = 0.0;

    sr = Fs;
    fc = Fc/Fs;
    q  = 1-Q;
}

mertIIR::~mertIIR()
{
    
}

// Sample-wise implementation of transposed direct-form II IIR
float mertIIR::Tick(float newSample)
{
    Undenormal denormal;
    updateCoefficients(fc,q);
    double out = newSample * b0 + s1;
    s1 = newSample * b1 + s2 - a1 * out;
    s2 = newSample * b2 - a2 * out;
    return out;
}

// Lowpass filter - second-order resonator
void mertIIR::updateCoefficients(double newFcutoff, double newRes)
{
    // Determine poles
    double r = 1 - newRes * std::sin(M_PI * newFcutoff);
    a0 = 1.0;
    a1 = -2.0 * r * std::cos(M_PI * newFcutoff);
    a2 = r*r;

    // Place two zeros at z = 1
    b0 = 1.0;
    b1 = 2.0;
    b2 = 1.0;
    
    // Scale for unity gain at DC.
    // A better approach might be to ensure unity gain at fcutoff
    double dcGain = (b0 + b1 + b2)/(a0 + a1 + a2);
    b0 /= dcGain;
    b1 /= dcGain;
    b2 /= dcGain;
}

struct Phasor
{
    double phase,f,sr,duty,g;
    int type;

    enum {
        Sin,
        Cos,
        Tan,
        Cot,
        Sec,
        Csc,
        Saw,
        Square,
        Pulse,
        RSaw,
        Triangle,
        Noise
    };
    enum Polarity {
        Positive,
        Negative,
        Bipolar,
    }
    polarityType = Positive;

    Phasor(float Fs, float F, float _g = 1, int _type = Sin )
    {
        sr = Fs;
        f  = F/Fs;
        phase = 0;
        duty = 0.5;
        type = _type;
        g = _g;
    }
    void setFrequency(float F) {
        f = F /sr;
    }
    double increment()
    {
        double r = phase;
        phase = fmod(phase+f,1);
        return r;
    }
    double saw() {
        return phase;
    }
    double rsaw() {
        return 1 - phase;
    }
    double pulse() {
        if(phase < duty) return 0;
        return 1;
    }
    double square() {
        if(phase < 0.5) return 0;
        return 1;
    }
    double triangle() {
        return 0.5 - 0.5*asin(sin(2*M_PI*phase + duty));
    }
    double tangent() {
        if(phase == 0.5) return 0;
        return 0.5 - 0.5*tan(M_PI*phase);
    }
    double cotangent() {
        return 1.0/tangent();
    }
    double sine() {
        return 0.5 - 0.5*sin(2*M_PI*phase);
    }
    double cosecant() {
        double x = sine();
        if( x == 0) return 0;
        return 1.0/x;
    }
    double cosine() {
        return 0.5 - 0.5*cos(2*M_PI*phase);
    }
    double secant() {
        double x = cosine();
        if(x == 0.0) return 0;
        return 1.0/x;
    }
    double noise() {
        return (double)rand() / (double)RAND_MAX;
    }    
    double operator()() {
        switch(type)
        {
            case Sin: return sine();
            case Cos: return cosine();
            case Tan: return tangent();
            case Cot: return cotangent();
            case Sec: return secant();
            case Csc: return cosecant();
            case Saw: return saw();
            case RSaw: return rsaw();
            case Square: return square();
            case Pulse: return pulse();
            case Triangle: return triangle();
            case Noise: return noise();
        }
        return sine();
    }
    double get() {
        return (*this)();
    }
    double Tick(double I = 1, double A = 1, double X = 0, double Y = 0)
    {
        double p = phase;
        phase = I*(f+X) + phase + Y;
        double r = g*get();        
        phase = p;        
        increment();
        if(polarityType == Positive) r = std::abs(r);
        else if(polarityType == Negative) r = -r;
        else r = 2*r-1;
        return A*r;
    }
};


Biquad6DB  filter6db(FilterType::Lowpass,44100.0f,1000.0f);
Biquad12DB lowpass12(FilterType::Lowpass,44100.0f,1000.0f);
Biquad24DB lowpass24(FilterType::Allpass,44100.0f,1000.0f);
Biquad36DB lowpass36(FilterType::Allpass,44100.0f,1000.0f);
Biquad48DB lowpass46(FilterType::Allpass,44100.0f,1000.0f);

BesselFilter         bessel(FilterType::Lowpass,1,44100.0f,1000.0f);
ButterworthFilter    butterworth(FilterType::Lowpass,1,44100.0f,1000.0f);
Chebyshev1Filter     cheby1(FilterType::Lowpass,1,44100.0f,1000.0f,0.8);
Chebyshev2Filter     cheby2(FilterType::Lowpass,1,44100.0f,1000.0f,0.9);

DSP1::OnePole       onepole(44100.0f,1000.0f);
DSP1::TwoPoles      twopole(44100.0f,1000.0f);
DSP1::OneZero       onezero(44100.0f,1000.0f);
DSP1::TwoZeros      twozero(44100.0f,1000.0f,0.5f);

// i need to fix it as it explodes if cutoff is higher than 1 or something
ZolzerBiquad  zolzer(FilterType::Lowpass,44100.0f,1000.0f,0.5f);
RbjFilter     rbj(FilterType::Lowpass,44100.0f,100.0f,0.5f);
Cascade18db   cascade18db(44100.0f,1000.0f,0);
mertIIR       iir(sampleRate,1000,0);

Phasor        lfo1(sampleRate,0.1,0.1);
Phasor        lfo2(sampleRate,2.6,1);


// Nigel is the same as Zolzer but has the missing ones
// Massberg = lowpass only 

PolyBLEP osc(44100.0f,PolyBLEP::SAWTOOTH);
DelayLine<float> delay(0.5);
MultiTapDelayLine<float> mtap(0.5);
CombFilter<float> comb1(0.9,0.5,0.1*44100,0.3*44100);
//CombFilter comb2(10,1,0.5,-2,-3);
//CombFilter comb3(5,0.4,0.5,-1,-1);
ADSR adsr(0.01,0.2,0.5,0.3,44100.0f);



int audio_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{    
    
    float * output = (float*)outputBuffer;
    float * input  = (float*)inputBuffer;

    sample_vector<float> buffer(framesPerBuffer);
    
    for(size_t i = 0; i < framesPerBuffer; i++)
    {
        float x = adsr.Tick()*osc_Play(lpO[0]);
        
        float l1 = lfo1.Tick();
        float l2 = lfo2.Tick();
        
        buffer[i] = cascade18db(x); //,1,l1,l2);
        buffer[i] = saturate(buffer[i],0.9+l1,2+l2)*pow(10,-0.5);
        buffer[i] = mtap.Tick(buffer[i]);
        
        //buffer[i] = delay.Tick(buffer[i]);
        //buffer[i] = comb1.Tick(buffer[i]);        
        //buffer[i] = comb2.Tick(buffer[i]);
        //buffer[i] = comb3.Tick(buffer[i]);        

        //buffer[i] = iir.Tick(x);        
        //buffer[i] = lowpass12.Tick(x);
        //buffer[i] = svf.Tick(x);
        //buffer[i] = filter6db.Tick(adsr.Tick()*x);        
        
    }    
    
    
    //buffer = DSP::dcremove(buffer,0.00025f);
    memcpy(output, buffer.data(), framesPerBuffer * sizeof(float));
    //for(size_t i = 0; i < framesPerBuffer; i++)
    //    output[i] += input[i];
    return 0;
}            


void midi_msg_print(MidiMsg * msg) {
    printf("%d %d %d\n",msg->msg,msg->data1,msg->data2);
}

double last_frequency;
double fcutoff;

void note_on(MidiMsg * msg) {
    double freq = midi_to_freq(msg->data1);
    double velocity = msg->data2/127.0f;
    
    osc.setFrequency(freq);
    adsr.noteOn();    
    
    iir.setCutoff(freq+fcutoff);
    filter6db.setCutoff(freq+fcutoff);    
    lowpass12.setCutoff(freq+fcutoff);
    cascade18db.setCutoff(freq+fcutoff);
    onepole.setCutoff(freq+fcutoff);
    onezero.setCutoff(freq+fcutoff);
    twopole.setCutoff(freq+fcutoff);
    twozero.setCutoff(freq+fcutoff);    
    zolzer.setCutoff(freq+fcutoff);

    last_frequency=freq;
    lpO[0]->f = freq/2;
    
    //poly.setFrequency(freq);
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
        double fc = sampleRate2*std::log(1 + (double)msg->data2/127.0f)/std::log(2);
        fcutoff=fc;
        iir.setCutoff(last_frequency + fc);
        filter6db.setCutoff(last_frequency+fc);
        cascade18db.setCutoff(last_frequency+fc);
        lowpass12.setCutoff(last_frequency+fc);        
        zolzer.setCutoff(last_frequency+fc);        
        onepole.setCutoff(last_frequency+fc);
        onezero.setCutoff(last_frequency+fc);
        twopole.setCutoff(last_frequency+fc);
        twozero.setCutoff(last_frequency+fc);                
    }
    if(msg->data1 == 103)
    {
        double q = std::log(1 + (double)msg->data2/127.0f)/std::log(2);
        iir.setQ(q);
        filter6db.setQ(q);
        lowpass12.setQ(q);        
        cascade18db.setQ(q);        
        twozero.setQ(q);
        twopole.setQ(q);                 
    }
}

void repl() {

}


int main()
{
    //set_audio_func(audio_callback);
    Init();
    noise.seed_engine();    

    /*
    mtap.addTap(0.01);
    mtap.addTap(0.086);
    mtap.addTap(0.1);
    mtap.addTap(0.1116);
    mtap.addTap(0.1551);
    mtap.addTap(0.2112);
    */
    mtap.addTap(0.25);
    mtap.addTap(0.1);
    minBLEP_Load();
    lpO[0] = osc_Create(sampleRate);
    int num_midi = GetNumMidiDevices();

    ITERATE(i,0,num_midi)
    {
        printf("midi device #%lu: %s\n", i, GetMidiDeviceName(i));
    }
    int num_audio = GetNumAudioDevices();
    int device = 9;

    ITERATE(i, 0, num_audio)    
    {
        //if(!strcmp(GetAudioDeviceName(i),"HDA Intel PCH: ALC3220 Analog (hw:1,0)")) device = i;
        printf("audio device #%lu: %s\n", i, GetAudioDeviceName(i));
    }
    
    set_note_on_func(note_on);
    set_note_off_func(note_off);
    set_audio_func(audio_callback);
    set_repl_func(repl);
    set_control_change_func(control_change);
    
    InitMidiDevice(0,3,-1);    
    InitAudioDevice(device,device,1,44100,256);
    RunAudio();
    StopAudio();
}
