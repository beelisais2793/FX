%{
#include "StifKarp.h"
%}
namespace stk {

    /***************************************************/
    /*! \class StifKarp
        \brief STK plucked stiff string instrument.
        This class implements a simple plucked string
        algorithm (Karplus Strong) with enhancements
        (Jaffe-Smith, Smith, and others), including
        string stiffness and pluck position controls.
        The stiffness is modeled with allpass filters.
        This is a digital waveguide model, making its
        use possibly subject to patents held by
        Stanford University, Yamaha, and others.
        Control Change Numbers:
        - Pickup Position = 4
        - String Sustain = 11
        - String Stretch = 1
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class StifKarp : public Instrmnt
    {
    public:

        StifKarp( StkFloat lowestFrequency = 10.0 );
        ~StifKarp( void );

        void clear( void );
        void setFrequency( StkFloat frequency );
        void setStretch( StkFloat stretch );
        void setPickupPosition( StkFloat position );
        void setBaseLoopGain( StkFloat aGain );
        void pluck( StkFloat amplitude );
        void noteOn( StkFloat frequency, StkFloat amplitude );
        void noteOff( StkFloat amplitude );
        void controlChange( int number, StkFloat value );
        StkFloat tick( unsigned int channel = 0 );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    };
};