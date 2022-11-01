%{
#include "ADSR.h"    
%}


namespace stk {

    class ADSR : public Generator
    {
    public:

    //! ADSR envelope states.
    enum {
        ATTACK,   /*!< Attack */
        DECAY,    /*!< Decay */
        SUSTAIN,  /*!< Sustain */
        RELEASE,  /*!< Release */
        IDLE      /*!< Before attack / after release */
    };
    
    ADSR( void );
    ~ADSR( void );

    void keyOn( void );
    void keyOff( void );
    void setAttackRate( StkFloat rate );
    void setAttackTarget( StkFloat target );
    void setDecayRate( StkFloat rate );
    void setSustainLevel( StkFloat level );
    void setReleaseRate( StkFloat rate );
    void setAttackTime( StkFloat time );
    void setDecayTime( StkFloat time );
    void setReleaseTime( StkFloat time );
    void setAllTimes( StkFloat aTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime );
    void setTarget( StkFloat target );
    int getState( void ) const { return state_; };
    void setValue( StkFloat value );

    StkFloat lastOut( void ) const;
    StkFloat tick( void );
    StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

    };
} // stk namespace
