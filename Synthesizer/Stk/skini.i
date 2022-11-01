%{
    #include "Skini.h"
%}
 namespace stk
 {
    class Skini : public Stk
    {
    public:

        struct Message;
        
        Skini();
        ~Skini();

        bool setFile( std::string fileName );
        long nextMessage( Skini::Message& message );
        long parseString( std::string& line, Skini::Message& message );
        static std::string whatsThisType(long type);
        static std::string whatsThisController(long number);
    };

    //! A static table of equal-tempered MIDI to frequency (Hz) values.
    static const double Midi2Pitch[129];
            
    struct SKINI::Message 
    { 
        long type;                         /*!< The message type, as defined in SKINImsg.h. */
        long channel;                      /*!< The message channel (not limited to 16!). */
        StkFloat time;                     /*!< The message time stamp in seconds (delta or absolute). */
        std::vector<StkFloat> floatValues; /*!< The message values read as floats (values are type-specific). */
        std::vector<long> intValues;       /*!< The message values read as ints (number and values are type-specific). */
        std::string remainder;             /*!< Any remaining message data, read as ascii text. */

        // Default constructor.
        Message();
    };


}