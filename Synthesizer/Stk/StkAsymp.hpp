#pragma once
#include "StkGenerator.hpp"
#include <cmath>

namespace stk {

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


template<typename T>
class Asymp : public Generator<T>
{
 public:

  const T TARGET_THRESHOLD = 0.000001;

  //! Default constructor.
  Asymp( void );

  //! Class destructor.
  ~Asymp( void );

  //! Set target = 1.
  void keyOn( void );

  //! Set target = 0.
  void keyOff( void );

  //! Set the asymptotic rate via the time factor \e tau (must be > 0).
  /*!
    The rate is computed as described above.  The value of \e tau
    must be greater than zero.  Values of \e tau close to zero produce
    fast approach rates, while values greater than 1.0 produce rather
    slow rates.
  */
  void setTau( T tau );

  //! Set the asymptotic rate based on a time duration (must be > 0).
  void setTime( T time );

  //! Set the asymptotic rate such that the target value is perceptually reached (to within -60dB of the target) in \e t60 seconds.
  void setT60( T t60 );

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

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

 protected:

  void sampleRateChanged( T newRate, T oldRate );

  T value_;
  T target_;
  T factor_;
  T constant_;
  int state_;
};

template<typename T>
inline T Asymp<T>:: tick( void )
{
  if ( state_ ) {

    value_ = factor_ * value_ + constant_;

    // Check threshold.
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
    this->lastFrame_[0] = value_;
  }

  return value_;
}

template<typename T>
inline StkFrames<T>& Asymp<T>:: tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Asymp::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = Asymp::tick();

  return frames;
}

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

template<typename T>
Asymp<T>:: Asymp( void )
{
  value_ = 0.0;
  target_ = 0.0;
  state_ = 0;
  factor_ = exp( -1.0 / ( 0.3 * sampleRate() ) );
  constant_ = 0.0;
  this->addSampleRateAlert( this );
}

template<typename T>
Asymp<T>:: ~Asymp( void )
{    
  this->removeSampleRateAlert( this );
}

template<typename T>
void Asymp<T>:: sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this->ignoreSampleRateChange_ ) {
    T tau = -1.0 / ( std::log( factor_ ) * oldRate );
    factor_ = std::exp( -1.0 / ( tau * newRate ) );
  }
}

template<typename T>
void Asymp<T>:: keyOn( void )
{
  this->setTarget( 1.0 );
}

template<typename T>
void Asymp<T>:: keyOff( void )
{
  this->setTarget( 0.0 );
}

template<typename T>
void Asymp<T>:: setTau( T tau )
{
  if ( tau <= 0.0 ) {
    oStream_ << "Asymp::setTau: negative or zero tau not allowed!";
    handleError( StkError::WARNING ); return;
  }

  factor_ = std::exp( -1.0 / ( tau * sampleRate() ) );
  constant_ = ( 1.0 - factor_ ) * target_;
}

template<typename T>
void Asymp<T>:: setTime( T time )
{
  if ( time <= 0.0 ) {
    oStream_ << "Asymp::setTime: negative or zero times not allowed!";
    handleError( StkError::WARNING ); return;
  }

  T tau = -time / std::log( TARGET_THRESHOLD );
  factor_ = std::exp( -1.0 / ( tau * sampleRate() ) );
  constant_ = ( 1.0 - factor_ ) * target_;
}

template<typename T>
void Asymp<T>:: setT60( T t60 )
{
  if ( t60 <= 0.0 ) {
    oStream_ << "Asymp::setT60: negative or zero t60 not allowed!";
    handleError( StkError::WARNING ); return;
  }

  setTau( t60 / 6.91 );
}

template<typename T>
void Asymp<T>:: setTarget( T target )
{
  target_ = target;
  if ( value_ != target_ ) state_ = 1;
  constant_ = ( 1.0 - factor_ ) * target_;
}

template<typename T>
void Asymp<T>:: setValue( T value )
{
  state_ = 0;
  target_ = value;
  value_ = value;
}

} // stk namespace

