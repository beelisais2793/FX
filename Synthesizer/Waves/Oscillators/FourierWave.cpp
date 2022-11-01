//#include "AudioDevice.h"
#include <cstring>
#include <vector>
#include <portaudio.h>
#include <cstdio>
#include "include/FourierWave.h"
#include <iostream>
#include "AudioDevice.h"


using std::cout;
using std::cin;
using std::endl;
using namespace SoundWave;

FourierWave * wav;

struct StreamCallbackData {
    const float * input;
    float * output;
    unsigned long frameCount;
    const PaStreamCallbackTimeInfo *timeinfo;
    StreamCallbackFlags flags;
    void * userData;
};


int StreamCallback(const void * input, void * output, unsigned long frameCount, const PaStreamCallbackTimeInfo * timeinfo, PaStreamCallbackFlags statusFlags, void * userData) {
    float * out = (float*)output;
    for(size_t i = 0; i < frameCount; i++) {
        out[i] = 0.2*wav->Tick();        
    }
    return paContinue;
}


int main()
{
    AudioDevice device;
    std::cout << device.GetDeviceCount() << std::endl;
    for(size_t i = 0; i < device.GetHostApiCount(); i++) {
        AudioHostApiInfo api(i);
        cout << "API: " << api.name() << endl;
    }
    int device_id = 14;
    for(size_t i = 0; i < device.GetDeviceCount(); i++)
    {
        AudioDeviceInfo info(i);
        cout << "Device # " << i << ": " << info.name() << endl;
        if(!strcmp(info.name(),"pulse")) device_id = i;
    }
    
    wav = new FourierWave(44100);
    wav->SetTriangle(220.0f);
    AudioStreamParameters input(device_id,1,SAMPLE_FLOAT32,-1.0);
    AudioStreamParameters output(device_id,1,SAMPLE_FLOAT32,-1.0);
    AudioStream stream(&input,&output,44100,256,STREAM_NOFLAG,StreamCallback,wav);
    stream.StartStream();
    Sleep(5000);
    stream.StopStream();
    
}
