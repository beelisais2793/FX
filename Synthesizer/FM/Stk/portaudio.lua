ffi = require('ffi')
ffi.cdef[[

void* malloc(size_t);

int Pa_GetVersion( void );
const char* Pa_GetVersionText( void );

typedef struct PaVersionInfo {
    int versionMajor;
    int versionMinor;
    int versionSubMinor;
    /**
     This is currently the Git revision hash but may change in the future.
     The versionControlRevision is updated by running a script before compiling the library.
     If the update does not occur, this value may refer to an earlier revision.
    */
    const char *versionControlRevision;
    /** Version as a string, for example "PortAudio V19.5.0-devel, revision 1952M" */
    const char *versionText;
} PaVersionInfo;

const PaVersionInfo* Pa_GetVersionInfo( void );

typedef int PaError;
typedef enum PaErrorCode
{
    paNoError = 0,

    paNotInitialized = -10000,
    paUnanticipatedHostError,
    paInvalidChannelCount,
    paInvalidSampleRate,
    paInvalidDevice,
    paInvalidFlag,
    paSampleFormatNotSupported,
    paBadIODeviceCombination,
    paInsufficientMemory,
    paBufferTooBig,
    paBufferTooSmall,
    paNullCallback,
    paBadStreamPtr,
    paTimedOut,
    paInternalError,
    paDeviceUnavailable,
    paIncompatibleHostApiSpecificStreamInfo,
    paStreamIsStopped,
    paStreamIsNotStopped,
    paInputOverflowed,
    paOutputUnderflowed,
    paHostApiNotFound,
    paInvalidHostApi,
    paCanNotReadFromACallbackStream,
    paCanNotWriteToACallbackStream,
    paCanNotReadFromAnOutputOnlyStream,
    paCanNotWriteToAnInputOnlyStream,
    paIncompatibleStreamHostApi,
    paBadBufferPtr
} PaErrorCode;

const char *Pa_GetErrorText( PaError errorCode );
PaError Pa_Initialize( void );
PaError Pa_Terminate( void );

typedef int PaDeviceIndex;
typedef int PaHostApiIndex;
PaHostApiIndex Pa_GetHostApiCount( void );
PaHostApiIndex Pa_GetDefaultHostApi( void );

typedef enum PaHostApiTypeId
{
    paInDevelopment=0, /* use while developing support for a new host API */
    paDirectSound=1,
    paMME=2,
    paASIO=3,
    paSoundManager=4,
    paCoreAudio=5,
    paOSS=7,
    paALSA=8,
    paAL=9,
    paBeOS=10,
    paWDMKS=11,
    paJACK=12,
    paWASAPI=13,
    paAudioScienceHPI=14,
    paAudioIO=15
} PaHostApiTypeId;

typedef struct PaHostApiInfo
{
    /** this is struct version 1 */
    int structVersion;
    /** The well known unique identifier of this host API @see PaHostApiTypeId */
    PaHostApiTypeId type;
    /** A textual description of the host API for display on user interfaces. */
    const char *name;

    /**  The number of devices belonging to this host API. This field may be
     used in conjunction with Pa_HostApiDeviceIndexToDeviceIndex() to enumerate
     all devices for this host API.
     @see Pa_HostApiDeviceIndexToDeviceIndex
    */
    int deviceCount;

    /** The default input device for this host API. The value will be a
     device index ranging from 0 to (Pa_GetDeviceCount()-1), or paNoDevice
     if no default input device is available.
    */
    PaDeviceIndex defaultInputDevice;

    /** The default output device for this host API. The value will be a
     device index ranging from 0 to (Pa_GetDeviceCount()-1), or paNoDevice
     if no default output device is available.
    */
    PaDeviceIndex defaultOutputDevice;

} PaHostApiInfo;


const PaHostApiInfo * Pa_GetHostApiInfo( PaHostApiIndex hostApi );
PaHostApiIndex Pa_HostApiTypeIdToHostApiIndex( PaHostApiTypeId type );
PaDeviceIndex Pa_HostApiDeviceIndexToDeviceIndex( PaHostApiIndex hostApi,
        int hostApiDeviceIndex );

typedef struct PaHostErrorInfo{
    PaHostApiTypeId hostApiType;    /**< the host API which returned the error code */
    long errorCode;                 /**< the error code returned */
    const char *errorText;          /**< a textual description of the error if available, otherwise a zero-length string */
}PaHostErrorInfo;

const PaHostErrorInfo* Pa_GetLastHostErrorInfo( void );
PaDeviceIndex Pa_GetDeviceCount( void );
PaDeviceIndex Pa_GetDefaultInputDevice( void );
PaDeviceIndex Pa_GetDefaultOutputDevice( void );


typedef double PaTime;
typedef unsigned long PaSampleFormat;


typedef struct PaDeviceInfo
{
    int structVersion;  /* this is struct version 2 */
    const char *name;
    PaHostApiIndex hostApi; /**< note this is a host API index, not a type id*/

    int maxInputChannels;
    int maxOutputChannels;

    /** Default latency values for interactive performance. */
    PaTime defaultLowInputLatency;
    PaTime defaultLowOutputLatency;
    /** Default latency values for robust non-interactive applications (eg. playing sound files). */
    PaTime defaultHighInputLatency;
    PaTime defaultHighOutputLatency;

    double defaultSampleRate;
} PaDeviceInfo;


const PaDeviceInfo* Pa_GetDeviceInfo( PaDeviceIndex device );

typedef struct PaStreamParameters
{
    PaDeviceIndex device;
    int channelCount;
    PaSampleFormat sampleFormat;
    PaTime suggestedLatency;
    void *hostApiSpecificStreamInfo;
} PaStreamParameters;


PaError Pa_IsFormatSupported( const PaStreamParameters *inputParameters,
                              const PaStreamParameters *outputParameters,
                              double sampleRate );


typedef void PaStream;


typedef unsigned long PaStreamFlags;


typedef struct PaStreamCallbackTimeInfo{
    PaTime inputBufferAdcTime;  /**< The time when the first sample of the input buffer was captured at the ADC input */
    PaTime currentTime;         /**< The time when the stream callback was invoked */
    PaTime outputBufferDacTime; /**< The time when the first sample of the output buffer will output the DAC */
} PaStreamCallbackTimeInfo;

typedef unsigned long PaStreamCallbackFlags;



typedef enum PaStreamCallbackResult
{
    paContinue=0,   /**< Signal that the stream should continue invoking the callback and processing audio. */
    paComplete=1,   /**< Signal that the stream should stop invoking the callback and finish once all output samples have played. */
    paAbort=2       /**< Signal that the stream should stop invoking the callback and finish as soon as possible. */
} PaStreamCallbackResult;


typedef int PaStreamCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

typedef int (* __stdcall luaPaStreamCallback)(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );


PaError Pa_OpenStream( PaStream** stream,
                       const PaStreamParameters *inputParameters,
                       const PaStreamParameters *outputParameters,
                       double sampleRate,
                       unsigned long framesPerBuffer,
                       PaStreamFlags streamFlags,
                       PaStreamCallback *streamCallback,
                       void *userData );


PaError Pa_OpenDefaultStream( PaStream** stream,
                              int numInputChannels,
                              int numOutputChannels,
                              PaSampleFormat sampleFormat,
                              double sampleRate,
                              unsigned long framesPerBuffer,
                              PaStreamCallback *streamCallback,
                              void *userData );


PaError Pa_CloseStream( PaStream *stream );
typedef void PaStreamFinishedCallback( void *userData );
PaError Pa_SetStreamFinishedCallback( PaStream *stream, PaStreamFinishedCallback* streamFinishedCallback );
PaError Pa_StartStream( PaStream *stream );
PaError Pa_StopStream( PaStream *stream );
PaError Pa_AbortStream( PaStream *stream );
PaError Pa_IsStreamStopped( PaStream *stream );
PaError Pa_IsStreamActive( PaStream *stream );

typedef struct PaStreamInfo
{    
    int structVersion;
    PaTime inputLatency;
    PaTime outputLatency;
    double sampleRate;

} PaStreamInfo;


const PaStreamInfo* Pa_GetStreamInfo( PaStream *stream );
PaTime Pa_GetStreamTime( PaStream *stream );
double Pa_GetStreamCpuLoad( PaStream* stream );
PaError Pa_ReadStream( PaStream* stream,
                       void *buffer,
                       unsigned long frames );

PaError Pa_WriteStream( PaStream* stream,
                        const void *buffer,
                        unsigned long frames );

signed long Pa_GetStreamReadAvailable( PaStream* stream );
signed long Pa_GetStreamWriteAvailable( PaStream* stream );
PaError Pa_GetSampleSize( PaSampleFormat format );
void Pa_Sleep( long msec );
]]

paFloat32 =  0x00000001;
paInt32   =  0x00000002;
paInt24   =  0x00000004;
paInt16   =  0x00000008;
paInt8    =  0x00000010;
paUInt8    =  0x00000020;
paNonInterleaved = 0x80000000;

paNoFlag = 0;
paClipOff = 0x01;
paDitherOff = 0x02;
paNeverDropInput = 0x04;
paPrimeOutputBuffersUsingStreamCallback = 0x00000008;
paPlatformSpecificFlags = 0xFFFF0000;

paInputUnderflow  = 0x00000001;
paInputOverflow   = 0x00000002;
paOutputUnderflow = 0x00000004;
paOutputOverflow  = 0x00000008;
paPrimingOutput   = 0x00000010;

paFormatIsSupported = 0;
paFramesPerBufferUnspecified = 0;

portaudio = ffi.load('libportaudio.so')

portaudio.Pa_Initialize()
device = 14
numDevices = portaudio.Pa_GetDeviceCount()
print("Num Device=",numDevices)
for i=0,numDevices-1 do 
    info = portaudio.Pa_GetDeviceInfo(i)
    s = ffi.string(info.name)
    if("pulse" == s) then device = i end 
    print(s)
end


require('sineosc')
sine = SineOsc(44100,8192)
function callback( inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags, userData)
    local buffer = ffi.cast("float *",outputBuffer)        
    local n = tonumber(framesPerBuffer)    
    for i=0,n-1 do 
        local x = sine:Generate(440)
        buffer[i] = x        
    end
    return portaudio.paContinue
end

cb = ffi.cast("luaPaStreamCallback", callback)

outputParameters = ffi.new("struct PaStreamParameters")
outputParameters["device"] = device
if(outputParameters["device"] == paNoDevice) then 
    print('no device')
    os.exit(-1)
end
outputParameters["channelCount"] = 1
outputParameters["sampleFormat"] = paFloat32
outputParameters["suggestedLatency"] = portaudio.Pa_GetDeviceInfo( outputParameters.device )["defaultLowOutputLatency"]
outputParameters["hostApiSpecificStreamInfo"] = nil


require('addressof')

cb = ffi.cast("luaPaStreamCallback", callback)

err = portaudio.Pa_OpenStream(addressof.get_stream_address(), nil, ffi.cast("struct PaStreamParameters*",outputParameters), 44100,256,paClipOff,callback,nil)
if(err ~= portaudio.paNoError) then print(ffi.string(portaudio.Pa_GetErrorText(err))) os.exit(-1) end

err = portaudio.Pa_StartStream(addressof.get_stream())
if(err ~= portaudio.paNoError) then print(ffi.string(portaudio.Pa_GetErrorText(err))) os.exit(-1) end

addressof.RunLoop()

portaudio.Pa_StopStream(addressof.get_stream())
portaudio.Pa_Terminate()
