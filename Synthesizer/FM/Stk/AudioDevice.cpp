#include <iostream>
#include "AudioDevice.h"
#include "DspGraph.hpp"

using std::cout;
using std::cin;
using std::endl;

struct StreamCallbackData {
    const float * input;
    float * output;
    unsigned long frameCount;
    const PaStreamCallbackTimeInfo *timeinfo;
    StreamCallbackFlags flags;
    void * userData;
};

int ProcessFunction(StreamCallbackData * ptr) 
{
    WaveTable * wav = (WaveTable*)ptr->userData;
    SampleVector input(ptr->input,ptr->frameCount,1);
    SampleVector output(ptr->output, ptr->frameCount,1);
    wav->Run(ptr->frameCount,input, output);   
    output *= 0.5;  
    memcpy(ptr->output,output.data(),ptr->frameCount*sizeof(float));
    return 0;
}

int StreamCallback(const void * input, void * output, unsigned long frameCount, const PaStreamCallbackTimeInfo * timeinfo, PaStreamCallbackFlags statusFlags, void * userData) {
    StreamCallbackData data;
    data.input = (float*)input;
    data.output = (float*)output;
    data.frameCount = frameCount;
    data.timeinfo = timeinfo;
    data.flags = (StreamCallbackFlags)statusFlags;
    data.userData = userData;
    return ProcessFunction(&data);
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
    
    Node * n = new Node;
    Connection * c = new Connection(256,n);
    WaveTable *wav = new WaveTable(c,44100,256,1);
    
    std::vector<float> v(4096);
    SoundWave::MakeSine<float>(v,4096,220,44100);
    wav->AddWaveTable(4096,v,22050);
    wav->SetFrequency(220);
    n->SetDSP(wav);
    
    AudioStreamParameters input(device_id,1,SAMPLE_FLOAT32,-1.0);
    AudioStreamParameters output(device_id,1,SAMPLE_FLOAT32,-1.0);
    AudioStream stream(&input,&output,44100,256,STREAM_NOFLAG,StreamCallback,wav);
    stream.StartStream();
    Sleep(5000);
    stream.StopStream();
    
}