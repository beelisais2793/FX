#pragma once

#include "Generator.h"
#include <cmath>

/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This class implements a traditional ADSR (Attack, Decay, Sustain,
    Release) envelope.  It responds to simple keyOn and keyOff
    messages, keeping track of its state.  The \e state = ADSR::IDLE
    before being triggered and after the envelope value reaches 0.0 in
    the ADSR::RELEASE state.  All rate, target and level settings must
    be non-negative.  All time settings are in seconds and must be
    positive.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

namespace stk 
{

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

        //! Default constructor.
        ADSR( void );

        //! Class destructor.
        ~ADSR( void );

        //! Set target = 1, state = \e ADSR::ATTACK.
        void keyOn( void );

        //! Set target = 0, state = \e ADSR::RELEASE.
        void keyOff( void );

        //! Set the attack rate (gain / sample).
        void setAttackRate( StkFloat rate );

        //! Set the target value for the attack (default = 1.0).
        void setAttackTarget( StkFloat target );

        //! Set the decay rate (gain / sample).
        void setDecayRate( StkFloat rate );

        //! Set the sustain level.
        void setSustainLevel( StkFloat level );

        //! Set the release rate (gain / sample).
        void setReleaseRate( StkFloat rate );

        //! Set the attack rate based on a time duration (seconds).
        void setAttackTime( StkFloat time );

        //! Set the decay rate based on a time duration (seconds).
        void setDecayTime( StkFloat time );

        //! Set the release rate based on a time duration (seconds).
        void setReleaseTime( StkFloat time );

        //! Set sustain level and attack, decay, and release time durations (seconds).
        void setAllTimes( StkFloat aTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime );

        //! Set a sustain target value and attack or decay from current value to target.
        void setTarget( StkFloat target );

        //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, IDLE).
        int getState( void ) const { return state_; };

        //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
        void setValue( StkFloat value );

        //! Return the last computed output value.
        StkFloat lastOut( void ) const { return lastFrame_[0]; };

        //! Compute and return one output sample.
        StkFloat tick( void );

        //! Fill a channel of the StkFrames object with computed outputs.
        /*!
            The \c channel argument must be less than the number of
            channels in the StkFrames argument (the first channel is specified
            by 0).  However, range checking is only performed if _STK_DEBUG_
            is defined during compilation, in which case an out-of-range value
            will trigger an StkError exception.
        */
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );

    protected:  

        void sampleRateChanged( StkFloat newRate, StkFloat oldRate );

        int state_;
        StkFloat value_;
        StkFloat target_;
        StkFloat attackRate_;
        StkFloat decayRate_;
        StkFloat releaseRate_;
        StkFloat releaseTime_;
        StkFloat sustainLevel_;
    };

    inline StkFloat ADSR :: tick( void )
    {
        switch ( state_ ) {

        case ATTACK:
            value_ += attackRate_;
            if ( value_ >= target_ ) {
            value_ = target_;
            target_ = sustainLevel_;
                state_ = DECAY;
            }
            lastFrame_[0] = value_;
            break;

        case DECAY:
            if ( value_ > sustainLevel_ ) {
            value_ -= decayRate_;
            if ( value_ <= sustainLevel_ ) {
                value_ = sustainLevel_;
                state_ = SUSTAIN;
            }
            }
            else {
            value_ += decayRate_; // attack target < sustain level
            if ( value_ >= sustainLevel_ ) {
                value_ = sustainLevel_;
                state_ = SUSTAIN;
            }
            }
            lastFrame_[0] = value_;
            break;

        case RELEASE:
            value_ -= releaseRate_;
            if ( value_ <= 0.0 ) {
            value_ = 0.0;
            state_ = IDLE;
            }
            lastFrame_[0] = value_;

        }
        return value_;
    }

    inline StkFrames& ADSR :: tick( StkFrames& frames, unsigned int channel )
    {
        #if defined(_STK_DEBUG_)
        if ( channel >= frames.channels() ) {
            oStream_ << "ADSR::tick(): channel and StkFrames arguments are incompatible!";
            handleError( StkError::FUNCTION_ARGUMENT );
        }
        #endif

        StkFloat *samples = &frames[channel];
        unsigned int hop = frames.channels();
        for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
            *samples = ADSR::tick();

        return frames;
    }


    inline ADSR :: ADSR( void )
    {
        target_ = 0.0;
        value_ = 0.0;
        attackRate_ = 0.001;
        decayRate_ = 0.001;
        releaseRate_ = 0.005;
        releaseTime_ = -1.0;
        sustainLevel_ = 0.5;
        state_ = IDLE;
        Stk::addSampleRateAlert( this );
    }

    inline  ADSR :: ~ADSR( void )
    {
        Stk::removeSampleRateAlert( this );
    }

    inline void ADSR :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
    {
        if ( !ignoreSampleRateChange_ ) {
            attackRate_ = oldRate * attackRate_ / newRate;
            decayRate_ = oldRate * decayRate_ / newRate;
            releaseRate_ = oldRate * releaseRate_ / newRate;
        }
    }

    inline void ADSR :: keyOn()
    {
        if ( target_ <= 0.0 ) target_ = 1.0;
        state_ = ATTACK;
    }

    inline void ADSR :: keyOff()
    {
        target_ = 0.0;
        state_ = RELEASE;

        // FIXED October 2010 - Nick Donaldson
        // Need to make release rate relative to current value!!
        // Only update if we have set a TIME rather than a RATE,
        // in which case releaseTime_ will be -1
        if ( releaseTime_ > 0.0 )
            releaseRate_ = value_ / ( releaseTime_ * Stk::sampleRate() );
    }

    inline void ADSR :: setAttackRate( StkFloat rate )
    {
        if ( rate < 0.0 ) {
            oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
            handleError( StkError::WARNING ); return;
        }
        attackRate_ = rate;
    }

    inline void ADSR :: setAttackTarget( StkFloat target )
    {
        if ( target < 0.0 ) {
            oStream_ << "ADSR::setAttackTarget: negative target not allowed!";
            handleError( StkError::WARNING ); return;
        }
        target_ = target;
    }

    inline void ADSR :: setDecayRate( StkFloat rate )
    {
        if ( rate < 0.0 ) {
            oStream_ << "ADSR::setDecayRate: negative rates not allowed!";
            handleError( StkError::WARNING ); return;
        }
        decayRate_ = rate;
    }

    inline void ADSR :: setSustainLevel( StkFloat level )
    {
        if ( level < 0.0 ) {
            oStream_ << "ADSR::setSustainLevel: negative level not allowed!";
            handleError( StkError::WARNING ); return;
        }

        sustainLevel_ = level;
    }

    inline void ADSR :: setReleaseRate( StkFloat rate )
    {
        if ( rate < 0.0 ) {
            oStream_ << "ADSR::setReleaseRate: negative rates not allowed!";
            handleError( StkError::WARNING ); return;
        }

        releaseRate_ = rate;

        // Set to negative value so we don't update the release rate on keyOff()
        releaseTime_ = -1.0;
    }

    inline void ADSR :: setAttackTime( StkFloat time )
    {
        if ( time <= 0.0 ) {
            oStream_ << "ADSR::setAttackTime: negative or zero times not allowed!";
            handleError( StkError::WARNING ); return;
        }

        attackRate_ = 1.0 / ( time * Stk::sampleRate() );
    }

    inline void ADSR :: setDecayTime( StkFloat time )
    {
        if ( time <= 0.0 ) {
            oStream_ << "ADSR::setDecayTime: negative or zero times not allowed!";
            handleError( StkError::WARNING ); return;
        }

        decayRate_ = (1.0 - sustainLevel_) / ( time * Stk::sampleRate() );
    }

    inline void ADSR :: setReleaseTime( StkFloat time )
    {
        if ( time <= 0.0 ) {
            oStream_ << "ADSR::setReleaseTime: negative or zero times not allowed!";
            handleError( StkError::WARNING ); return;
        }

        releaseRate_ = sustainLevel_ / ( time * Stk::sampleRate() );
        releaseTime_ = time;
    }

    inline void ADSR :: setAllTimes( StkFloat aTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime )
    {
        this->setAttackTime( aTime );
        this->setSustainLevel( sLevel );
        this->setDecayTime( dTime );
        this->setReleaseTime( rTime );
    }

    inline void ADSR :: setTarget( StkFloat target )
    {
        if ( target < 0.0 ) {
            oStream_ << "ADSR::setTarget: negative target not allowed!";
            handleError( StkError::WARNING ); return;
        }

        target_ = target;

        this->setSustainLevel( target_ );
        if ( value_ < target_ ) state_ = ATTACK;
        if ( value_ > target_ ) state_ = DECAY;
    }

    inline void ADSR :: setValue( StkFloat value )
    {
        state_ = SUSTAIN;
        target_ = value;
        value_ = value;
        this->setSustainLevel( value );
        lastFrame_[0] = value;
    }

} // stk namespace

#endif
