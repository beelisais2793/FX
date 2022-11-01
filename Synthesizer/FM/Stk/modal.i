%{
#include "Modal.h"
%}

namespace stk {

    class Modal : public Instrmnt
    {
    public:

        Modal( unsigned int modes = 4 );
        virtual ~Modal( void );

        void clear( void );
        virtual void setFrequency( StkFloat frequency );
        void setRatioAndRadius( unsigned int modeIndex, StkFloat ratio, StkFloat radius );
        void setMasterGain( StkFloat aGain );
        void setDirectGain( StkFloat aGain );
        void setModeGain( unsigned int modeIndex, StkFloat gain );

        virtual void strike( StkFloat amplitude );
        void damp( StkFloat amplitude );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        virtual void controlChange( int number, StkFloat value ) = 0;
        StkFloat tick( unsigned int channel = 0 );

        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };

    class ModalBar : public Modal
    {
    public:
        
        ModalBar( void );
        ~ModalBar( void );

        void setStickHardness( StkFloat hardness );
        void setStrikePosition( StkFloat position );
        void setPreset( int preset );
        void setModulationDepth( StkFloat mDepth );
        void controlChange( int number, StkFloat value );
    };
}