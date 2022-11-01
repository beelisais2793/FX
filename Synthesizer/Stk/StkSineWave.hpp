// will replace this with a wavetable or function generator
#pragma once

const unsigned long TABLE_SIZE = 2048;

#include "StkGenerator.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class SineWave
    \brief STK sinusoid oscillator class.

    This class computes and saves a static sine "table" that can be
    shared by multiple instances.  It has an interface similar to the
    WaveLoop class but inherits from the Generator class.  Output
    values are computed using linear interpolation.

    The "table" length, set in SineWave.h, is 2048 samples by default.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class SineWave : public Generator<T>
{
public:
  //! Default constructor.
  SineWave( void );

  //! Class destructor.
  ~SineWave( void );

  //! Clear output and reset time pointer to zero.
  void reset( void );

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  void setRate( T rate ) { rate_ = rate; };

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e frequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
   */
  void setFrequency( T frequency );

  //! Increment the read pointer by \e time in samples, modulo the table size.
  void addTime( T time );

  //! Increment the read pointer by a normalized \e phase value.
  /*!
    This function increments the read pointer by a normalized phase
    value, such that \e phase = 1.0 corresponds to a 360 degree phase
    shift.  Positive or negative values are possible.
   */
  void addPhase( T phase );

  //! Add a normalized phase offset to the read pointer.
  /*!
    A \e phaseOffset = 1.0 corresponds to a 360 degree phase
    offset.  Positive or negative values are possible.
   */
  void addPhaseOffset( T phaseOffset );

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

  static StkFrames<T> table_;
  T time_;
  T rate_;
  T phaseOffset_;
  unsigned int iIndex_;
  T alpha_;

};

template<typename T>
inline T SineWave<T>::tick( void )
{
  // Check limits of time address ... if necessary, recalculate modulo
  // TABLE_SIZE.
  while ( time_ < 0.0 )
    time_ += TABLE_SIZE;
  while ( time_ >= TABLE_SIZE )
    time_ -= TABLE_SIZE;

  iIndex_ = (unsigned int) time_;
  alpha_ = time_ - iIndex_;
  T tmp = table_[ iIndex_ ];
  tmp += ( alpha_ * ( table_[ iIndex_ + 1 ] - tmp ) );

  // Increment time, which can be negative.
  time_ += rate_;

  this->lastFrame_[0] = tmp;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& SineWave<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    this->oStream_ << "SineWave::tick(): channel and StkFrames<T> arguments are incompatible!";
    this->handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  T tmp = 0.0;

  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {

    // Check limits of time address ... if necessary, recalculate modulo
    // TABLE_SIZE.
    while ( time_ < 0.0 )
      time_ += TABLE_SIZE;
    while ( time_ >= TABLE_SIZE )
      time_ -= TABLE_SIZE;

    iIndex_ = (unsigned int) time_;
    alpha_ = time_ - iIndex_;
    tmp = table_[ iIndex_ ];
    tmp += ( alpha_ * ( table_[ iIndex_ + 1 ] - tmp ) );
    *samples = tmp;

    // Increment time, which can be negative.
    time_ += rate_;
  }

  this->lastFrame_[0] = tmp;
  return frames;
}

/***************************************************/
/*! \class SineWave
    \brief STK sinusoid oscillator class.

    This class computes and saves a static sine "table" that can be
    shared by multiple instances.  It has an interface similar to the
    WaveLoop class but inherits from the Generator class.  Output
    values are computed using linear interpolation.

    The "table" length, set in SineWave.h, is 2048 samples by default.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/




template<typename T>StkFrames<T> SineWave<T>::table_;

template<typename T>
SineWave<T>::SineWave( void )
  : time_(0.0), rate_(1.0), phaseOffset_(0.0)
{
  if ( table_.empty() ) {
    table_.resize( TABLE_SIZE + 1, 1 );
    T temp = 1.0 / TABLE_SIZE;
    for ( unsigned long i=0; i<=TABLE_SIZE; i++ )
      table_[i] = sin( TWO_PI * i * temp );
  }

  this->addSampleRateAlert( this );
}

template<typename T>
SineWave<T>::~SineWave()
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void SineWave<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this->ignoreSampleRateChange_ )
    this->setRate( oldRate * rate_ / newRate );
}

template<typename T>
void SineWave<T>::reset( void )
{
  time_ = 0.0;
  this->lastFrame_[0] = 0;
}

template<typename T>
void SineWave<T>::setFrequency( T frequency )
{
  // This is a looping frequency.
  this->setRate( TABLE_SIZE * frequency / sampleRate() );
}

template<typename T>
void SineWave<T>::addTime( T time )
{
  // Add an absolute time in samples.
  time_ += time;
}

template<typename T>
void SineWave<T>::addPhase( T phase )
{
  // Add a time in cycles (one cycle = TABLE_SIZE).
  time_ += TABLE_SIZE * phase;
}

template<typename T>
void SineWave<T>::addPhaseOffset( T phaseOffset )
{
  // Add a phase offset relative to any previous offset value.
  time_ += ( phaseOffset - phaseOffset_ ) * TABLE_SIZE;
  phaseOffset_ = phaseOffset;
}

} // stk namespace



