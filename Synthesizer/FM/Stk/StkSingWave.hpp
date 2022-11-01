#pragma once
#include "StkFileLoop.hpp"
#include "StkModulate.hpp"
#include "StkEnvelope.hpp"

namespace stk {

/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class loops a specified soundfile and modulates it both
    periodically and randomly to produce a pitched musical sound, like
    a simple voice or violin.  In general, it is not be used alone
    because of "munchkinification" effects from pitch shifting.
    Within STK, it is used as an excitation source for other
    instruments.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class SingWave : public Generator<T>
{
 public:
  //! Class constructor taking filename argument.
  /*!
    An StkError will be thrown if the file is not found, its format
    is unknown, or a read error occurs.  If the soundfile has no
    header, the second argument should be \e true and the file data
    will be assumed to consist of 16-bit signed integers in big-endian
    byte order at a sample rate of 22050 Hz.
  */
  SingWave( std::string fileName, bool raw = false );

  //! Class destructor.
  ~SingWave( void );

  //! Reset file to beginning.
  void reset( void ) { wave_.reset(); this->lastFrame_[0] = 0.0; };

  //! Normalize the file to a maximum of +-1.0.
  void normalize( void ) { wave_.normalize(); };

  //! Normalize the file to a maximum of \e +- peak.
  void normalize( T peak ) { wave_.normalize( peak ); };

  //! Set looping parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Set the vibrato frequency in Hz.
  void setVibratoRate( T rate ) { modulator_.setVibratoRate( rate ); };

  //! Set the vibrato gain.
  void setVibratoGain( T gain ) { modulator_.setVibratoGain( gain ); };

  //! Set the random-ness amount.
  void setRandomGain( T gain ) { modulator_.setRandomGain( gain ); };

  //! Set the sweep rate.
  void setSweepRate( T rate ) { sweepRate_ = rate; };

  //! Set the gain rate.
  void setGainRate( T rate ) { envelope_.setRate( rate ); };

  //! Set the gain target value.
  void setGainTarget( T target ) { envelope_.setTarget( target ); };

  //! Start a note.
  void noteOn( void ) { envelope_.keyOn(); };

  //! Stop a note.
  void noteOff( void ) { envelope_.keyOff(); };

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

  FileLoop<T> wave_;
  Modulate<T> modulator_;
  Envelope<T> envelope_;
  Envelope<T> pitchEnvelope_;
  T rate_;
  T sweepRate_;

};

template<typename T>
inline T SingWave<T>::tick( void )
{
  // Set the wave rate.
  T newRate = pitchEnvelope_.tick();
  newRate += newRate * modulator_.tick();
  wave_.setRate( newRate );

  this->lastFrame_[0] = wave_.tick();
  this->lastFrame_[0] *= envelope_.tick();

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& SingWave<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "SingWave::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = SingWave::tick();

  return frames;
}

/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class loops a specified soundfile and modulates it both
    periodically and randomly to produce a pitched musical sound, like
    a simple voice or violin.  In general, it is not be used alone
    because of "munchkinification" effects from pitch shifting.
    Within STK, it is used as an excitation source for other
    instruments.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/
 
template<typename T>
SingWave<T>::SingWave( std::string fileName, bool raw )
{
  // An exception could be thrown here.
  wave_.openFile( fileName, raw );

	rate_ = 1.0;
	sweepRate_ = 0.001;

	modulator_.setVibratoRate( 6.0 );
	modulator_.setVibratoGain( 0.04 );
	modulator_.setRandomGain( 0.005 );

	this->setFrequency( 75.0 );
	pitchEnvelope_.setRate( 1.0 );
	this->tick();
	this->tick();
	pitchEnvelope_.setRate( sweepRate_ * rate_ );
}

template<typename T>
SingWave<T>::~SingWave()
{
}

template<typename T>
void SingWave<T>::setFrequency( T frequency )
{
	T temp = rate_;
	rate_ = wave_.getSize() * frequency / stk::sampleRate();
	temp -= rate_;
	if ( temp < 0) temp = -temp;
	pitchEnvelope_.setTarget( rate_ );
	pitchEnvelope_.setRate( sweepRate_ * temp );
}

} // stk namespace
