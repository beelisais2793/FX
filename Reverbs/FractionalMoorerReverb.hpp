//https://github.com/yamabee/MoorerReverb
#pragma once
#include <cmath>
using namespace std;

class FractionalDelay {
    
public:
    
    // Constructor function (special function - no return type, name = Class name)
    FractionalDelay();
    
    FractionalDelay(float delay, float speed);
    
    // Destructor
    ~FractionalDelay();
    
    float processSample(float x,int channel);

    void setFs(float Fs);
    
    void setDelaySamples(float delay);
    
    void setSpeed(float speed);
    void setDepth(float depth);

    
private:
    
    float Fs = 48000.f;
    
    float delay = 5.f;
    
    const int MAX_BUFFER_SIZE = 96000;
    float delayBuffer[96000][2] = {0.0f};
    int index[2] = {0};
    
    float speed = 1.0f; // Hz, frequency of LFO
    float depth = 10.0f; // percentage of intensity, control amp of LFO
    
    float currentAngle[2] = {0.0f};
    float angleChange = speed * (1.f/Fs) * 2.f * M_PI;
    
    
    
};

FractionalDelay::FractionalDelay(){
}

FractionalDelay::FractionalDelay(float delay,float speed){
    this->delay = delay;
    this->speed = speed;
}

// Destructor
FractionalDelay::~FractionalDelay(){
}


float FractionalDelay::processSample(float x, int channel){
    
    if (delay < 1.f){
        return x;
    }
    else{
        float lfo;
        
        lfo = depth * sin(currentAngle[channel]);
        
        currentAngle[channel] += angleChange;
        if (currentAngle[channel] > 2.f * M_PI){ //o.phase 

            currentAngle[channel] -= 2.f * M_PI;
        }
        
        // Delay Buffer
        // "delay" can be fraction
        int d1 = floor(delay+lfo);
        int d2 = d1 + 1;
        float g2 = delay + lfo - (float)d1;
        float g1 = 1.0f - g2;
        
        int indexD1 = index[channel] - d1;
        if (indexD1 < 0){
            indexD1 += MAX_BUFFER_SIZE;
        }
        
        int indexD2 = index[channel] - d2;
        if (indexD2 < 0){
            indexD2 += MAX_BUFFER_SIZE;
        }
        
        float y = g1 * delayBuffer[indexD1][channel] + g2 * delayBuffer[indexD2][channel];
        
        delayBuffer[index[channel]][channel] = x;
        
        if (index[channel] < MAX_BUFFER_SIZE - 1){
            index[channel]++;
        }
        else{
            index[channel] = 0;
        }
        
        return y;
    }
}

void FractionalDelay::setFs(float Fs){
    this->Fs = Fs;
}


void FractionalDelay::setDelaySamples(float delay){
    if (delay >= 1.f){
        this->delay = delay;
    }
    else{
        this->delay = 0.f;
    }
}


void FractionalDelay::setSpeed(float speed){
    if (speed >= 0.1f && speed <= 10.0f){
        this->speed = speed;
        angleChange = speed * (1.f/Fs) * 2 * M_PI;
    }
}

void FractionalDelay::setDepth(float depth){

    this->depth = depth;
}

// Class for implimenting colorless reverb using an
// All pass filter
class APF{
    
public:
    //No call constructor
    APF();
    APF(float newFs, float newDelaySamples, float newSpeed, float newDepth, float newGain);
    
    float processSample(float x, int chan);
    
    //call to update delay for APF in Moorer Reverb
    void setDelaySamples(float newDelaySamples);
    
    //call to update speed for APF in Moorer Reverb
    void setSpeed(float newSpeed);
    void setDepth(float newDepth);
    void setGain(float newGain);
    
//    float getDelaySamples();
    float getGain();
    
    void setSampleRate(float newFs);
    
private:
    
    float Fs = 48000.f;
    
    float delaySamples = 5.0f;
    float speed = 0.9f;
    float depth = 0.8f;
    float gain = 0.f;
    
    float memory[2] = {0.f};
    float w[2] = {0.f};
    float v[2] = {0.f};
    
    FractionalDelay frac;
    
};

APF::APF(){
    
}

APF::APF(float newFs, float newDelaySamples, float newSpeed, float newDepth, float newGain) {
    setSampleRate(newFs);
    setDelaySamples(newDelaySamples);
    setGain(newGain);
    
}

void APF::setSampleRate(float newFs){
    Fs = newFs;
    frac.setFs(Fs);
    
}

void APF::setDelaySamples(float newDelaySamples){
    delaySamples = newDelaySamples;
    frac.setDelaySamples(delaySamples);
    
}

void APF::setSpeed(float newSpeed){
    speed = newSpeed;
    frac.setSpeed(speed);
    
}

void APF::setDepth(float newDepth){
    depth = newDepth;
    frac.setDepth(depth);
    
}

void APF::setGain(float newGain){
    gain = newGain;
    
}

float APF::getGain() {
    return gain;
    
}

float APF::processSample(float x, int chan){
    w[chan] = frac.processSample(memory[chan], chan);
    v[chan] = x + (-gain * w[chan]);
    memory[chan] = v[chan];
    float y = (gain * v[chan]) + w[chan];
    
    return y;
    
}

class EarlyReflections {
    
public:
    EarlyReflections();
    ~EarlyReflections();
    
    void setSampleRate(float newFs);
    
    void setDelayTimeSamples();
    void setGain();
    
    void createIndex();
    
    float processSample(float x, int chan);
    
private:
    float Fs = 48000.f;
    
    FractionalDelay *frac[19];
    
//    float buf[48000][2] = {0.f};
//    float bufferLength = 48000.f;
//
//    int wIndex[2] = {48000,48000};
//    int rIndex[19][2];
    
    float delayTimeSamples[19];
    float gain[19];
    
    float y[2] = {0.f,0.f};
    
};


EarlyReflections::EarlyReflections() {
    frac[0] = new FractionalDelay();
    frac[1] = new FractionalDelay();
    frac[2] = new FractionalDelay();
    frac[3] = new FractionalDelay();
    frac[4] = new FractionalDelay();
    frac[5] = new FractionalDelay();
    frac[6] = new FractionalDelay();
    frac[7] = new FractionalDelay();
    frac[8] = new FractionalDelay();
    frac[9] = new FractionalDelay();
    frac[10] = new FractionalDelay();
    frac[11] = new FractionalDelay();
    frac[12] = new FractionalDelay();
    frac[13] = new FractionalDelay();
    frac[14] = new FractionalDelay();
    frac[15] = new FractionalDelay();
    frac[16] = new FractionalDelay();
    frac[17] = new FractionalDelay();
    frac[18] = new FractionalDelay();
    
}

EarlyReflections::~EarlyReflections() {
    delete frac[0];
    delete frac[1];
    delete frac[2];
    delete frac[3];
    delete frac[4];
    delete frac[5];
    delete frac[6];
    delete frac[7];
    delete frac[8];
    delete frac[9];
    delete frac[10];
    delete frac[11];
    delete frac[12];
    delete frac[13];
    delete frac[14];
    delete frac[15];
    delete frac[16];
    delete frac[17];
    delete frac[18];
}

void EarlyReflections::setSampleRate(float newFs) {
    Fs = newFs;
    setDelayTimeSamples();
    
}

void EarlyReflections::setDelayTimeSamples(){
    
    delayTimeSamples[0] = 0.f;
    delayTimeSamples[1] = round(Fs*0.01277f);
    delayTimeSamples[2] = round(Fs*0.01283f);
    delayTimeSamples[3] = round(Fs*0.01293f);
    delayTimeSamples[4] = round(Fs*0.01333f);
    delayTimeSamples[5] = round(Fs*0.01566f);
    delayTimeSamples[6] = round(Fs*0.02404f);
    delayTimeSamples[7] = round(Fs*0.02679f);
    delayTimeSamples[8] = round(Fs*0.02731f);
    delayTimeSamples[9] = round(Fs*0.02737f);
    delayTimeSamples[10] = round(Fs*0.02914f);
    delayTimeSamples[11] = round(Fs*0.02920f);
    delayTimeSamples[12] = round(Fs*0.02981f);
    delayTimeSamples[13] = round(Fs*0.03389f);
    delayTimeSamples[14] = round(Fs*0.04518f);
    delayTimeSamples[15] = round(Fs*0.04522f);
    delayTimeSamples[16] = round(Fs*0.04527f);
    delayTimeSamples[17] = round(Fs*0.05452f);
    delayTimeSamples[18] = round(Fs*0.06958f);
    
    for (int i = 0; i < 19; i++) {
        frac[i]->setFs(Fs);
        frac[i]->setDepth(0);
        frac[i]->setDelaySamples(delayTimeSamples[i]);
    }
}

void EarlyReflections::setGain(){
    gain[0] = 1.f;
    gain[1] = 0.1526f;
    gain[2] = -0.4097f;
    gain[3] = 0.2984f;
    gain[4] = 0.1553f;
    gain[5] = 0.1442f;
    gain[6] = -0.3124f;
    gain[7] = -0.4176f;
    gain[8] = -0.9391f;
    gain[9] = 0.6926f;
    gain[10] = -0.5787f;
    gain[11] = 0.5782f;
    gain[12] = 0.4206f;
    gain[13] = 0.3958f;
    gain[14] = 0.3450f;
    gain[15] = -0.5361f;
    gain[16] = 0.417f;
    gain[17] = 0.1948f;
    gain[18] = 0.1548f;
    
}



float EarlyReflections::processSample(float x, int chan) {

    y[chan] = 0.f;
    
    for (int tap = 0; tap < 19; tap++) {
        y[chan] += gain[tap] * frac[tap]->processSample(x, chan);
        
    }
    
    return y[chan];
}


class FBCF 
{
public:
    FBCF();
    FBCF(float newFs, float newDelaySamples, float newSpeed, float newDepth, float newGain);
    
    ~FBCF() override;
    
    void setGain(float newGain);
    void setSampleRate(float newFs);
    void setDelaySamples(float newDelaySamples);
    void setSpeed(float newSpeed);
    void setDepth(float newDepth);
    
    float getDelaySamples();
    float getGain();

    float processSample(float x, int channel);

private:
    //create instance of fractional delay in private
    FractionalDelay frac;
    
    float Fs =  48000.f;
    
    float gain = -0.9f;
    float delaySamples = 0.f;
    float speed = 0.f;
    float depth = 5.f;
    float memory[2] = {0.f,0.f};
    float w[2] = {0.f,0.f};
    float y[2] = {0.f,0.f};
    
//    float fb = 0.f;
    
};


FBCF::FBCF(){}

FBCF::FBCF(float newFs, float newDelaySamples, float newSpeed, float newDepth, float newGain) {
    setSampleRate(newFs);
    setDelaySamples(newDelaySamples);
    setSpeed(newSpeed);
    setDepth(newDepth);
    setGain(newGain);
    
}

FBCF::~FBCF()
{
}

void FBCF::setGain(float newGain){
    gain=newGain;
     
}

void FBCF::setSampleRate(float newFs){
    Fs = newFs;
    frac.setFs(Fs);
}

void FBCF::setDelaySamples(float newDelaySamples){
    delaySamples = newDelaySamples;
    frac.setDelaySamples(delaySamples);
}

void FBCF::setSpeed(float newSpeed){
    speed = newSpeed;
    frac.setSpeed(speed);
}

void FBCF::setDepth(float newDepth) {
    depth = newDepth;
    frac.setDepth(depth);
    
}

float FBCF::getDelaySamples() {
    return delaySamples;
}

float FBCF::getGain() {
    return gain;
}

float FBCF::processSample(float x, int channel){
    
    w[channel] = frac.processSample(x + gain * (0.5 * w[channel] + 0.5 * memory[channel]), channel);
    y[channel] = w[channel];
    memory[channel] = y[channel];
    
//    float fb = x + -gain * w;
//    float y = fb;
    
    return y[channel];
    
}


class Moorer
{
public:
    
    Moorer();
    ~Moorer();
    
    const float combDelay[4]={.0437,.0411,.0371,.0297};
    const float cDGain[4] = {-.9,.9, -.9,.9};
    const float apfGain[2] = {.707,.707};
    const float apfDelay[2] = {.0050, .0017};
    
    
    //getters
//    float getCombDelay(const int elem);
//    float getAllpassDelay(const int elem);
//    float getAllpassGain(const int elem);
    
    //setters
    void setCombDelay(const int elem, const float combDelay);
    void setAllpassDelay(const int elem, const float apfDelay);
    
    void setSampleRate(float newFs);
    void setCombGain(const float percent);
    void setAllpassGain(float newGain);
    
//    void setAllpassDelay(const int elem, const int Fs, const float d);
    void setModulation(float newModulation);
    
//    void setAllpassGain(const int elem, const float g);
    
    //main methods
    float processSample(const float x, const int channel);

private:
    float Fs = 48000;
    float apGainPercent = 1;
    float cfGainPercent = 1;
    float modulation = 1;
    
    FBCF *combs[4];
    APF *allpass[2];
    EarlyReflections *er[1];

};

Moorer::Moorer(){
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    combs[0] = new FBCF();
    combs[1] = new FBCF();
    combs[2] = new FBCF();
    combs[3] = new FBCF();
    for (int i = 0; i<4; i++) {
        combs[i]->setDelaySamples(combDelay[i]*Fs);
        combs[i]->setGain(cDGain[i]);
    }
    
    
    allpass[0] = new APF();
    allpass[1] = new APF();
    for (int i = 0; i<2; i++) {
        allpass[i]->setDelaySamples(apfDelay[i]*Fs);
        allpass[i]->setGain(apfGain[i]);
    }
    
    er[0] = new EarlyReflections();
    er[0]->setGain();
}

Moorer::~Moorer(){
    delete combs[0];
    delete combs[1];
    delete combs[2];
    delete combs[3];
    
    delete allpass[0];
    delete allpass[1];
    
    delete er[0];
}

//float Moorer::getCombDelay(const int elem) {
//    return combs[elem]->getDelaySamples();
//
//}

//float Moorer::getAllpassDelay(const int elem){
//    return allpass[elem]->getDelaySamples();
//
//}

//float Moorer::getAllpassGain(const int elem){
//    return allpass[elem]->getGain();
//
//}

void Moorer::setSampleRate(float newFs) {
    Fs = newFs;
    for (int i = 0; i < 4; i++) {
        combs[i]->setSampleRate(Fs);
    }
    
    for (int j = 0; j < 2; j++) {
        allpass[j]->setSampleRate(Fs);
    }
    
    er[0]->setSampleRate(Fs);
    
}

void Moorer::setCombDelay(const int elem, const float combDelay) {
    for (int i = 0; i < 4 ; i++) {
        combs[i]->setDelaySamples(combDelay);
    }
}
void Moorer::setAllpassDelay(const int elem, const float apfDelay) {
    for (int j = 0; j < 2 ; j++) {
        allpass[j]->setDelaySamples(apfDelay);
    }
}

void Moorer::setCombGain(const float newGainPercent){
    cfGainPercent = newGainPercent;
    for(int i = 0; i < 4; i++){
        
            float gain = 0.0f;
            gain = combs[i]->getGain()*cfGainPercent;
            combs[i]->setGain(gain);
        }
}

void Moorer::setAllpassGain(float newGain) {
    apGainPercent = newGain;
    for (int i = 0; i < 2; ++i) {
        float gain = 0.0f;
        gain = allpass[i] -> getGain()*apGainPercent;
        allpass[i]->setGain(gain);
    }
}

void Moorer::setModulation(float newModulation) {
    modulation = newModulation;
    float rateValue = 0.6f;
    for(int i = 0; i < 4; i++){
        combs[i]-> setDepth(modulation);
        combs[i]-> setSpeed(rateValue);
        rateValue = rateValue + 0.12;
        }
    for (int i = 0; i < 2; ++i) {
        allpass[i]->setDepth(modulation);
        allpass[i]->setSpeed(rateValue);
        rateValue = rateValue + 0.12;
    }
}

//void Moorer::setAllpassGain(const int elem, const float g){
//    allpass[elem]->setGain(g);
//
//}

//void Moorer::setAllpassDelay(const int elem, const int Fs, const float d) {
//    allpass[elem]->setFs(Fs);
//    allpass[elem]->setDelaySamples(d);
//
//}

float Moorer::processSample(const float x, const int channel){
    float combOut[2] = {0.0f, 0.f};
    float apf1Out[2] = {0.0f,0.f};
    float apf2Out[2] = {0.0f,0.f};
    float ers[2] = {0.0f,0.f};
    float tapOut[2] = {0.0f,0.f};
    
    //early reflect
    ers[channel] = er[0]->processSample(x,channel);
    tapOut[channel] = ers[channel];
    
    //combs take output from early reflect generator
    for(int i = 0; i < 4; i++){
        combOut[channel] += combs[i]->processSample(tapOut[channel] * 0.25f, channel);
    }

    //output of combs fed to all pass
    apf1Out[channel] = allpass[0]->processSample(combOut[channel],channel);
    apf2Out[channel] = allpass[1]->processSample(apf1Out[channel],channel);
    
    return apf2Out[channel];
    
}
