#pragma once
#include "Stk.hpp"
#include "StkTwang.hpp"
#include "StkOnePole.hpp"
#include "StkOneZero.hpp"
#include "StkFileWvIn.hpp"
#include "StkNoise.hpp"
#include "SKINImsg.h"
#include <cmath>

#define BASE_COUPLING_GAIN 0.01

namespace stk {

/***************************************************/
/*! \class Guitar
    \brief STK guitar model class.

    This class implements a guitar model with an arbitrary number of
    strings (specified during instantiation).  Each string is
    represented by an stk::Twang object.  The model supports commuted
    synthesis, as discussed by Smith and Karjalainen.  It also includes
    a basic body coupling model and supports feedback.

    This class does not attempt voice management.  Rather, most
    functions support a parameter to specify a particular string
    number and string (voice) management is assumed to occur
    externally.  Note that this class does not inherit from
    stk::Instrmnt because of API inconsistencies.

    This is a digital waveguide model, making its use possibly subject
    to patents held by Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bridge Coupling Gain = 2
       - Pluck Position = 4
       - Loop Gain = 11
       - Coupling Filter Pole = 1
       - Pick Filter Pole = 128

    by Gary P. Scavone, 2012.
*/
/***************************************************/

template<typename T>
class Guitar : public Stk
{
 public:
  //! Class constructor, specifying an arbitrary number of strings (default = 6).
  Guitar( unsigned int nStrings = 6, std::string bodyfile = "" );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the string excitation, using either a soundfile or computed noise.
  /*!
     If no argument is provided, the std::string is empty, or an error
     occurs reading the file data, an enveloped noise signal will be
     generated for use as the pluck excitation.
   */
  void setBodyFile( std::string bodyfile = "" );

  //! Set the pluck position for one or all strings.
  /*!
     If the \c string argument is < 0, the pluck position is set
     for all strings.
  */
  void setPluckPosition( T position, int string = -1 );

  //! Set the loop gain for one or all strings.
  /*!
     If the \c string argument is < 0, the loop gain is set for all
     strings.
  */
  void setLoopGain( T gain, int string = -1 );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( T frequency, unsigned int string = 0 );

  //! Start a note with the given frequency and amplitude.
  /*!
     If the \c amplitude parameter is less than 0.2, the string will
     be undamped but it will not be "plucked."
   */
  void noteOn( T frequency, T amplitude, unsigned int string = 0 );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude, unsigned int string = 0 );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  /*!
     If the \c string argument is < 0, then the control change is
     applied to all strings (if appropriate).
  */
  void controlChange( int number, T value, int string = -1 );

  //! Return the last computed output value.
  T lastOut( void ) { return lastFrame_[0]; };

  //! Take an optional input sample and compute one output sample.
  T tick( T input = 0.0 );

  //! Take a channel of the \c iFrames object as inputs to the class and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take a channel of the \c iFrames object as inputs to the effect and write outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  Each channel
    argument must be less than the number of channels in the
    corresponding StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:

  std::vector< stk::Twang<T> > strings_;
  std::vector< int > stringState_; // 0 = off, 1 = decaying, 2 = on
  std::vector< unsigned int > decayCounter_;
  std::vector< unsigned int > filePointer_;
  std::vector< T > pluckGains_;

  OnePole<T>   pickFilter_;
  OnePole<T>   couplingFilter_;
  T  couplingGain_;
  StkFrames<T> excitation_;
  StkFrames<T> lastFrame_;
};

// NOTE: It is not possible to implement the Smith coupled string model here because the Twang class does
// not currently offer the chance to have access to a traveling-wave component. Thus, the coupling

// implemented here is approximate.
template<typename T>
inline T Guitar<T>::tick( T input )
{
  T temp, output = 0.0;
  lastFrame_[0] = couplingGain_ * couplingFilter_.tick( lastFrame_[0] ) / strings_.size();
  for ( unsigned int i=0; i<strings_.size(); i++ ) {
    if ( stringState_[i] ) {
      temp = input;
      // If pluckGain < 0.2, let string ring but don't pluck it.
      if ( filePointer_[i] < excitation_.frames() && pluckGains_[i] > 0.2 )
        temp += pluckGains_[i] * excitation_[filePointer_[i]++];
      temp += lastFrame_[0]; // bridge coupling
      output += strings_[i].tick( temp );
      // Check if string energy has decayed sufficiently to turn it off.
      if ( stringState_[i] == 1 ) {
        if ( fabs( strings_[i].lastOut() ) < 0.001 ) decayCounter_[i]++;
        else decayCounter_[i] = 0;
        if ( decayCounter_[i] > (unsigned int) floor( 0.1 * stk::sampleRate() ) ) {
          stringState_[i] = 0;
          decayCounter_[i] = 0;
        }
      }
    }
  }

  return lastFrame_[0] = output;
}

template<typename T>
inline StkFrames<T>& Guitar<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "Guitar::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick( *samples );

  return frames;
}

template<typename T>
inline StkFrames<T>& Guitar<T>::tick( StkFrames<T>& iFrames, StkFrames<T>& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "Guitar::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop )
    *oSamples = tick( *iSamples );

  return iFrames;
}

/***************************************************/
/*! \class Guitar
    \brief STK guitar model class.

    This class implements a guitar model with an arbitrary number of
    strings (specified during instantiation).  Each string is
    represented by an stk::Twang object.  The model supports commuted
    synthesis, as discussed by Smith and Karjalainen.  It also includes
    a basic body coupling model and supports feedback.

    This class does not attempt voice management.  Rather, most
    functions support a parameter to specify a particular string
    number and string (voice) management is assumed to occur
    externally.  Note that this class does not inherit from
    stk::Instrmnt because of API inconsistencies.

    This is a digital waveguide model, making its use possibly subject
    to patents held by Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bridge Coupling Gain = 2
       - Pluck Position = 4
       - Loop Gain = 11
       - Coupling Filter Pole = 1
       - Pick Filter Pole = 128

    by Gary P. Scavone, 2012.
*/
/***************************************************/

template<typename T>
Guitar<T>::Guitar( unsigned int nStrings, std::string bodyfile )
{
  strings_.resize( nStrings );
  stringState_.resize( nStrings, 0 );
  decayCounter_.resize( nStrings, 0 );
  filePointer_.resize( nStrings, 0 );
  pluckGains_.resize( nStrings, 0 );

  setBodyFile( bodyfile );

  couplingGain_ = BASE_COUPLING_GAIN;
  couplingFilter_.setPole( 0.9 );
  pickFilter_.setPole( 0.95 );
  lastFrame_.resize(1, 1, 0.0);
}

template<typename T>
void Guitar<T>::clear( void )
{
  for ( unsigned int i=0; i<strings_.size(); i++ ) {
    strings_[i].clear();
    stringState_[i] = 0;
    filePointer_[i] = 0;
  }
}

template<typename T>
void Guitar<T>::setBodyFile( std::string bodyfile )
{
  bool fileLoaded = false;
  if ( bodyfile != "" ) {
    try {
      FileWvIn<T> file( bodyfile );
  
      // Fill the StkFrames<T> variable with the (possibly interpolated)
      // file data.
      excitation_.resize( (unsigned long) ( 0.5 + ( file.getSize() * stk::sampleRate() / file.getFileRate() ) ) );
      file.tick( excitation_ );
      fileLoaded = true;
    }
    catch ( StkError &error ) {
      oStream_ << "Guitar::setBodyFile: file error (" << error.getMessage() << ") ... using noise excitation.";
      handleError( StkError::WARNING );
    }
  }

  if ( !fileLoaded ) {
    unsigned int M = 200;  // arbitrary value
    excitation_.resize( M );
    Noise<T> noise;
    noise.tick( excitation_ );
    // Smooth the start and end of the noise.
    unsigned int N = (unsigned int) M * 0.2; // arbitrary value
    for ( unsigned int n=0; n<N; n++ ) {
      T weight = 0.5 * ( 1.0 - cos( n * PI / (N-1) ) );
      excitation_[n] *= weight;
      excitation_[M-n-1] *= weight;
    }
  }

  // Filter the excitation to simulate pick hardness
  pickFilter_.tick( excitation_ );

  // Compute file mean and remove (to avoid DC bias).
  T mean = 0.0;
  for ( unsigned int i=0; i<excitation_.frames(); i++ )
    mean += excitation_[i];
  mean /= excitation_.frames();

  for ( unsigned int i=0; i<excitation_.frames(); i++ )
    excitation_[i] -= mean;

  // Reset all the file pointers.
  for ( unsigned int i=0; i<strings_.size(); i++ )
    filePointer_[i] = 0;
}


template<typename T>
void Guitar<T>::setPluckPosition( T position, int string )
{
  if ( position < 0.0 || position > 1.0 ) {
    oStream_ << "Guitar::setPluckPosition: position parameter out of range!";
    handleError( StkError::WARNING ); return;
  }

  if ( string >= (int) strings_.size() ) {
    oStream_ << "Guitar::setPluckPosition: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }

  if ( string < 0 ) // set all strings
    for ( unsigned int i=0; i<strings_.size(); i++ )
      strings_[i].setPluckPosition( position );
  else
    strings_[string].setPluckPosition( position );
}

template<typename T>
void Guitar<T>::setLoopGain( T gain, int string )
{
  if ( gain < 0.0 || gain > 1.0 ) {
    oStream_ << "Guitar::setLoopGain: gain parameter out of range!";
    handleError( StkError::WARNING ); return;
  }

  if ( string >= (int) strings_.size() ) {
    oStream_ << "Guitar::setLoopGain: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }

  if ( string < 0 ) // set all strings
    for ( unsigned int i=0; i<strings_.size(); i++ )
      strings_[i].setLoopGain( gain );
  else
    strings_[string].setLoopGain( gain );
}

template<typename T>
void Guitar<T>::setFrequency( T frequency, unsigned int string )
{
#if defined(_STK_DEBUG_)
  if ( frequency <= 0.0 ) {
    oStream_ << "Guitar::setFrequency: frequency parameter is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  if ( string >= strings_.size() ) {
    oStream_ << "Guitar::setFrequency: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }
#endif

  strings_[string].setFrequency( frequency );
}

template<typename T>
void Guitar<T>::noteOn( T frequency, T amplitude, unsigned int string )
{
#if defined(_STK_DEBUG_)
  if ( string >= strings_.size() ) {
    oStream_ << "Guitar::noteOn: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }

  if ( stk::inRange( amplitude, 0.0, 1.0 ) == false ) {
    oStream_ << "Guitar::noteOn: amplitude parameter is outside range 0.0 - 1.0!";
    handleError( StkError::WARNING ); return;
  }
#endif

  this->setFrequency( frequency, string );
  stringState_[string] = 2;
  filePointer_[string] = 0;
  strings_[string].setLoopGain( 0.995 );
  pluckGains_[string] = amplitude;
}

template<typename T>
void Guitar<T>::noteOff( T amplitude, unsigned int string )
{
#if defined(_STK_DEBUG_)
  if ( string >= strings_.size() ) {
    oStream_ << "Guitar::noteOff: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }

  if ( stk::inRange( amplitude, 0.0, 1.0 ) == false ) {
    oStream_ << "Guitar::noteOff: amplitude parameter is outside range 0.0 - 1.0!";
    handleError( StkError::WARNING ); return;
  }
#endif

  strings_[string].setLoopGain( (1.0 - amplitude) * 0.9 );
  stringState_[string] = 1;
}

template<typename T>
void Guitar<T>::controlChange( int number, T value, int string )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Guitar::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }

  if ( string > 0 && string >= (int) strings_.size() ) {
    oStream_ << "Guitar::controlChange: string parameter is greater than number of strings!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == 2 )
    couplingGain_ = 1.5 * BASE_COUPLING_GAIN * normalizedValue;
  else if ( number == __SK_PickPosition_ ) // 4
    this->setPluckPosition( normalizedValue, string );
  else if ( number == __SK_StringDamping_ ) // 11
    this->setLoopGain( 0.97 + (normalizedValue * 0.03), string );
  else if ( number == __SK_ModWheel_ ) // 1
    couplingFilter_.setPole( 0.98 * normalizedValue );
  else if (number == __SK_AfterTouch_Cont_) // 128
    pickFilter_.setPole( 0.95 * normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Guitar::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

