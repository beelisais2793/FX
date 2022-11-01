%{
#include "RtAudio.h"
%}

typedef unsigned long RtAudioFormat;
static const RtAudioFormat RTAUDIO_SINT8;
static const RtAudioFormat RTAUDIO_SINT16;
static const RtAudioFormat RTAUDIO_SINT24;
static const RtAudioFormat RTAUDIO_SINT32;
static const RtAudioFormat RTAUDIO_FLOAT32;
static const RtAudioFormat RTAUDIO_FLOAT64;

typedef unsigned int RtAudioStreamFlags;
static const RtAudioStreamFlags RTAUDIO_NONINTERLEAVED;
static const RtAudioStreamFlags RTAUDIO_MINIMIZE_LATENCY;
static const RtAudioStreamFlags RTAUDIO_HOG_DEVICE;
static const RtAudioStreamFlags RTAUDIO_SCHEDULE_REALTIME;
static const RtAudioStreamFlags RTAUDIO_ALSA_USE_DEFAULT;
static const RtAudioStreamFlags RTAUDIO_JACK_DONT_CONNECT;

typedef unsigned int RtAudioStreamStatus;
static const RtAudioStreamStatus RTAUDIO_INPUT_OVERFLOW;
static const RtAudioStreamStatus RTAUDIO_OUTPUT_UNDERFLOW;


class RtAudioError 
{
 public:

  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    MEMORY_ERROR,      /*!< An error occurred during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    INVALID_USE,       /*!< The function was called incorrectly. */
    DRIVER_ERROR,      /*!< A system driver error occurred. */
    SYSTEM_ERROR,      /*!< A system error occurred. */
    THREAD_ERROR       /*!< A thread error occurred. */
  };


  RtAudioError( const std::string& message,
                Type type = RtAudioError::UNSPECIFIED );
  
  virtual void printMessage( void ) const;
  virtual const Type& getType(void) const;
  virtual const std::string getMessage(void) const;
};


class RtAudio
{
 public:

  enum Api {
    UNSPECIFIED,    /*!< Search for a working compiled API. */
    LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
    LINUX_PULSE,    /*!< The Linux PulseAudio API. */
    LINUX_OSS,      /*!< The Linux Open Sound System API. */
    UNIX_JACK,      /*!< The Jack Low-Latency Audio Server API. */
    MACOSX_CORE,    /*!< Macintosh OS-X Core Audio API. */
    WINDOWS_WASAPI, /*!< The Microsoft WASAPI API. */
    WINDOWS_ASIO,   /*!< The Steinberg Audio Stream I/O API. */
    WINDOWS_DS,     /*!< The Microsoft DirectSound API. */
    RTAUDIO_DUMMY,  /*!< A compilable but non-functional API. */
    NUM_APIS        /*!< Number of values in this enum. */
  };

  
  struct DeviceInfo;
  struct StreamParameters;
  struct StreamOptions;  
  
  static std::string getVersion( void );
  static void getCompiledApi( std::vector<RtAudio::Api> &apis );
  static std::string getApiName( RtAudio::Api api );
  static std::string getApiDisplayName( RtAudio::Api api );
  static RtAudio::Api getCompiledApiByName( const std::string &name );

  RtAudio( RtAudio::Api api=UNSPECIFIED );
  ~RtAudio();

  RtAudio::Api getCurrentApi( void );

  unsigned int getDeviceCount( void );
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device );
  unsigned int getDefaultOutputDevice( void );
  unsigned int getDefaultInputDevice( void );

  void openStream( RtAudio::StreamParameters *outputParameters,
                   RtAudio::StreamParameters *inputParameters,
                   RtAudioFormat format, unsigned int sampleRate,
                   unsigned int *bufferFrames, RtAudioCallback callback,
                   void *userData = NULL, RtAudio::StreamOptions *options = NULL, RtAudioErrorCallback errorCallback = NULL );

  void closeStream( void );
  void startStream( void );
  void stopStream( void );
  void abortStream( void );
  bool isStreamOpen( void ) const;
  bool isStreamRunning( void ) const;
  double getStreamTime( void );
  void setStreamTime( double time );
  long getStreamLatency( void );
  unsigned int getStreamSampleRate( void );
  void showWarnings( bool value = true );
};


// it can't be in the class for swig to work  
struct RtAudio::DeviceInfo {
    bool probed;                  /*!< true if the device capabilities were successfully probed. */
    std::string name;             /*!< Character string device identifier. */
    unsigned int outputChannels{};  /*!< Maximum output channels supported by device. */
    unsigned int inputChannels{};   /*!< Maximum input channels supported by device. */
    unsigned int duplexChannels{};  /*!< Maximum simultaneous input/output channels supported by device. */
    bool isDefaultOutput{false};         /*!< true if this is the default output device. */
    bool isDefaultInput{false};          /*!< true if this is the default input device. */
    std::vector<unsigned int> sampleRates; /*!< Supported sample rates (queried from list of standard rates). */
    unsigned int preferredSampleRate{}; /*!< Preferred sample rate, e.g. for WASAPI the system sample rate. */
    RtAudioFormat nativeFormats{};  /*!< Bit mask of supported data formats. */
};

struct RtAudio::StreamParameters {
    unsigned int deviceId{};     /*!< Device index (0 to getDeviceCount() - 1). */
    unsigned int nChannels{};    /*!< Number of channels. */
    unsigned int firstChannel{}; /*!< First channel index on device (default = 0). */
  };

struct RtAudio::StreamOptions {
    RtAudioStreamFlags flags{};      /*!< A bit-mask of stream flags (RTAUDIO_NONINTERLEAVED, RTAUDIO_MINIMIZE_LATENCY, RTAUDIO_HOG_DEVICE, RTAUDIO_ALSA_USE_DEFAULT). */
    unsigned int numberOfBuffers{};  /*!< Number of stream buffers. */
    std::string streamName;        /*!< A stream name (currently used only in Jack). */
    int priority{};                  /*!< Scheduling priority of callback thread (only used with flag RTAUDIO_SCHEDULE_REALTIME). */
  };


class S24 
{
public:
    S24();

    S24& operator = ( const int& i );
    S24( const double& d );
    S24( const float& f );
    S24( const signed short& s );
    S24( const char& c );
    int asInt();
};

class RTAUDIO_DLL_PUBLIC RtApi
{
public:

  RtApi();
  virtual ~RtApi();
  virtual RtAudio::Api getCurrentApi( void ) = 0;
  virtual unsigned int getDeviceCount( void ) = 0;
  virtual RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) = 0;
  virtual unsigned int getDefaultInputDevice( void );
  virtual unsigned int getDefaultOutputDevice( void );
  void openStream( RtAudio::StreamParameters *outputParameters,
                   RtAudio::StreamParameters *inputParameters,
                   RtAudioFormat format, unsigned int sampleRate,
                   unsigned int *bufferFrames, RtAudioCallback callback,
                   void *userData, RtAudio::StreamOptions *options,
                   RtAudioErrorCallback errorCallback );
  virtual void closeStream( void );
  virtual void startStream( void ) = 0;
  virtual void stopStream( void ) = 0;
  virtual void abortStream( void ) = 0;
  long getStreamLatency( void );
  unsigned int getStreamSampleRate( void );
  virtual double getStreamTime( void );
  virtual void setStreamTime( double time );
  bool isStreamOpen( void ) const;
  bool isStreamRunning( void ) const;
  void showWarnings( bool value );
};

class RtApiCore: public RtApi
{
public:

  RtApiCore();
  ~RtApiCore();
  RtAudio::Api getCurrentApi( void ) override { return RtAudio::MACOSX_CORE; }
  unsigned int getDeviceCount( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  unsigned int getDefaultOutputDevice( void ) override;
  unsigned int getDefaultInputDevice( void ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;

  // This function is intended for internal use only.  It must be
  // public because it is called by the internal callback handler,
  // which is not a member of RtAudio.  External use of this function
  // will most likely produce highly undesirable results!
  bool callbackEvent( AudioDeviceID deviceId,
                      const AudioBufferList *inBufferList,
                      const AudioBufferList *outBufferList );

  private:

  bool probeDeviceOpen( unsigned int device, StreamMode mode, unsigned int channels, 
                        unsigned int firstChannel, unsigned int sampleRate,
                        RtAudioFormat format, unsigned int *bufferSize,
                        RtAudio::StreamOptions *options ) override;
  static const char* getErrorCode( OSStatus code );
};











