#pragma once
#include "StkInstrmnt.hpp"
#include "StkTwang.hpp"
#include "StkFileWvIn.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class uses two "twang" models and "commuted
    synthesis" techniques to model a mandolin
    instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.  Commuted
    Synthesis, in particular, is covered by patents,
    granted, pending, and/or applied-for.  All are
    assigned to the Board of Trustees, Stanford
    University.  For information, contact the Office
    of Technology Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Mandolin : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Mandolin( T lowestFrequency );

  //! Class destructor.
  ~Mandolin( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Detune the two strings by the given factor.  A value of 1.0 produces unison strings.
  void setDetune( T detune );

  //! Set the body size (a value of 1.0 produces the "default" size).
  void setBodySize( T size );

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPluckPosition( T position );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency );

  //! Pluck the strings with the given amplitude (0.0 - 1.0) using the current frequency.
  void pluck( T amplitude );

  //! Pluck the strings with the given amplitude (0.0 - 1.0) and position (0.0 - 1.0).
  void pluck( T amplitude,T position );

  //! Start a note with the given frequency and amplitude (0.0 - 1.0).
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, T value );

  //! Compute and return one output sample.
  T tick( unsigned int channel = 0 );

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

  Twang<T> strings_[2];
  FileWvIn<T> soundfile_[12];

  int mic_;
  T detuning_;
  T frequency_;
  T pluckAmplitude_;
};

template<typename T>
inline T Mandolin<T>::tick( unsigned int )
{
  T temp = 0.0;
  if ( !soundfile_[mic_].isFinished() )
    temp = soundfile_[mic_].tick() * pluckAmplitude_;

  this->lastFrame_[0] = strings_[0].tick( temp );
  this->lastFrame_[0] += strings_[1].tick( temp );
  this->lastFrame_[0] *= 0.2;

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Mandolin<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Mandolin::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  if ( nChannels == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
      *samples++ = tick();
  }
  else {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
      *samples++ = tick();
      for ( j=1; j<nChannels; j++ )
        *samples++ = this->lastFrame_[j];
    }
  }

  return frames;
}

/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class uses two "twang" models and "commuted
    synthesis" techniques to model a mandolin
    instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.  Commuted
    Synthesis, in particular, is covered by patents,
    granted, pending, and/or applied-for.  All are
    assigned to the Board of Trustees, Stanford
    University.  For information, contact the Office
    of Technology Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Mandolin<T>::Mandolin( T lowestFrequency )
{
  if ( lowestFrequency <= 0.0 ) {
    oStream_ << "Mandolin::Mandolin: argument is less than or equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // Concatenate the STK rawwave path to the rawwave files
  soundfile_[0].openFile( (stk::rawwavePath() + "mand1.raw").c_str(), true );
  soundfile_[1].openFile( (stk::rawwavePath() + "mand2.raw").c_str(), true );
  soundfile_[2].openFile( (stk::rawwavePath() + "mand3.raw").c_str(), true );
  soundfile_[3].openFile( (stk::rawwavePath() + "mand4.raw").c_str(), true );
  soundfile_[4].openFile( (stk::rawwavePath() + "mand5.raw").c_str(), true );
  soundfile_[5].openFile( (stk::rawwavePath() + "mand6.raw").c_str(), true );
  soundfile_[6].openFile( (stk::rawwavePath() + "mand7.raw").c_str(), true );
  soundfile_[7].openFile( (stk::rawwavePath() + "mand8.raw").c_str(), true );
  soundfile_[8].openFile( (stk::rawwavePath() + "mand9.raw").c_str(), true );
  soundfile_[9].openFile( (stk::rawwavePath() + "mand10.raw").c_str(), true );
  soundfile_[10].openFile( (stk::rawwavePath() + "mand11.raw").c_str(), true );
  soundfile_[11].openFile( (stk::rawwavePath() + "mand12.raw").c_str(), true );

  mic_ = 0;
  detuning_ = 0.995;
  pluckAmplitude_ = 0.5;

  strings_[0].setLowestFrequency( lowestFrequency );
  strings_[1].setLowestFrequency( lowestFrequency );
  this->setFrequency( 220.0 );
  this->setPluckPosition( 0.4 );
}

template<typename T>
Mandolin<T>::~Mandolin( void )
{
}

template<typename T>
void Mandolin<T>::clear( void )
{
  strings_[0].clear();
  strings_[1].clear();
}

template<typename T>
void Mandolin<T>::setPluckPosition( T position )
{
  if ( position < 0.0 || position > 1.0 ) {
    oStream_ << "Mandolin::setPluckPosition: position parameter out of range!";
    handleError( StkError::WARNING ); return;
  }

  strings_[0].setPluckPosition( position );
  strings_[1].setPluckPosition( position );
}

template<typename T>
void Mandolin<T>::setDetune( T detune )
{
  if ( detune <= 0.0 ) {
    oStream_ << "Mandolin::setDeturn: parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  detuning_ = detune;
  strings_[1].setFrequency( frequency_ * detuning_ );
}

template<typename T>
void Mandolin<T>::setBodySize( T size )
{
  // Scale the commuted body response by its sample rate (22050).
  T rate = size * 22050.0 / stk::sampleRate();
  for ( int i=0; i<12; i++ )
    soundfile_[i].setRate( rate );
}

template<typename T>
void Mandolin<T>::setFrequency( T frequency )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Mandolin::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif

  frequency_ = frequency;
  strings_[0].setFrequency( frequency_ );
  strings_[1].setFrequency( frequency_ * detuning_ );
}

template<typename T>
void Mandolin<T>::pluck( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Mandolin::pluck: amplitude parameter out of range!";
    handleError( StkError::WARNING ); return;
  }

  soundfile_[mic_].reset();
  pluckAmplitude_ = amplitude;

  //strings_[0].setLoopGain( 0.97 + pluckAmplitude_ * 0.03 );
  //strings_[1].setLoopGain( 0.97 + pluckAmplitude_ * 0.03 );
}

template<typename T>
void Mandolin<T>::pluck( T amplitude, T position )
{
  this->setPluckPosition( position );
  this->pluck( amplitude );
}

template<typename T>
void Mandolin<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );
}

template<typename T>
void Mandolin<T>::noteOff( T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Mandolin::noteOff: amplitude is out of range!";
    handleError( StkError::WARNING ); return;
  }

  //strings_[0].setLoopGain( 0.97 + (1 - amplitude) * 0.03 );
  //strings_[1].setLoopGain( 0.97 + (1 - amplitude) * 0.03 );
}

template<typename T>
void Mandolin<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Mandolin::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == __SK_BodySize_ ) // 2
    this->setBodySize( normalizedValue * 2.0 );
  else if ( number == __SK_PickPosition_ ) // 4
    this->setPluckPosition( normalizedValue );
  else if ( number == __SK_StringDamping_ ) { // 11
    strings_[0].setLoopGain( 0.97 + (normalizedValue * 0.03) );
    strings_[1].setLoopGain( 0.97 + (normalizedValue * 0.03) );
  }
  else if ( number == __SK_StringDetune_ ) // 1
    this->setDetune( 1.0 - (normalizedValue * 0.1) );
  else if ( number == __SK_AfterTouch_Cont_ ) // 128
    mic_ = (int) (normalizedValue * 11.0);
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Mandolin::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

