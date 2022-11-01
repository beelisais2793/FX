// https://github.com/yamabee/MoorerReverb
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




class APF {
    
public:
    
    // Constructor function (special function - no return type, name = Class name)
    APF();
    
    APF(float delay , float speed){
        fractionalDelay.setDelaySamples(delay);
        fractionalDelay.setSpeed(speed);
    }
    
    // Destructor
    ~APF();
    
    float processSample(float x,int channel);

    void setFs(float Fs);
    
    void setFeedbackGain(float feedbackGain );
    
    void setDepth(float depth);
    
private:
    
    float Fs = 48000.f;
    
    FractionalDelay fractionalDelay {240.f , 0.7f};
    
    float feedbackGain = 0.5f;
    
    float fb1[2] = {0.f};
    
    float depth = 10.0f; // percentage of intensity, control amp of LFO
    
};


APF::APF(){
}

// Destructor
APF::~APF(){
}


float APF::processSample(float x, int channel){
    
    float y;
    
    float inDL1 = x + (-feedbackGain) * fb1[channel];
    
    float outDL1 = fractionalDelay.processSample(inDL1 , channel);
    
    y = outDL1 + feedbackGain*inDL1;
    
    fb1[channel] = outDL1;
        
    return y;
}

void APF::setFs(float Fs){
    this->Fs = Fs;
    fractionalDelay.setFs(Fs);
}

void APF::setFeedbackGain(float feedbackGain){
    this->feedbackGain = feedbackGain;
}

void APF::setDepth(float depth){
    this->depth = depth;
    fractionalDelay.setDepth(depth); //Do I really need both of these?
}


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
        if (currentAngle[channel] > 2.f * M_PI){
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

class EarlyReflections {
    
    
public:
    EarlyReflections(){};

    void setFs(int Fs){
        if (Fs == 44100||Fs == 48000||Fs == 88200||Fs == 96000||Fs == 192000){
        this->Fs = Fs;
        }
    }
        
    float getFs(){
        return Fs;
    }
    
    // Call on PrepareToPlay to scale delay tap timestamps
    void scaleTapTimes(float Fs, int tapTimes[18]){
        f = Fs / 44100.0f;
        for (int n = 0; n < 18; ++n){;
            tapTimes[n] *= f;
        }
    }
    
    float processSample(float x, int channel){
        y = 0.0f;
       delayBuffer[index[channel]][channel] = x;
       for (int n = 0; n < 18; ++n){
           int circularIndex = index[channel] + tapTimes[n];
           if (circularIndex >= 3520){
               circularIndex = circularIndex - 3520;
           }
           y += tapGains[n] * delayBuffer[circularIndex][channel];
        }
        ++index[channel];
        if (index[channel] >= 3520){
            index[channel] = index[channel] - 3520;
        }
       return y;
    }
                                       
    
private:
    float f = 1.0f;
    float y = 0.0f;
    int delayBufferLength = 3520;
    float Fs = 48000;
    int tapIndex = 0;
    int gainIndex = 0;
                                               
                                               
    int tapTimes[18] = {190,949,993,1183,1192,1315,2021,2140,2524,2590,2625,2700,3119,3123,3202,3268,3321,3515};
                                               
    float tapGains[18] = {.841,.504,.49,.379,.38,.346,.289,.272,.192,.193,.217,.181,.18,.181,.176,.142,.167,.134};

                                              
    float delayBuffer[3520][2] = {0.0f};
    int index[2] = {0};
};

class FBCF {
    
public:
    
    // Constructor function (special function - no return type, name = Class name)
    FBCF();
    
    FBCF(float delay , float speed){
        fractionalDelay.setDelaySamples(delay);
        fractionalDelay.setSpeed(speed);
    }
    
    // Destructor
    ~FBCF();
    
    float processSample(float x,int channel);

    void setFs(float Fs);
    
    void setFeedbackGain(float feedbackGain );
    
    void setDepth(float depth);
    
private:
    
    float Fs = 48000.f;
    
    FractionalDelay fractionalDelay {240.f , 0.7f};
    
    float feedbackGain = 0.5f;
    
    float fb1[2] = {0.f};
    
    float depth = 10.0f; // percentage of intensity, control amp of LFO
    
};



FBCF::FBCF(){
}

// Destructor
FBCF::~FBCF(){
}


float FBCF::processSample(float x, int channel){
    
    float y;
    
    float inDL1 = x + (-feedbackGain) * fb1[channel];
    
    float outDL1 = fractionalDelay.processSample(inDL1 , channel);
    
    y = outDL1;
    
    fb1[channel] = outDL1;
        
    return y;
}

void FBCF::setFs(float Fs){
    this->Fs = Fs;
    fractionalDelay.setFs(Fs);
}

void FBCF::setFeedbackGain(float feedbackGain){
    this->feedbackGain = feedbackGain;
}

void FBCF::setDepth(float depth){
    this->depth = depth;
    fractionalDelay.setDepth(depth); //Do I really need both of these?
}


class Schroeder {
    
public:
    
    // Constructor function (special function - no return type, name = Class name)
    Schroeder();
    
    // Destructor
    ~Schroeder();
    
    float processSample(float x,int channel);

    void setFs(float Fs);
    
    void setFeedbackGain(float feedbackGain );
    
    void setDiffusionGain(float diffusionGain );
    
    void setDepth(float depth);
    
private:
    
    float Fs = 48000.f;
    
    FBCF fbcf1 {1426.f , 0.723f};
    FBCF fbcf2 {1781.f , 1.257f};
    FBCF fbcf3 {1973.f , .893f};
    FBCF fbcf4 {2098.f , 1.111f};
    
    APF apf1{240.f, 0.8321f};
    APF apf2{82.f , 0.9640f};
    
    float feedbackGain = 0.5f;
    float diffusionGain = 0.5f;
    
    float depth = 10.0f; // percentage of intensity, control amp of LFO
    
};

Schroeder::Schroeder(){
}

// Destructor
Schroeder::~Schroeder(){
}


float Schroeder::processSample(float x, int channel){
    
    float y;
    
    float outFBCF1 = fbcf1.processSample(x, channel);
    float outFBCF2 = fbcf1.processSample(x, channel);
    float outFBCF3 = fbcf1.processSample(x, channel);
    float outFBCF4 = fbcf1.processSample(x, channel);
    
    float inAPF1 = outFBCF1 + outFBCF2 + outFBCF3 + outFBCF4;
    
    float outAPF1 = apf1.processSample(outAPF1, channel);
    
    y = apf2.processSample(inAPF1, channel);
        
    return y;
}

void Schroeder::setFs(float Fs){
    this->Fs = Fs;
    fbcf1.setFs(Fs);
    fbcf2.setFs(Fs);
    fbcf3.setFs(Fs);
    fbcf4.setFs(Fs);
    apf1.setFs(Fs);
    apf2.setFs(Fs);
}

void Schroeder::setFeedbackGain(float feedbackGain){
    this->feedbackGain = feedbackGain;
    fbcf1.setFeedbackGain(feedbackGain);
    fbcf2.setFeedbackGain(feedbackGain);
    fbcf3.setFeedbackGain(feedbackGain);
    fbcf4.setFeedbackGain(feedbackGain);
}

void Schroeder::setDiffusionGain(float diffusionGain){
    this->diffusionGain = diffusionGain;
    apf1.setFeedbackGain(diffusionGain);
    apf2.setFeedbackGain(diffusionGain);
}

void Schroeder::setDepth(float depth){
    this->depth = depth;
    fbcf1.setDepth(depth);
    fbcf2.setDepth(depth);
    fbcf3.setDepth(depth);
    fbcf4.setDepth(depth);
    apf1.setDepth(depth);
    apf1.setDepth(depth);
}