%{
#include "VoicForm.h"
%}
namespace stk {
    class VoicForm : public Instrmnt
    {
    public:

        VoicForm( void );
        ~VoicForm( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        bool setPhoneme( const char* phoneme );
        void setVoiced( StkFloat vGain );
        void setUnVoiced( StkFloat nGain );
        void setFilterSweepRate( unsigned int whichOne, StkFloat rate );
        void setPitchSweepRate( StkFloat rate );
        void speak( void );
        void quiet( void );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
}