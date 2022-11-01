%{
#include "Messager.h"
%}

namespace stk
{
    class Messager : public Stk
    {
    public:
      
        Messager();
        ~Messager();

        
        void popMessage( Skini::Message& message );
        void pushMessage( Skini::Message& message );

        bool setScoreFile( const char* filename );

        // non-realtime
        //bool startStdInput();
        //bool startSocketInput( int port=2001 );
        //bool startMidiInput( int port=0 );

    };

    /*
    // nested not supported
    struct Messager::MessagerData 
    {
        Skini skini;
        std::queue<Skini::Message> queue;
        unsigned int queueLimit;
        int sources;
    #if defined(__STK_REALTIME__)
        Mutex mutex;
        RtMidiIn *midi;
        TcpServer *socket;
        std::vector<int> fd;
        fd_set mask;
    #endif
        // Default constructor.
        MessagerData();
    };
    */
}