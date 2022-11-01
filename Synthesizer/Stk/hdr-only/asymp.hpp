#pragma once 

#include "Generator.h"
#include <cmath>

namespace stk {


    const StkFloat TARGET_THRESHOLD = 0.000001;

/***************************************************/
/*! \class Asymp
    \brief STK asymptotic curve envelope class

    This class implements a simple envelope generator
    which asymptotically approaches a target value.
    The algorithm used is of the form:

    y[n] = a y[n-1] + (1-a) target,

    where a = exp(-T/tau), T is the sample period, and
    tau is a time constant.  The user can set the time
    constant (default value = 0.3) and target value.
    Theoretically, this recursion never reaches its
    target, though the calculations in this class are
    stopped when the current value gets within a small
    threshold value of the target (at which time the
    current value is set to the target).  It responds
    to \e keyOn and \e keyOff messages by ramping to
    1.0 on keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

    class Asymp : public Generator
    {
    public:

        Asymp( void );
        ~Asymp( void );

        void keyOn( void );
        void keyOff( void );

        void setTau( StkFloat tau );
        void setTime( StkFloat time );
        void setT60( StkFloat t60 );
        void setTarget( StkFloat target );
        void setValue( StkFloat value );
        int getState( void ) const { return state_; };
        StkFloat lastOut( void ) const { return lastFrame_[0]; };
        StkFloat tick( void );
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

    protected:

        void sampleRateChanged( StkFloat newRate, StkFloat oldRate );

        StkFloat value_;
        StkFloat target_;
        StkFloat factor_;
        StkFloat constant_;
        int state_;
    };

    inline StkFloat Asymp :: tick( void )
    {
        if ( state_ ) 
        {
            value_ = factor_ * value_ + constant_;
        
            if ( target_ > value_ ) {
                if ( target_ - value_ <= TARGET_THRESHOLD ) {
                    value_ = target_;
                    state_ = 0;
                }
            }
            else {
                if ( value_ - target_ <= TARGET_THRESHOLD ) {
                    value_ = target_;
                    state_ = 0;
                }
            }        
            lastFrame_[0] = value_;
        }
        return value_;
    }

    inline StkFrames& Asymp :: tick( StkFrames& frames, unsigned int channel )
    {
        #if defined(_STK_DEBUG_)
        if ( channel >= frames.channels() ) {
            oStream_ << "Asymp::tick(): channel and StkFrames arguments are incompatible!";
            handleError( StkError::FUNCTION_ARGUMENT );
        }
        #endif

        StkFloat *samples = &frames[channel];
        unsigned int hop = frames.channels();
        for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
            *samples = Asymp::tick();

        return frames;
    }

    inline Asymp :: Asymp( void )
    {
        value_ = 0.0;
        target_ = 0.0;
        state_ = 0;
        factor_ = exp( -1.0 / ( 0.3 * Stk::sampleRate() ) );
        constant_ = 0.0;
        Stk::addSampleRateAlert( this );
    }

    inline Asymp :: ~Asymp( void )
    {    
        Stk::removeSampleRateAlert( this );
    }

    inline void Asymp :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
    {
        if ( !ignoreSampleRateChange_ ) {
            StkFloat tau = -1.0 / ( std::log( factor_ ) * oldRate );
            factor_ = std::exp( -1.0 / ( tau * newRate ) );
        }
    }

    inline void Asymp :: keyOn( void )
    {
        this->setTarget( 1.0 );
    }

    inline void Asymp :: keyOff( void )
    {
        this->setTarget( 0.0 );
    }

    inline void Asymp :: setTau( StkFloat tau )
    {
        if ( tau <= 0.0 ) {
            oStream_ << "Asymp::setTau: negative or zero tau not allowed!";
            handleError( StkError::WARNING ); return;
        }

        factor_ = std::exp( -1.0 / ( tau * Stk::sampleRate() ) );
        constant_ = ( 1.0 - factor_ ) * target_;
    }

    inline void Asymp :: setTime( StkFloat time )
    {
        if ( time <= 0.0 ) {
            oStream_ << "Asymp::setTime: negative or zero times not allowed!";
            handleError( StkError::WARNING ); return;
        }

        StkFloat tau = -time / std::log( TARGET_THRESHOLD );
        factor_ = std::exp( -1.0 / ( tau * Stk::sampleRate() ) );
        constant_ = ( 1.0 - factor_ ) * target_;
    }

    inline void Asymp :: setT60( StkFloat t60 )
    {
        if ( t60 <= 0.0 ) {
            oStream_ << "Asymp::setT60: negative or zero t60 not allowed!";
            handleError( StkError::WARNING ); return;
        }

        setTau( t60 / 6.91 );
    }

    inline void Asymp :: setTarget( StkFloat target )
    {
        target_ = target;
        if ( value_ != target_ ) state_ = 1;
        constant_ = ( 1.0 - factor_ ) * target_;
    }

    inline void Asymp :: setValue( StkFloat value )
    {
        state_ = 0;
        target_ = value;
        value_ = value;
    }

} // stk namespace
