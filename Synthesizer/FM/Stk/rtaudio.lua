ffi = require('ffi')
ffi.cdef[[

typedef unsigned long rtaudio_format_t;


typedef unsigned int rtaudio_stream_flags_t;

typedef enum RTAudioFormat
{
    RTAUDIO_FORMAT_SINT8=0x1,
    RTAUDIO_FORMAT_SINT16=0x2,
    RTAUDIO_FORMAT_SINT24=0x4,
    RTAUDIO_FORMAT_SINT32=0x8,
    RTAUDIO_FORMAT_FLOAT32=0x10,
    RTAUDIO_FORMAT_FLOAT64=0x20,
};

typedef enum RTAudioFlags {
    RTAUDIO_FLAGS_NONINTERLEAVED=0x1,
    RTAUDIO_FLAGS_MINIMIZE_LATENCY=0x2,
    RTAUDIO_FLAGS_HOG_DEVICE =0x4,
    RTAUDIO_FLAGS_SCHEDULE_REALTIME= 0x8,
    RTAUDIO_FLAGS_ALSA_USE_DEFAULT=0x10,
    RTAUDIO_FLAGS_JACK_DONT_CONNEC = 0x20,
};


typedef unsigned int rtaudio_stream_status_t;

typedef int (*rtaudio_cb_t)(void *out, void *in, unsigned int nFrames,
                            double stream_time, rtaudio_stream_status_t status,
                            void *userdata);

typedef enum rtaudio_error {
  RTAUDIO_ERROR_NONE = -1,         /*!< No error. */
  RTAUDIO_ERROR_WARNING,           /*!< A non-critical error. */
  RTAUDIO_ERROR_DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
  RTAUDIO_ERROR_UNSPECIFIED,       /*!< The default, unspecified error type. */
  RTAUDIO_ERROR_NO_DEVICES_FOUND,  /*!< No devices found on system. */
  RTAUDIO_ERROR_INVALID_DEVICE,    /*!< An invalid device ID was specified. */
  RTAUDIO_ERROR_MEMORY_ERROR,      /*!< An error occurred during memory allocation. */
  RTAUDIO_ERROR_INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
  RTAUDIO_ERROR_INVALID_USE,       /*!< The function was called incorrectly. */
  RTAUDIO_ERROR_DRIVER_ERROR,      /*!< A system driver error occurred. */
  RTAUDIO_ERROR_SYSTEM_ERROR,      /*!< A system error occurred. */
  RTAUDIO_ERROR_THREAD_ERROR,      /*!< A thread error occurred. */
} rtaudio_error_t;

typedef void (*rtaudio_error_cb_t)(rtaudio_error_t err, const char *msg);

//! Audio API specifier.  See \ref RtAudio::Api.
typedef enum rtaudio_api {
  RTAUDIO_API_UNSPECIFIED,    /*!< Search for a working compiled API. */
  RTAUDIO_API_LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
  RTAUDIO_API_LINUX_PULSE,    /*!< The Linux PulseAudio API. */
  RTAUDIO_API_LINUX_OSS,      /*!< The Linux Open Sound System API. */
  RTAUDIO_API_UNIX_JACK,      /*!< The Jack Low-Latency Audio Server API. */
  RTAUDIO_API_MACOSX_CORE,    /*!< Macintosh OS-X Core Audio API. */
  RTAUDIO_API_WINDOWS_WASAPI, /*!< The Microsoft WASAPI API. */
  RTAUDIO_API_WINDOWS_ASIO,   /*!< The Steinberg Audio Stream I/O API. */
  RTAUDIO_API_WINDOWS_DS,     /*!< The Microsoft DirectSound API. */
  RTAUDIO_API_DUMMY,          /*!< A compilable but non-functional API. */
  RTAUDIO_API_NUM,            /*!< Number of values in this enum. */
} rtaudio_api_t;


//! The public device information structure for returning queried values.
//! See \ref RtAudio::DeviceInfo.
typedef struct rtaudio_device_info {
  int probed;
  unsigned int output_channels;
  unsigned int input_channels;
  unsigned int duplex_channels;

  int is_default_output;
  int is_default_input;

  rtaudio_format_t native_formats;

  unsigned int preferred_sample_rate;
  int sample_rates[16];

  char name[512];
} rtaudio_device_info_t;

//! The structure for specifying input or output stream parameters.
//! See \ref RtAudio::StreamParameters.
typedef struct rtaudio_stream_parameters {
  unsigned int device_id;
  unsigned int num_channels;
  unsigned int first_channel;
} rtaudio_stream_parameters_t;

//! The structure for specifying stream options.
//! See \ref RtAudio::StreamOptions.
typedef struct rtaudio_stream_options {
  rtaudio_stream_flags_t flags;
  unsigned int num_buffers;
  int priority;
  char name[512];
} rtaudio_stream_options_t;

typedef struct rtaudio *rtaudio_t;

const char *rtaudio_version(void);
unsigned int rtaudio_get_num_compiled_apis(void);
const rtaudio_api_t *rtaudio_compiled_api(void);
const char *rtaudio_api_name(rtaudio_api_t api);
const char *rtaudio_api_display_name(rtaudio_api_t api);
rtaudio_api_t rtaudio_compiled_api_by_name(const char *name);
const char *rtaudio_error(rtaudio_t audio);
rtaudio_error_t rtaudio_error_type(rtaudio_t audio);
rtaudio_t rtaudio_create(rtaudio_api_t api);
void rtaudio_destroy(rtaudio_t audio);
rtaudio_api_t rtaudio_current_api(rtaudio_t audio);
int rtaudio_device_count(rtaudio_t audio);
rtaudio_device_info_t rtaudio_get_device_info(rtaudio_t audio,int i);
unsigned int rtaudio_get_default_output_device(rtaudio_t audio);
unsigned int rtaudio_get_default_input_device(rtaudio_t audio);
int rtaudio_open_stream(rtaudio_t audio, rtaudio_stream_parameters_t *output_params,
                    rtaudio_stream_parameters_t *input_params,
                    rtaudio_format_t format, unsigned int sample_rate,
                    unsigned int *buffer_frames, rtaudio_cb_t cb,
                    void *userdata, rtaudio_stream_options_t *options,
                    rtaudio_error_cb_t errcb);

void rtaudio_close_stream(rtaudio_t audio);
int rtaudio_start_stream(rtaudio_t audio);
int rtaudio_stop_stream(rtaudio_t audio);
int rtaudio_abort_stream(rtaudio_t audio);
int rtaudio_is_stream_open(rtaudio_t audio);
int rtaudio_is_stream_running(rtaudio_t audio);
double rtaudio_get_stream_time(rtaudio_t audio);
void rtaudio_set_stream_time(rtaudio_t audio, double time);
int rtaudio_get_stream_latency(rtaudio_t audio);
unsigned int rtaudio_get_stream_sample_rate(rtaudio_t audio);
void rtaudio_show_warnings(rtaudio_t audio, int show);

void sleep(int);
]]

rtaudio = ffi.load("rtaudio")

RTAUDIO_STATUS_INPUT_OVERFLOW  = 0x1
RTAUDIO_STATUS_OUTPUT_UNDERFLOW = 0x2

apis = rtaudio.rtaudio_get_num_compiled_apis()
print("num apis=",apis)

audio = rtaudio.rtaudio_create(rtaudio.RTAUDIO_API_LINUX_PULSE)

numDevices = rtaudio.rtaudio_device_count(audio)
print("num devices=",numDevices)
for i =0,numDevices-1 do 
    info = rtaudio.rtaudio_get_device_info(audio,i)    
    print(ffi.string(info.name))
end

params = ffi.new("struct rtaudio_stream_parameters")
params["device_id"] = rtaudio.rtaudio_get_default_output_device(audio)
params["num_channels"] = 2
params["first_channel"] = 0

require('sineosc')
sine = SineOsc(44100,8192)
function callback(output,input,nFrames,stream_time,status,userData)                                
    local buffer = ffi.cast("float *",output)        
    local n = tonumber(nFrames)    
    for i=0,n-1 do 
        local x = sine:Generate(440)
        buffer[2*i] = x        
        buffer[2*i+1] = x
    end 
    return 0
end 

cb = ffi.cast("rtaudio_cb_t",callback)
frames = ffi.new("unsigned int[1]",256)

stream = rtaudio.rtaudio_open_stream(audio,ffi.cast("struct rtaudio_stream_parameters*",params),nil,rtaudio.RTAUDIO_FORMAT_FLOAT32,44100,frames,cb,nil,nil,nil)
if(stream ~= rtaudio.RTAUDIO_ERROR_NONE) then 
    print(ffi.string(rtaudio.rtaudio_error(audio)))
    os.exit(-1)
end
rtaudio.rtaudio_start_stream(audio)
rtaudio.sleep(5)
cb:free()