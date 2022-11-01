#pragma once
#include "StkGenerator.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Envelope
    \brief STK linear line envelope class.

    This class implements a simple linear line envelope generator
    which is capable of ramping to an arbitrary target value by a
    specified \e rate.  It also responds to simple \e keyOn and \e
    keyOff messages, ramping to a specified target (default = 1.0) on
    keyOn and to a specified target (default = 0.0) on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Envelope : public Generator<T>
{
 public:

  //! Default constructor.
  Envelope( void );

  //! Class destructor.
  ~Envelope( void );

  //! Assignment operator.
  Envelope& operator= ( const Envelope& e );

  //! Start ramping to specified target (default = 1).
  void keyOn( T target = 1.0 ) { this->setTarget( target ); };

  //! Start ramping to specified target (default = 0).
  void keyOff(  T target = 0.0 ) { this->setTarget( target ); };

  //! Set the \e rate.
  /*!
    The \e rate must be positive (though a value of 0.0 is allowed).
   */
  void setRate( T rate );

  //! Set the \e rate based on a positive time duration (seconds).
  /*!
    The \e rate is calculated such that the envelope will ramp from
    the current value to the current target in the specified time duration.
   */
  void setTime( T time );

  //! Set the target value.
  void setTarget( T target );

  //! Set current and target values to \e value.
  void setValue( T value );

  //! Return the current envelope \e state (0 = at target, 1 otherwise).
  int getState( void ) const { return state_; };

  //! Return the last computed output value.
  T lastOut( void ) const { return this->lastFrame_[0]; };

  //! Compute and return one output sample.
  T tick( void );

  //! Fill a channel of the StkFrames<T> object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

 protected:

  void sampleRateChanged( T newRate, T oldRate );

  T value_;
  T target_;
  T rate_;
  int state_;
};

template<typename T>
inline T Envelope<T>::tick( void )
{
  if ( state_ ) {
    if ( target_ > value_ ) {
      value_ += rate_;
      if ( value_ >= target_ ) {
        value_ = target_;
        state_ = 0;
      }
    }
    else {
      value_ -= rate_;
      if ( value_ <= target_ ) {
        value_ = target_;
        state_ = 0;
      }
    }
    this->lastFrame_[0] = value_;
  }

  return value_;
}

template<typename T>
inline StkFrames<T>& Envelope<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Envelope::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick();

  return frames;
}

/***************************************************/
/*! \class Envelope
    \brief STK linear line envelope class.

    This class implements a simple linear line envelope generator
    which is capable of ramping to an arbitrary target value by a
    specified \e rate.  It also responds to simple \e keyOn and \e
    keyOff messages, ramping to a specified target (default = 1.0) on
    keyOn and to a specified target (default = 0.0) on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
Envelope<T>::Envelope( void ) : Generator<T>()
{    
  target_ = 0.0;
  value_ = 0.0;
  rate_ = 0.001;
  state_ = 0;
  this->addSampleRateAlert( this );
}

template<typename T>
Envelope<T>::~Envelope( void )
{
  this->removeSampleRateAlert( this );
}

template<typename T>
Envelope<T>& Envelope<T>::operator= ( const Envelope<T>& e )
{
  if ( this != &e ) {
    target_ = e.target_;
    value_ = e.value_;
    rate_ = e.rate_;
    state_ = e.state_;
  }

  return *this;
}

template<typename T>
void Envelope<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( !this->ignoreSampleRateChange_ )
    rate_ = oldRate * rate_ / newRate;
}

template<typename T>
void Envelope<T>::setRate( T rate )
{
  if ( rate < 0.0 ) {
    oStream_ << "Envelope::setRate: argument must be >= 0.0!";
    handleError( StkError::WARNING ); return;
  }

  rate_ = rate;
}

template<typename T>
void Envelope<T>::setTime( T time )
{
  if ( time <= 0.0 ) {
    oStream_ << "Envelope::setTime: argument must be > 0.0!";
    handleError( StkError::WARNING ); return;
  }

  //rate_ = 1.0 / ( time * Stk::sampleRate() );
  rate_ = fabs(target_ - value_) / ( time * sampleRate() );
}

template<typename T>
void Envelope<T>::setTarget( T target )
{
  target_ = target;
  if ( value_ != target_ ) state_ = 1;
}

template<typename T>
void Envelope<T>::setValue( T value )
{
  state_ = 0;
  target_ = value;
  value_ = value;
  this->lastFrame_[0] = value_;
}

} // stk namespace

