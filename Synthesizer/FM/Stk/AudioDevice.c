#include "AudioDevice.h"

static int version = 0;
static const char* version_text = NULL;
static int host_api_count = 0;
static int default_host_api = 0;
static int device_count=-1;
static int default_input=-1;
static int default_output=-1;
static bool audio_init = false;

const char* GetVersionText()
{
    return version_text;
}

int GetHostApiCount() {
    return host_api_count;
}
int GetDefaultHostApi()
{
    return default_host_api;
}
int GetDeviceCount()
{
    return device_count;
}
int GetDefaultInput()
{
    return default_input;
}
int GetDefaultOutput() 
{
    return default_output;
}
bool GetAudioInit()
{
    return audio_init;
}

int GetVersion() { return version; }

void Sleep(long msec) {
    Pa_Sleep(msec);
}

void InitAudio() {
   int err =  Pa_Initialize();
   if(err != paNoError) {
       printf("Can't initialize portaudio\n");
       exit(-1);
   }
   audio_init = true;
   process_f = NULL;
   version = Pa_GetVersion();
   version_text = Pa_GetVersionText();
   host_api_count = Pa_GetHostApiCount();
   default_host_api = Pa_GetDefaultHostApi();
   device_count = Pa_GetDeviceCount();
   default_input = Pa_GetDefaultInputDevice();
   default_output = Pa_GetDefaultOutputDevice();
}
void TerminateAudio() {
    if(!audio_init) return;
    audio_init = false;
    int err = Pa_Terminate();
    if(err != paNoError) {
        printf("Failed to terminate portaudio\n");
    }
}


int GetNumAudioDevices() {
    int numDevices = Pa_GetDeviceCount();
    return numDevices;
}

void GetAudioDeviceInfo(AudioDeviceInfo * info)
{
    assert(info != NULL);
    Pa_GetDeviceInfo(&info->info);
}

bool IsFormatSupported(AudioDeviceParameters * p, double sample_rate) {
    assert(p != NULL);
    PaError err = Pa_IsFormatSupported(&p->input,&p->output, sample_rate)
    return err == paFormatIsSupported;
}

void SetInputParameters(AudioDeviceParameters * p, int device_index, int channels, SampleFormat format, double latency)
{
    assert(p != NULL);
    p->input.channelCount = channels;
    p->input.device       = device_index;    
    p->input.sampleFormat = format;
    p->input.suggestedLatency = latency == 1.0? Pa_GetDeviceInfo(device_index)->defaultLowOutputLatency : latency;
    p->input.hostApiSpecificStreamInfo = NULL;    
}
void SetOutputParameters(AudioDeviceParameters * p, int device_index, int channels, SampleFormat format, double latency) 
{
    assert(p != NULL);
    p->output.channelCount = channels;
    p->output.device       = device_index;    
    p->output.sampleFormat = format;
    p->output.suggestedLatency = latency == 1.0? Pa_GetDeviceInfo(device_index)->defaultLowOutputLatency : latency;
    p->output.hostApiSpecificStreamInfo = NULL;    
}









/*
int main() {
    InitAudio();
    print_portaudio();
    const PaDeviceInfo * info;
    for(size_t i = 0; i < device_count; i++)
    {
        info = Pa_GetDeviceInfo(i);
        print_info(info);
    }
    int err = Pa_OpenDefaultStream( &stream, 0,2,paFloat32,44100,256,StreamCallback,&data);
    if(err != paNoError) {
        printf("Failed to open stream\n");
        exit(-1);
    }
    err = Pa_StartStream(stream);
    if(err != paNoError) {
        printf("Failed to start stream\n");
        exit(-1);
    }
    Sleep(5000);
    err = Pa_StopStream(stream);
    if(err != paNoError) {
        printf("Failed to stop stream\n");
        exit(-1);
    }
    TerminateAudio();
}
*/