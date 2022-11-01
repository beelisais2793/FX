// https://github.com/erictarrbelmont/MoorerReverb
#pragma once

#include <math.h>


class APF {
    
public:
    
    APF(){};
    ~APF(){};
    
    float processSample(float x,int channel);
    
    void setFs(float Fs);
    float getFs();
    
    void setDelaySamples(int delaySamples);
    float getDelaySamples();
    
    void setDelayMs(float delayMs);
    float getDelayMs();
    
    void setGain(float gain);
    float getGain();
    
    void setModAmp(float m);
    void setFreqLFO(float f);
    
private:

    float Fs;
    float delaySamples;
    float delayMs;
    float gain;
    
    Delay delay;
    
    float delaySig = 0.0f;
};

float APF::processSample(float x, int channel){
    
    float delayIn = x + (-gain * delaySig);
    delaySig = delay.processSample(delayIn, channel);

    float y = delaySig + (gain * delayIn);
    
    return y;
};

void APF::setFs(float Fs){
    if (Fs == 44100 || Fs == 48000 || Fs == 88200 || Fs == 96000 || Fs == 192000){
        this->Fs = Fs;
        delay.setFs(Fs);
    }
};

float APF::getFs(){
    return Fs;
};

void APF::setDelaySamples(int delaySamples){
    this->delaySamples = delaySamples;
    delay.setDelaySamples(delaySamples);
};

float APF::getDelaySamples(){
    return delaySamples;
};

void APF::setDelayMs(float delayMs){
    this->delayMs = delayMs;
    delay.setDelaySamples(delayMs * .001 * Fs);
};

float APF::getDelayMs(){
    return delayMs;
};

void APF::setGain(float gain){
    this->gain = gain;
};

float APF::getGain(){
    return gain;
};

void APF::setModAmp(float m){
    delay.setModAmp(m);
};

void APF::setFreqLFO(float f){
    delay.setFreqLFO(f);
};

class Delay{
    
public:
    
    float processSample(float x, int channel);
    
    
    void setFs(int Fs);
    
    float getFs();
    
    
    void setDelaySamples(float d);
    
    float getDelaySamples();
    
    
    void setModAmp(float m);
    
    float getModAmp();
    
    
    void setFreqLFO(float f);
    
    float getFreqLFO();
    
    float lfo();
    
    
    
    
    
    
    
    
private:
    const int maxBufferSize = 96000;
    float delayBuffer[96000][2]= {0.0f};
    float getNextSample();
    float Fs = 48000.f;
    float offset = 0.0f;
    float angleChange = 0.0f;
    int index[2] = {0};
    float delay=0.0f;

    float amp = 0.0f;
    float modAmp=0.0f;
    float freqLFO=0.0f;
    float sinSynth();
    
    float currentAngle[2] = {0.0f};
    void updateAngle(int channel);
};

float Delay::processSample(float x, int channel){
    float y = 0;
    
    float lfo = amp * sinf(currentAngle[channel]) + offset;
    updateAngle(channel);
    
    float d1 = floorf(delay + lfo);
    float d2 = d1 + 1.0f;
    float g2 = delay + lfo - d1;
    float g1 = 1.0f - g2;
    
    int indexD1 = index[channel] - d1;
    if (indexD1 < 0){
        indexD1 += maxBufferSize;
    }
    
    int indexD2 = index[channel] - d2;
    if (indexD2 < 0){
        indexD2 += maxBufferSize;
    }
    
    y= g1 * delayBuffer[indexD1][channel] + g2 * delayBuffer[indexD2][channel];
    
    delayBuffer[index[channel]][channel] = x;
    if (index[channel] <= maxBufferSize - 2){
            index[channel]++;
    }
    else{
        index[channel] = 0;
    }
    return y; 
}

void Delay::setFs(int Fs){
    if (Fs == 44100 || Fs == 48000 || Fs == 88200 || Fs == 96000 || Fs == 192000){
        this ->Fs = Fs;
    }
};

float Delay::getFs(){
    return Fs;
};

void Delay::setDelaySamples(float d){
    if (d <= maxBufferSize || d >= 0.0f){
        delay = d;
    }
};

float Delay::getDelaySamples(){
    return delay;
};

void Delay::setModAmp(float m){
    if (m <= 10.0f || m >= 0.0f){
        amp = m;
        offset = amp+1;
    }
};

float Delay::getModAmp(){
    return amp;
};

void Delay::setFreqLFO(float f){
    if (f >= 0.1f || f <= 5){
        angleChange = 2 * M_PI * f/Fs;
        freqLFO = f;
    }
};

float Delay::getFreqLFO(){
    return freqLFO;
};

void Delay::updateAngle(int channel){
    currentAngle[channel] += angleChange;
    if (currentAngle[channel] > 2 * M_PI){
        currentAngle[channel] -= (2*M_PI);
    }
}

class EarlyReflections {
    
  
public:
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

        float processSample(float x,int channel);

        void setFs(float Fs);
        float getFs();

        void setGain(float ampdB);//float input needs to come from process block
        float getGain();

        void setDelaySamples(int d); //get delay from somewhere
        int getDelaySamples();

        void setRate(float r);
        float getRate();

        void setDepth(float depth);
        float getDepth();


    private:
    int Fs = 48000;
    Delay delay;
    float delayTime = 1.0f;
    float gain = 0.7f;
    float y1[2]={0.0f};
    float rate = 1.0f;
    float depth = 1.0f;
    int maxBufferSize = 960000;
    
};


   // Functions for Compressor
    float FBCF::processSample(float x,int channel){
    
    float w=x+ (-gain)*y1[channel];
    //y(n)=x(n)-g*y(n - delay) use this for difference equation for FBCF
        //delay.processSample(w);
        y1[channel]=delay.processSample(w,channel);
        return y1[channel];
    }

    void FBCF::setFs(float Fs){
        // Code to check for valid sampling rate
        if (Fs == 44100 || Fs==48000 ||Fs==88200||Fs==96000||Fs==192000) {
            
            this->Fs = Fs;
            delay.setFs(Fs);
        }
    }
    
    
    
    float FBCF::getFs(){
        return Fs;
    };
    
    void FBCF::setGain(float g){
        // Code to check that "d" is valid for drive
        if (g < 1.0f) {
            if (g >= 0.0f){
                gain = g;
            }
        }
    };
    
    
    float FBCF::getGain(){
        
        return gain;
    };
    
    
    void FBCF::setDelaySamples(int d){
        if (d <= maxBufferSize || d >= 0.0f){
            delayTime = d;
            delay.setDelaySamples(d);
        }
    };
    
    int FBCF::getDelaySamples(){
        return delayTime;
    };
    
    void FBCF::setRate(float r){
        rate = r;
        delay.setFreqLFO(rate);
    };
    float FBCF::getRate(){
        return rate;
    };
    
    void FBCF::setDepth(float depth){
      this->depth = depth;
        delay.setModAmp(depth);
        
    }
    float FBCF::getDepth(){
        return depth;
    }

class MoorerReverb {
    
public:
    MoorerReverb();
    
    float processSample(float x, int channel);
    
    void setReverbTime(float rTime);
    
    void setDiffusion(float difValue);
    
    void setSamplingRate(int sampleRate);
    
    void setModulation(float modValue);

    APF apf1;
    APF apf2;
    FBCF fbcf1;
    FBCF fbcf2;
    FBCF fbcf3;
    FBCF fbcf4;
    EarlyReflections er1;
    
    
private:
    int Fs = 48000;
};

MoorerReverb::MoorerReverb(){
    fbcf1.setDelaySamples(1426);
    fbcf2.setDelaySamples(1781);
    fbcf3.setDelaySamples(1973);
    fbcf4.setDelaySamples(2098);
    
    fbcf1.setGain(0.7f);
    fbcf2.setGain(0.7f);
    fbcf3.setGain(0.7f);
    fbcf4.setGain(0.7f);
    
    fbcf1.setDepth(.1);
    fbcf2.setDepth(.1);
    fbcf3.setDepth(.1);
    fbcf4.setDepth(.1);
    
    fbcf1.setRate(.99);
    fbcf2.setRate(.91);
    fbcf3.setRate(.93);
    fbcf4.setRate(.97);
    
    apf1.setModAmp(.1);
    apf2.setModAmp(.1);
    
    apf1.setFreqLFO(1.1);
    apf2.setFreqLFO(1.2);
    
    apf1.setGain(.7);
    apf2.setGain(.7);
    
    apf1.setDelaySamples(240);
    apf2.setDelaySamples(82);
    
};


float MoorerReverb::processSample(float x, int channel){
    
    // Send input into early reflections
    x = er1.processSample(x, channel);
    
    // Send input into each FBCF
    float x1 = fbcf1.processSample(x,channel);
    float x2 = fbcf2.processSample(x,channel);
    float x3 = fbcf3.processSample(x,channel);
    float x4 = fbcf4.processSample(x,channel);
    
    // Sum the FBCFs together
    x = (x1 + x2 + x3 + x4)*0.5f;
    
    // Send processed input into APFs
    x = apf1.processSample(x, channel);
    x = apf2.processSample(x, channel);
    
    float y = x;
    
    return y;
};

void MoorerReverb::setSamplingRate(int Fs){
    this->Fs = Fs;
    er1.setFs(Fs);
    fbcf1.setFs(Fs);
    fbcf2.setFs(Fs);
    fbcf3.setFs(Fs);
    fbcf4.setFs(Fs);
    apf1.setFs(Fs);
    apf2.setFs(Fs);
};


void MoorerReverb::setReverbTime(float gain){
    fbcf1.setGain(gain);
    fbcf2.setGain(gain);
    fbcf3.setGain(gain);
    fbcf4.setGain(gain);
    
}

void MoorerReverb::setDiffusion(float difValue){
    if (difValue > 0.99){
        difValue = 0.99;
    }
    if (difValue < 0.1){
        difValue = 0.1;
    }
    apf1.setGain(difValue -.01);
    apf2.setGain(difValue -.03);

};


void MoorerReverb::setModulation(float modValue){
    apf1.setModAmp(modValue);
    apf2.setModAmp(modValue);
    fbcf1.setDepth(modValue);
    fbcf2.setDepth(modValue);
    fbcf3.setDepth(modValue);
    fbcf4.setDepth(modValue);
};