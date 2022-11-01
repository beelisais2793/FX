%{
#include "TapDelay.h"
%}

namespace stk {

    /***************************************************/
    /*! \class TapDelay
        \brief STK non-interpolating tapped delay line class.
        This class implements a non-interpolating digital delay-line with
        an arbitrary number of output "taps".  If the maximum length and
        tap delays are not specified during instantiation, a fixed maximum
        length of 4095 and a single tap delay of zero is set.
        
        A non-interpolating delay line is typically used in fixed
        delay-length applications, such as for reverberation.
        by Perry R. Cook and Gary P. Scavone, 1995--2021.
    */
    /***************************************************/

    class TapDelay : public Filter
    {
    public:

        TapDelay( std::vector<unsigned long> taps = std::vector<unsigned long>( 1, 0 ), unsigned long maxDelay = 4095 );
        ~TapDelay();

        void setMaximumDelay( unsigned long delay );
        void setTapDelays( std::vector<unsigned long> taps );
        std::vector<unsigned long> getTapDelays( void ) const { return delays_; };
        StkFloat lastOut( unsigned int tap = 0 ) const;
        StkFrames& tick( StkFloat input, StkFrames& outputs );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFrames& tick( StkFrames& iFrames, StkFrames &oFrames, unsigned int iChannel = 0 );
    };
}