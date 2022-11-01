#pragma once 

#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <iostream>

enum SampleFormats
{
    SAMPLE_FLOAT32 = paFloat32,
    SAMPLE_INT16   = paInt16,
    SAMPLE_INT32   = paInt32,
    SAMPLE_INT24   = paInt24,
    SAMPLE_INT8    = paInt8,
    SAMPLE_UINT8   = paUInt8,
};

enum StreamFlags 
{
    STREAM_NOFLAG = paNoFlag,
    STREAM_CLIPOFF= paClipOff,
    STREAM_DITHEROFF=paDitherOff,
    STREAM_NEVERDROPINPUT=paNeverDropInput,
    STREAM_PRIMEBUFFERS=paPrimeOutputBuffersUsingStreamCallback,
    STREAM_PLATFORM=paPlatformSpecificFlags
};

enum ErrorCode
  {
    ERROR_NONE = paNoError,
    ERROR_NOT_INITIALIZED = paNotInitialized,
    ERROR_UNANTICIPATED_HOST_ERROR = paUnanticipatedHostError,
    ERROR_INVALID_CHANNEL_COUNT = paInvalidChannelCount,
    ERROR_INVALID_SAMPLERATE = paInvalidSampleRate,
    ERROR_INVALID_DEVICE = paInvalidDevice,
    ERROR_INVALID_FLAG = paInvalidFlag,
    ERROR_SAMPLE_FORMATN_NOT_SUPPORTED = paSampleFormatNotSupported,
    ERROR_BAD_IO_DEVICEC_OMBINATION = paBadIODeviceCombination,
    ERROR_INSUFFICIENT_MEMORY =  paInsufficientMemory,
    ERROR_BUFFER_TOO_BIG = paBufferTooBig,
    ERROR_BUFFER_TOO_SMALL = paBufferTooSmall,
    ERROR_NULL_CALLBACK = paNullCallback,
    ERROR_BAD_STREAM_PTR = paBadStreamPtr,
    ERROR_TIMED_OUT = paTimedOut,
    ERROR_INTERNAL_ERROR = paInternalError,
    ERROR_DEVICEUN_AVAIABLE = paDeviceUnavailable,
    ERROR_INCOMPATIBLE_HOST_API_SPECIFIC_STREAM_INFO = paIncompatibleHostApiSpecificStreamInfo,
    ERROR_STREAM_IS_STOPPED =  paStreamIsStopped,
    ERROR_STREAM_IS_NOT_STOPPED = paStreamIsNotStopped,
    ERROR_INPUT_OVERFLOWED = paInputOverflowed,
    ERROR_OUTPUT_UNDERFLOWED = paOutputUnderflowed,
    ERROR_HOST_API_NOT_FOUND = paHostApiNotFound,
    ERROR_INVALID_HOST_API = paInvalidHostApi,
    ERROR_CAN_NOT_READ_FROM_CALLBACKSTREAM = paCanNotReadFromACallbackStream,
    ERROR_CAN_NOT_WRITE_TO_A_CALLBACKSTREAM = paCanNotWriteToACallbackStream,
    ERROR_CAN_NOT_READ_FROM_AN_OUTPUT_ONLY_STREAM = paCanNotReadFromAnOutputOnlyStream,
    ERROR_CAN_NOT_WRITE_TO_AN_INPUT_ONLY_STREAM = paCanNotWriteToAnInputOnlyStream,
    ERROR_INCOMPATIBLE_STREAM_HOST_API = paIncompatibleStreamHostApi,
    ERROR_BUFFERPTR = paBadBufferPtr,
  };

enum HostApiTypeId
  {
    API_INDEVLOPEMENT = paInDevelopment, /* use while developing support for a new host API */
    API_DIRECTSOUND   = paDirectSound,
    API_MME           = paMME,
    API_ASIO          = paASIO,
    API_SOUNDMANAGER  = paSoundManager,
    API_COREAUDIO     = paCoreAudio,
    API_OSS           = paOSS,
    API_ALSO          = paALSA,
    API_AL            = paAL,
    API_BEOS          = paBeOS,
    API_DMKS          = paWDMKS,
    API_JACK          = paJACK,
    API_WSAPI         = paWASAPI,
    API_AUDIOSCIENCE  = paAudioScienceHPI,
    API_AUDIOIO       = paAudioIO
  };

enum StreamCallbackFlags
{
    SFLAG_INPUT_UNDERFLOW = paInputUnderflow,
    SFLAG_INPUT_OVERFLOW = paInputOverflow,
    SFLAG_OUTPUT_UNDERFLOW = paOutputUnderflow,
    SFLAG_OUTPUT_OVERFLOW = paOutputOverflow,
    SFLAG_PRIMING_OUTPUT  = paPrimingOutput,
};

enum StreamCallbackResult {
    RESULT_CONTINUE=0,
    RESULT_COMPLETE=1,
    RESULT_ABORT=2
};

inline const char* GetErrorText(ErrorCode code) { return Pa_GetErrorText(code); }

struct AudioHostApiInfo 
{
    const PaHostApiInfo * info;

    AudioHostApiInfo(HostApiTypeId id) {
        info = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex((PaHostApiTypeId)id));
    }
    AudioHostApiInfo(int index) {
        info = Pa_GetHostApiInfo(index);
    }

    int structVersion() const { return info->structVersion; }
    HostApiTypeId type() const { return (HostApiTypeId)info->type; }
    const char* name() const { return info->name; }
    int deviceCount() const { return info->deviceCount; }
    int defaultInputDevice() const { return info->defaultInputDevice; }
    int defaultOutputDevice() const { return info->defaultOutputDevice; }    
};

struct AudioHostError
{
    const PaHostErrorInfo * info;

    AudioHostError() {
        info = Pa_GetLastHostErrorInfo();
    }

    HostApiTypeId hostApiType() const { return (HostApiTypeId)info->hostApiType; }
    long          errorCode()   const { return info->errorCode;   }
    const char *  errorText()   const { return info->errorText;   }
};

// PortAudio
struct AudioDeviceInfo
{
    const PaDeviceInfo * info;

    AudioDeviceInfo(int device) {
        info = Pa_GetDeviceInfo(device);
    }
    
    int structVersion() const { return info->structVersion; }
    const char * name() const { return info->name; }
    int hostApi() const { return info->hostApi; }
    int maxInputChannels() const { return info->maxInputChannels; }
    int maxOutputChannels() const { return info->maxOutputChannels; }

    double defaultLowInputLatency() const { return info->defaultLowInputLatency; }
    double defaultLowOutputLatency() const { return info->defaultLowOutputLatency; }
    double defaultHighInputLatency() const { return info->defaultHighInputLatency; }
    double defaultHighOutputLatency() const { return info->defaultHighOutputLatency; }
    double defaultSampleRate() const { return info->defaultSampleRate; }
};

struct AudioStreamParameters 
{
    PaStreamParameters params;

    AudioStreamParameters(int device_index, int channels, SampleFormats format, double latency)
    {        
        params.channelCount = channels;
        params.device       = device_index;    
        params.sampleFormat = format;
        params.suggestedLatency = latency == -1.0? Pa_GetDeviceInfo(device_index)->defaultLowOutputLatency : latency;
        params.hostApiSpecificStreamInfo = NULL;    
    }

    int device() const { return params.device; }
    int channelCount() const { return params.channelCount; }
    SampleFormats sampleFormat() const { return (SampleFormats)params.sampleFormat; }
    double suggestedLatency() const { return params.suggestedLatency; }
    void* hostApiSpecificStreamInfo() { return params.hostApiSpecificStreamInfo; }
};


struct VersionInfo {
    const PaVersionInfo * info;

    VersionInfo() {
        info = Pa_GetVersionInfo();
    }
    

    int versionMajor() const { return info->versionMajor; }
    int versionMinor() const { return info->versionMinor; }
    int versionSubMinor() const { return info->versionSubMinor; }
    const char * versionControlRevision() const { return info->versionControlRevision; }
    const char * versionText() const { return info->versionText; }
};

struct AudioStreamInfo {
    const PaStreamInfo *info;

    AudioStreamInfo( PaStream * stream )
    {
        info = Pa_GetStreamInfo(stream);
    }
    AudioStreamInfo(const AudioStreamInfo & i) : info(i.info) {} 

    AudioStreamInfo& operator = (const AudioStreamInfo & i) { info = i.info; return *this; }
    int structVersion() const { return info->structVersion; }
    double inputLatency() const { return info->inputLatency; }
    double outputLatency() const { return info->outputLatency; }
    double sampleRate() const { return info->sampleRate; }
};

struct AudioStream
{
    PaStream * stream;    
    AudioStreamParameters * input_params;
    AudioStreamParameters * output_params;

    AudioStream(AudioStreamParameters * in_params, AudioStreamParameters * out_params, double sample_rate, long frames, StreamFlags flags, PaStreamCallback cb, void * user_data) {
        PaError err = Pa_OpenStream(&stream,in_params? &in_params->params:NULL, out_params? &out_params->params:NULL, sample_rate, frames, flags, cb,user_data);
        input_params = in_params;
        output_params= out_params;
    }
    ~AudioStream() { if(stream) Pa_CloseStream(stream); }

    AudioStreamInfo GetStreamInfo() { return AudioStreamInfo(stream); }

    void StartStream() { Pa_StartStream(stream); }
    void StopStream()  { Pa_StopStream(stream);  }
    void AbortStream() { Pa_AbortStream(stream); }

    bool IsFormatSupported(double sample_rate) {        
        PaError err = Pa_IsFormatSupported(&input_params->params,&output_params->params, sample_rate);
        return err == paFormatIsSupported;
    }

    signed long GetStreamReadAvailable() { return Pa_GetStreamReadAvailable(stream); }
    signed long GetStreamWriteAvailable() { return Pa_GetStreamWriteAvailable(stream); }

    bool ReadStream(void * buffer, unsigned long frames) {
        return Pa_ReadStream(stream,buffer,frames) == 0;
    }
    bool WriteStream(void * buffer, unsigned long frames) {
        return Pa_WriteStream(stream,buffer,frames) == 0;
    }

    double GetStreamCpuLoad() {
        return Pa_GetStreamCpuLoad(stream);
    }

    double GetStreamTime() {
        return Pa_GetStreamTime(stream);
    }

    void GetStreamInfo(AudioStreamInfo & info) {
        Pa_GetStreamInfo(&info.info);
    }

    bool IsStreamActive() {
        return Pa_IsStreamActive(stream);
    }
    bool IsStreamStopped() {
        return Pa_IsStreamStopped(stream);
    }

};

struct AudioProcessData{
    const float * inut;
    float * output;
    unsigned long frameCount;
    int statusFlags;
    void * userData ;
    PaStreamCallbackTimeInfo * timeInfo;
};

struct AudioErrorInfo
{
    const PaHostErrorInfo * info;

    AudioErrorInfo() {
        info = Pa_GetLastHostErrorInfo();
    }

    long  errorCode() const  { return info->errorCode; }
    const char* name() const { return info->errorText; }
};

struct AudioDevice
{
    int version;
    const char* version_text;
    int host_api_count;
    int default_host_api;
    int device_count;
    int default_input;
    int default_output;
    bool audio_init;
    
    AudioDevice() {
        int err =  Pa_Initialize();
        if(err != paNoError) {
            printf("Can't initialize portaudio\n");
            exit(-1);
        }
        audio_init = true;    
        version = Pa_GetVersion();
        version_text = Pa_GetVersionText();
        host_api_count = Pa_GetHostApiCount();
        default_host_api = Pa_GetDefaultHostApi();
        device_count = Pa_GetDeviceCount();
        default_input = Pa_GetDefaultInputDevice();
        default_output = Pa_GetDefaultOutputDevice();     
    }
    ~AudioDevice() {
        if(!audio_init) return;
        audio_init = false;
        int err = Pa_Terminate();
        if(err != paNoError) {
            printf("Failed to terminate portaudio\n");    
        }
    }
        
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

    int GetNumAudioDevices() {
        int numDevices = Pa_GetDeviceCount();
        return numDevices;
    }
    
    size_t GetSampleSize(SampleFormats format) {
        return Pa_GetSampleSize(format);
    }    
};
    

inline void Sleep(long msec) {
    Pa_Sleep(msec);
}

