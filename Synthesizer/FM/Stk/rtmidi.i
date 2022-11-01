%{
#include "RtMidi.h"
%}

// it doesn't really go through the script language and you'll just exception was thrown error
class RtMidiError 
{
 public:


  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    MEMORY_ERROR,      /*!< An error occured during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    INVALID_USE,       /*!< The function was called incorrectly. */
    DRIVER_ERROR,      /*!< A system driver error occured. */
    SYSTEM_ERROR,      /*!< A system error occured. */
    THREAD_ERROR       /*!< A thread error occured. */
  };


  RtMidiError( const std::string& message, Type type = RtMidiError::UNSPECIFIED ) throw();
  virtual ~RtMidiError( void ) throw();

  virtual void printMessage( void ) const throw();
  virtual const Type& getType( void ) const throw();
  virtual const std::string& getMessage( void ) const throw();
  virtual const char* what( void ) const throw();
};


class RtMidi
{
 public:

     RtMidi(RtMidi&& other) noexcept;
  
  enum Api {
    UNSPECIFIED,    /*!< Search for a working compiled API. */
    MACOSX_CORE,    /*!< Macintosh OS-X CoreMIDI API. */
    LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
    UNIX_JACK,      /*!< The JACK Low-Latency MIDI Server API. */
    WINDOWS_MM,     /*!< The Microsoft Multimedia MIDI API. */
    RTMIDI_DUMMY,   /*!< A compilable but non-functional API. */
    WEB_MIDI_API,   /*!< W3C Web MIDI API. */
    NUM_APIS        /*!< Number of values in this enum. */
  };

  
  static std::string getVersion( void ) throw();
  static void getCompiledApi( std::vector<RtMidi::Api> &apis ) throw();
  static std::string getApiName( RtMidi::Api api );
  static std::string getApiDisplayName( RtMidi::Api api );
  static RtMidi::Api getCompiledApiByName( const std::string &name );
  virtual void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi" ) ) = 0;
  virtual void openVirtualPort( const std::string &portName = std::string( "RtMidi" ) ) = 0;
  virtual unsigned int getPortCount() = 0;
  virtual std::string getPortName( unsigned int portNumber = 0 ) = 0;
  virtual void closePort( void ) = 0;
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName );
  virtual bool isPortOpen( void ) const = 0;
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 ) = 0;
};


class RtMidiIn : public RtMidi
{
 public:
  
  typedef void (*RtMidiCallback)( double timeStamp, std::vector<unsigned char> *message, void *userData );

  RtMidiIn( RtMidi::Api api=UNSPECIFIED,
            const std::string& clientName = "RtMidi Input Client",
            unsigned int queueSizeLimit = 100 );

  RtMidiIn(RtMidiIn&& other) noexcept : RtMidi(std::move(other));

  ~RtMidiIn ( void ) throw();
  RtMidi::Api getCurrentApi( void ) throw();
  void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi Input" ) );
  void openVirtualPort( const std::string &portName = std::string( "RtMidi Input" ) );
  void setCallback( RtMidiCallback callback, void *userData = 0 );
  void cancelCallback();
  void closePort( void );
  virtual bool isPortOpen() const;
  unsigned int getPortCount();
  std::string getPortName( unsigned int portNumber = 0 );
  void ignoreTypes( bool midiSysex = true, bool midiTime = true, bool midiSense = true );
  double getMessage( std::vector<unsigned char> *message );
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 );
  virtual void setBufferSize( unsigned int size, unsigned int count );
};


class RtMidiOut : public RtMidi
{
 public:

  RtMidiOut( RtMidi::Api api=UNSPECIFIED,
             const std::string& clientName = "RtMidi Output Client" );

  RtMidiOut(RtMidiOut&& other) noexcept : RtMidi(std::move(other));
  ~RtMidiOut( void ) throw();
  RtMidi::Api getCurrentApi( void ) throw();

  void openPort( unsigned int portNumber = 0, const std::string &portName = std::string( "RtMidi Output" ) );
  void closePort( void );
  virtual bool isPortOpen() const;
  void openVirtualPort( const std::string &portName = std::string( "RtMidi Output" ) );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber = 0 );
  void sendMessage( const std::vector<unsigned char> *message );
  void sendMessage( const unsigned char *message, size_t size );
  virtual void setErrorCallback( RtMidiErrorCallback errorCallback = NULL, void *userData = 0 );

};

// they can't be used directly and not sure it needs to be wrapped even
class MidiApi
{
 public:

  MidiApi();
  virtual ~MidiApi();
  virtual RtMidi::Api getCurrentApi( void ) = 0;
  virtual void openPort( unsigned int portNumber, const std::string &portName ) = 0;
  virtual void openVirtualPort( const std::string &portName ) = 0;
  virtual void closePort( void ) = 0;
  virtual void setClientName( const std::string &clientName ) = 0;
  virtual void setPortName( const std::string &portName ) = 0;
  virtual unsigned int getPortCount( void ) = 0;
  virtual std::string getPortName( unsigned int portNumber ) = 0;
  inline bool isPortOpen() const;
  void setErrorCallback( RtMidiErrorCallback errorCallback, void *userData );
  void error( RtMidiError::Type type, std::string errorString );
};


class MidiInApi : public MidiApi
{
 public:

  MidiInApi( unsigned int queueSizeLimit );
  virtual ~MidiInApi( void );
  void setCallback( RtMidiIn::RtMidiCallback callback, void *userData );
  void cancelCallback( void );
  virtual void ignoreTypes( bool midiSysex, bool midiTime, bool midiSense );
  double getMessage( std::vector<unsigned char> *message );
  virtual void setBufferSize( unsigned int size, unsigned int count );

  struct MidiMessage;
  struct MidiQueue;
  struct RtMidiInData;
};

  struct MidiInApi::MidiMessage {
    std::vector<unsigned char> bytes;

    //! Time in seconds elapsed since the previous message
    double timeStamp;

    // Default constructor.
    MidiMessage()
      : bytes(0), timeStamp(0.0) {}
  };

  struct MidiInApi::MidiQueue {
    unsigned int front;
    unsigned int back;
    unsigned int ringSize;
    MidiMessage *ring;

      // Default constructor.
    MidiQueue()
      : front(0), back(0), ringSize(0), ring(0) {}
    bool push( const MidiMessage& );
    bool pop( std::vector<unsigned char>*, double* );
    unsigned int size( unsigned int *back=0, unsigned int *front=0 );
  };

  // The RtMidiInData structure is used to pass private class data to
  // the MIDI input handling function or thread.
  struct MidiInApi::RtMidiInData {
    MidiQueue queue;
    MidiMessage message;
    unsigned char ignoreFlags;
    bool doInput;
    bool firstMessage;
    void *apiData;
    bool usingCallback;
    RtMidiIn::RtMidiCallback userCallback;
    void *userData;
    bool continueSysex;
    unsigned int bufferSize;
    unsigned int bufferCount;
    // Default constructor.
    RtMidiInData()
      : ignoreFlags(7), doInput(false), firstMessage(true), apiData(0), usingCallback(false),
        userCallback(0), userData(0), continueSysex(false), bufferSize(1024), bufferCount(4) {}
  };


class MidiOutApi : public MidiApi
{
 public:

  MidiOutApi( void );
  virtual ~MidiOutApi( void );
  virtual void sendMessage( const unsigned char *message, size_t size ) = 0;
};