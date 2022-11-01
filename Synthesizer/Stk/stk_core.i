// mostly, stk can be included directly by swig without much problem.
// but I decided to implement all the interface from the header like this as a reference for the interface
%{
#include "Effect.h"
#include "Filter.h"
#include "Generator.h"
#include "Instrmnt.h"
#include "Filter.h"
#include "Function.h"
#include "FM.h"
%}

namespace stk {

    typedef float StkFloat;

    class StkError 
    {
    public:
        enum Type {
            STATUS,
            WARNING,
            DEBUG_PRINT,
            MEMORY_ALLOCATION,
            MEMORY_ACCESS,
            FUNCTION_ARGUMENT,
            FILE_NOT_FOUND,
            FILE_UNKNOWN_FORMAT,
            FILE_ERROR,
            PROCESS_THREAD,
            PROCESS_SOCKET,
            PROCESS_SOCKET_IPADDR,
            AUDIO_SYSTEM,
            MIDI_SYSTEM,
            UNSPECIFIED
        };


        StkError(const std::string& message, Type type = StkError::UNSPECIFIED);    
        virtual ~StkError(void) throw();

        virtual void printMessage(void);
        virtual const Type& getType(void);
        virtual const std::string& getMessage(void);
        virtual const char *getMessageCString(void);
        virtual const char *what(void) const throw();
    };


    class Stk
    {
    public:

        typedef unsigned long StkFormat;
        static const StkFormat STK_SINT8;   /*!< -128 to +127 */
        static const StkFormat STK_SINT16;  /*!< -32768 to +32767 */
        static const StkFormat STK_SINT24;  /*!< Lower 3 bytes of 32-bit signed integer. */
        static const StkFormat STK_SINT32;  /*!< -2147483648 to +2147483647. */
        static const StkFormat STK_FLOAT32; /*!< Normalized between plus/minus 1.0. */
        static const StkFormat STK_FLOAT64; /*!< Normalized between plus/minus 1.0. */

        static StkFloat sampleRate( void );
        static void setSampleRate( StkFloat rate );   
        void ignoreSampleRateChange( bool ignore = true );
        static void  clear_alertList();    
        static std::string rawwavePath(void);
        static void setRawwavePath( std::string path );
        static void swap16( unsigned char *ptr );
        static void swap32( unsigned char *ptr );
        static void swap64( unsigned char *ptr );
        static void sleep( unsigned long milliseconds );
        static bool inRange( StkFloat value, StkFloat min, StkFloat max );
        static void handleError( const char *message, StkError::Type type );
        static void handleError( std::string message, StkError::Type type );
        static void showWarnings( bool status );
        static void printErrors( bool status );
    };

    class StkFrames
    {
    public:

        StkFrames( unsigned int nFrames = 0, unsigned int nChannels = 0 );
        StkFrames( const StkFloat& value, unsigned int nFrames, unsigned int nChannels );
        virtual ~StkFrames();

        StkFrames( const StkFrames& f );
        virtual StkFrames& operator= ( const StkFrames& f );
        
        %extend {
            StkFloat __getitem__ (size_t n) { return (*$self)[n]; }
            void     __setitem__ (size_t n, StkFloat value) { (*$self)[n] = value; }
        }

        StkFrames operator+(const StkFrames &frames) const;
        StkFrames& operator+= ( StkFrames& f );
        StkFrames& operator*= ( StkFrames& f );
        StkFrames operator* ( StkFloat v ) const;
        friend StkFrames operator*(StkFloat v, const StkFrames& f);
        StkFrames& operator*= ( StkFloat v );


        
        %extend {
            StkFloat& get(size_t frame, unsigned int channel) { return (*$self)(frame,channel); }
        }
        
        StkFloat interpolate( StkFloat frame, unsigned int channel = 0 ) const;
        
        size_t size() const;
        bool empty() const;

        virtual void resize( size_t nFrames, unsigned int nChannels = 1 );
        virtual void resize( size_t nFrames, unsigned int nChannels, StkFloat value );
        StkFrames& getChannel(unsigned int channel,StkFrames& destinationFrames, unsigned int destinationChannel) const;
        void setChannel(unsigned int channel,const StkFrames &sourceFrames,unsigned int sourceChannel);

        unsigned int channels( void ) const;
        unsigned int frames( void ) const;

        void setDataRate( StkFloat rate );
        StkFloat dataRate( void ) const;
    };

    // Here are a few other useful typedefs.
    typedef unsigned short UINT16;
    typedef unsigned int UINT32;
    typedef signed short SINT16;
    typedef signed int SINT32;
    typedef float FLOAT32;
    typedef double FLOAT64;
    
    const StkFloat SRATE;
    const unsigned int RT_BUFFER_SIZE;
    const StkFloat PI           ;
    const StkFloat TWO_PI       ;
    const StkFloat ONE_OVER_128 ;


    class Effect : public Stk
    {
    public:
        
        Effect( void );
        unsigned int channelsOut( void ) const;
        const StkFrames& lastFrame( void ) const;
        
        virtual void clear() = 0;
        virtual void setEffectMix( StkFloat mix );
    };

    
    class Filter : public Stk
    {
    public:
    
    
        Filter( void );
        unsigned int channelsIn( void ) const;
        unsigned int channelsOut( void ) const;

        virtual void clear( void );
        void setGain( StkFloat gain );
        StkFloat getGain( void ) const;
        StkFloat phaseDelay( StkFloat frequency );
        const StkFrames& lastFrame( void ) const;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };

    class Generator : public Stk
    {
    public:
        Generator( void );

        unsigned int channelsOut( void ) const;
        const StkFrames& lastFrame( void ) const ;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };

    class Instrmnt : public Stk
    {
    public:

        Instrmnt( void );
        virtual void clear( void );

        virtual void noteOn( StkFloat frequency, StkFloat amplitude ) = 0;
        virtual void noteOff( StkFloat amplitude ) = 0;
        virtual void setFrequency( StkFloat frequency );
        virtual void controlChange(int number, StkFloat value);

        unsigned int channelsOut( void ) const;
        const StkFrames& lastFrame( void ) const;
        StkFloat lastOut( unsigned int channel = 0 );

        virtual StkFloat tick( unsigned int channel = 0 ) = 0;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };

    class Function : public Stk
    {
    public:
        Function( void );
        StkFloat lastOut( void ) const;
        virtual StkFloat tick( StkFloat input ) = 0;
    };

    class FM : public Instrmnt
    {
    public:
    
        FM( unsigned int operators = 4 );
        virtual ~FM( void );

        void loadWaves( const char **filenames );
        virtual void setFrequency( StkFloat frequency );
        void setRatio( unsigned int waveIndex, StkFloat ratio );
        void setGain( unsigned int waveIndex, StkFloat gain );
        void setModulationSpeed( StkFloat mSpeed );
        void setModulationDepth( StkFloat mDepth );
        void setControl1( StkFloat cVal );
        void setControl2( StkFloat cVal );
        void keyOn( void );
        void keyOff( void );
        void noteOff( StkFloat amplitude );
        virtual void controlChange( int number, StkFloat value );
        virtual StkFloat tick( unsigned int ) = 0;
        virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 0 ) = 0;
    };


} // stk namespace