%{
#include "Gamma/AudioIO.h"
%}

%include "lua_fnptr.i"

namespace gam
{
    class AudioDevice{
    public:

        /// Stream mode
        enum StreamMode{
            INPUT	= 1,	/**< Input stream */
            OUTPUT	= 2		/**< Output stream */
        };
        
        AudioDevice(int deviceNum);
        AudioDevice(const std::string& nameKeyword, StreamMode stream = StreamMode(INPUT | OUTPUT));
        ~AudioDevice();

        bool valid() const;					///< Get whether device is valid
        int id() const;						///< Get device unique ID
        const char * name() const;			///< Get device name
        int channelsInMax() const;			///< Get maximum number of input channels supported
        int channelsOutMax() const;			///< Get maximum number of output channels supported
        double defaultSampleRate() const;	///< Get default sample rate
        
        bool hasInput() const;				///< Get whether device has input
        bool hasOutput() const;				///< Get whether device has output
        
        void print() const;					///< Prints info about specific i/o device to stdout

        static AudioDevice defaultInput();	///< Get system's default input device
        static AudioDevice defaultOutput();	///< Get system's default output device
        static int numDevices();			///< Get number of audio i/o devices available
        static void printAll();				///< Prints info about all available i/o devices to stdout
    };

    class AudioIOData {
    public:


        AudioIOData(void * user);
        virtual ~AudioIOData();

        
        
        %extend {
            bool Tick() const { return (*$self)(); }
        }    
        
        
        int frame() const;        
        float& bus(int chan) const;        
        float& bus(int chan, int frame) const;        
        float * busBuffer(int chan=0) const;        
        const float& in(int chan) const;        
        const float& in(int chan, int frame) const;
        const float * inBuffer(int chan=0) const;        
        float& out(int chan) const;        
        float& out(int chan, int frame) const;        
        float * outBuffer(int chan=0) const;                
        void sum(float v, int chan) const;        
        void sum(float v, int ch1, int ch2) const;        
        float& temp(int frame) const;
        float * tempBuffer() const;
        void * user() const;

        //template<class UserDataType>
        //UserDataType& user() const { return *(static_cast<UserDataType *>(mUser)); }

        int channelsIn () const;			///< Get effective number of input channels
        int channelsOut() const;			///< Get effective number of output channels
        int channelsBus() const;			///< Get number of allocated bus channels
        int channelsInDevice() const;		///< Get number of channels opened on input device
        int channelsOutDevice() const;		///< Get number of channels opened on output device
        int framesPerBuffer() const;		///< Get frames/buffer of audio I/O stream
        double framesPerSecond() const;		///< Get frames/second of audio I/O streams
        double fps() const;
        double secondsPerBuffer() const;	///< Get seconds/buffer of audio I/O stream
        double time() const;				///< Get current stream time in seconds
        double time(int frame) const;		///< Get current stream time in seconds of frame
        void user(void * v);
        void frame(int v);
        void zeroBus();						///< Zeros all the bus buffers
        void zeroOut();						///< Zeros all the internal output buffers
        AudioIOData& gain(float v);
        bool usingGain() const;
        float mGain, mGainPrev;

    };

    class AudioIO : public AudioIOData {
    public:

        AudioIO(
            int framesPerBuf=64, double framesPerSec=44100.0,
            void (* callback)(AudioIOData &) = 0, void * userData = 0,
            int outChans = 2, int inChans = 0 );

        virtual ~AudioIO();

        using AudioIOData::channelsIn;
        using AudioIOData::channelsOut;
        using AudioIOData::framesPerBuffer;
        using AudioIOData::framesPerSecond;

        AudioIO& append(AudioCallback& v);		
        AudioIO& prepend(AudioCallback& v);
        AudioIO& remove(AudioCallback& v);

        bool autoZeroOut() const;
        int channels(bool forOutput) const;
        bool clipOut() const;
        double cpu() const;	
        bool supportsFPS(double fps) const;			///< Return true if fps supported, otherwise false
        bool zeroNANs() const;						///< Returns whether to zero NANs in output buffer going to DAC
        
        void processAudio();						///< Call callback manually
        bool open();								///< Opens audio device.
        bool close();								///< Closes audio device. Will stop active IO.
        bool start();								///< Starts the audio IO.  Will open audio device if necessary.
        bool stop();								///< Stops the audio IO.

        void autoZeroOut(bool v);
        void channels(int num, bool forOutput);
        void channelsIn(int n);
        void channelsOut(int n);
        void channelsBus(int num);					///< Set number of bus channels
        void clipOut(bool v);
        void device(const AudioDevice& v);			///< Set input/output device (must be duplex)	
        void deviceIn(const AudioDevice& v);		///< Set input device
        void deviceOut(const AudioDevice& v);		///< Set output device
        void framesPerSecond(double v);				///< Set number of frames per second
        void framesPerBuffer(int n);				///< Set number of frames per processing buffer
        void zeroNANs(bool v);

        void print();								///< Prints info about current i/o devices to stdout.

        static const char * errorText(int errNum);		// Returns error string.
    };

    // will provide a lua callback
    class AudioCallback {
    public:
        virtual ~AudioCallback();
        virtual void onAudio(AudioIOData& io) = 0;
    };

}

%inline %{
    class LuaCallback : public gam::AudioCallback
    {
    public:
        SWIGLUA_REF cb;

        LuaCallback(SWIGLUA_REF ref) : cb(ref), gam::AudioCallback() {}

        void onAudio(AudioIOData & ib) {
            // todo call the callback 
        }
    };
%};