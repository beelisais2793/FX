#pragma once
#include "StkInstrmnt.hpp"
#include <vector>
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of STK instrument
    classes.  Individual voices can be controlled via unique note
    tags.  Instrument groups can be controlled by group number.

    A previously constructed STK instrument class is linked with a
    voice manager using the addInstrument() function.  An optional
    group number argument can be specified to the addInstrument()
    function as well (default group = 0).  The voice manager does not
    delete any instrument instances ... it is the responsibility of
    the user to allocate and deallocate all instruments.

    The tick() function returns the mix of all sounding voices.  Each
    noteOn returns a unique tag (credits to the NeXT MusicKit), so you
    can send control changes to specific voices within an ensemble.
    Alternately, control changes can be sent to all voices in a given
    group.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Voicer : public Stk
{
 public:
  //! Class constructor taking an optional note decay time (in seconds).
  Voicer( T decayTime = 0.2 );

  //! Add an instrument with an optional group number to the voice manager.
  /*!
    A set of instruments can be grouped by group number and
    controlled via the functions that take a group number argument.
  */
  void addInstrument( Instrmnt<T> *instrument, int group=0 );

  //! Remove the given instrument pointer from the voice manager's control.
  /*!
    It is important that any instruments which are to be deleted by
    the user while the voice manager is running be first removed from
    the manager's control via this function!!
  */
  void removeInstrument( Instrmnt<T> *instrument );

  //! Initiate a noteOn event with the given note number and amplitude and return a unique note tag.
  /*!
    Send the noteOn message to the first available unused voice.
    If all voices are sounding, the oldest voice is interrupted and
    sent the noteOn message.  If the optional group argument is
    non-zero, only voices in that group are used.  If no voices are
    found for a specified non-zero group value, the function returns
    -1.  The amplitude value should be in the range 0.0 - 128.0.
  */
  long noteOn( T noteNumber, T amplitude, int group=0 );

  //! Send a noteOff to all voices having the given noteNumber and optional group (default group = 0).
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( T noteNumber, T amplitude, int group=0 );

  //! Send a noteOff to the voice with the given note tag.
  /*!
    The amplitude value should be in the range 0.0 - 128.0.
  */
  void noteOff( long tag, T amplitude );

  //! Send a frequency update message to all voices assigned to the optional group argument (default group = 0).
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
  */
  void setFrequency( T noteNumber, int group=0 );

  //! Send a frequency update message to the voice with the given note tag.
  /*!
    The \e noteNumber argument corresponds to a MIDI note number, though it is a floating-point value and can range beyond the normal 0-127 range.
  */
  void setFrequency( long tag, T noteNumber );

  //! Send a pitchBend message to all voices assigned to the optional group argument (default group = 0).
  void pitchBend( T value, int group=0 );

  //! Send a pitchBend message to the voice with the given note tag.
  void pitchBend( long tag, T value );

  //! Send a controlChange to all instruments assigned to the optional group argument (default group = 0).
  void controlChange( int number, T value, int group=0 );

  //! Send a controlChange to the voice with the given note tag.
  void controlChange( long tag, int number, T value );

  //! Send a noteOff message to all existing voices.
  void silence( void );

  //! Return the current number of output channels.
  unsigned int channelsOut( void ) const { return lastFrame_.channels(); };

  //! Return an StkFrames<T> reference to the last output sample frame.
  const StkFrames<T>& lastFrame( void ) const { return lastFrame_; };

  //! Return the specified channel value of the last computed frame.
  /*!
    The \c channel argument must be less than the number of output
    channels, which can be determined with the channelsOut() function
    (the first channel is specified by 0).  However, range checking is
    only performed if _STK_DEBUG_ is defined during compilation, in
    which case an out-of-range value will trigger an StkError
    exception. \sa lastFrame()
  */
  T lastOut( unsigned int channel = 0 );

  //! Mix one sample frame of all sounding voices and return the specified \c channel value.
  /*!
    The \c channel argument must be less than the number of output
    channels, which can be determined with the channelsOut() function
    (the first channel is specified by 0).  However, range checking is
    only performed if _STK_DEBUG_ is defined during compilation, in
    which case an out-of-range value will trigger an StkError
    exception.
  */
  T tick( unsigned int channel = 0 );

  //! Fill the StkFrames<T> argument with computed frames and return the same reference.
  /*!
    The number of channels in the StkFrames<T> argument must equal
    the number of channels in the file data.  However, this is only
    checked if _STK_DEBUG_ is defined during compilation, in which
    case an incompatibility will trigger an StkError exception.  If no
    file data is loaded, the function does nothing (a warning will be
    issued if _STK_DEBUG_ is defined during compilation).
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

 protected:

  struct Voice {
    Instrmnt<T> *instrument;
    long tag;
    T noteNumber;
    T frequency;
    int sounding;
    int group;

    // Default constructor.
    Voice()
      :instrument(0), tag(0), noteNumber(-1.0), frequency(0.0), sounding(0), group(0) {}
  };

  std::vector<Voice> voices_;
  long tags_;
  int muteTime_;
  StkFrames<T> lastFrame_;
};


template<typename T>
inline T Voicer<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= lastFrame_.channels() ) {
    oStream_ << "Voicer::lastOut(): channel argument is invalid!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return lastFrame_[channel];
}


template<typename T>
inline T Voicer<T>::tick( unsigned int channel )
{
  unsigned int j;
  for ( j=0; j<lastFrame_.channels(); j++ ) lastFrame_[j] = 0.0;
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].sounding != 0 ) {
      voices_[i].instrument->tick();
      for ( j=0; j<voices_[i].instrument->channelsOut(); j++ ) lastFrame_[j] += voices_[i].instrument->lastOut( j );
    }
    if ( voices_[i].sounding < 0 )
      voices_[i].sounding++;
    if ( voices_[i].sounding == 0 )
      voices_[i].noteNumber = -1;
  }

  return lastFrame_[channel];
}


template<typename T>
inline StkFrames<T>& Voicer<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Voicer::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    tick();
    for ( j=0; j<nChannels; j++ )
      *samples++ = lastFrame_[j];
  }

  return frames;
}

/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of STK instrument
    classes.  Individual voices can be controlled via unique note
    tags.  Instrument groups can be controlled by group number.

    A previously constructed STK instrument class is linked with a
    voice manager using the addInstrument() function.  An optional
    group number argument can be specified to the addInstrument()
    function as well (default group = 0).  The voice manager does not
    delete any instrument instances ... it is the responsibility of
    the user to allocate and deallocate all instruments.

    The tick() function returns the mix of all sounding voices.  Each
    noteOn returns a unique tag (credits to the NeXT MusicKit), so you
    can send control changes to specific voices within an ensemble.
    Alternately, control changes can be sent to all voices in a given
    group.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
Voicer<T>::Voicer( T decayTime )
{
  if ( decayTime < 0.0 ) {
    oStream_ << "Voicer::Voicer: argument (" << decayTime << ") must be positive!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  tags_ = 23456;
  muteTime_ = (int) ( decayTime * stk::sampleRate() );
  lastFrame_.resize( 1, 1, 0.0 );
}


template<typename T>
void Voicer<T>::addInstrument( Instrmnt<T> *instrument, int group )
{
  Voicer::Voice voice;
  voice.instrument = instrument;
  voice.group = group;
  voice.noteNumber = -1;
  voices_.push_back( voice );

  // Check output channels and resize lastFrame_ if necessary.
  if ( instrument->channelsOut() > lastFrame_.channels() ) {
    unsigned int startChannel = lastFrame_.channels();
    lastFrame_.resize( 1, instrument->channelsOut() );
    for ( unsigned int i=startChannel; i<lastFrame_.size(); i++ )
      lastFrame_[i] = 0.0;
  }
}


template<typename T>
void Voicer<T>::removeInstrument( Instrmnt<T> *instrument )
{
  bool found = false;
  typename std::vector< Voicer::Voice >::iterator i;
  for ( i=voices_.begin(); i!=voices_.end(); ++i ) {
    if ( (*i).instrument != instrument ) continue;
    voices_.erase( i );
    found = true;
    break;
  }

  if ( found ) {
    // Check output channels and resize lastFrame_ if necessary.
    unsigned int maxChannels = 1;
    for ( i=voices_.begin(); i!=voices_.end(); ++i ) {
      if ( (*i).instrument->channelsOut() > maxChannels ) maxChannels = (*i).instrument->channelsOut();
    }
    if ( maxChannels < lastFrame_.channels() )
      lastFrame_.resize( 1, maxChannels );
  }
  else {
    oStream_ << "Voicer::removeInstrument: instrument pointer not found in current voices!";
    handleError( StkError::WARNING );
  }
}


template<typename T>
long Voicer<T>::noteOn(T noteNumber, T amplitude, int group )
{
  unsigned int i;
  T frequency = (T) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( i=0; i<voices_.size(); i++ ) {
    if (voices_[i].noteNumber < 0 && voices_[i].group == group) {
      voices_[i].tag = tags_++;
      voices_[i].group = group;
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
      voices_[i].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
      voices_[i].sounding = 1;
      return voices_[i].tag;
    }
  }

  // All voices are sounding, so interrupt the oldest voice.
  int voice = -1;
  for ( i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].group == group ) {
      if ( voice == -1 ) voice = i;
      else if ( voices_[i].tag < voices_[voice].tag ) voice = (int) i;
    }
  }

  if ( voice >= 0 ) {
    voices_[voice].tag = tags_++;
    voices_[voice].group = group;
    voices_[voice].noteNumber = noteNumber;
    voices_[voice].frequency = frequency;
    voices_[voice].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
    voices_[voice].sounding = 1;
    return voices_[voice].tag;
  }

  return -1;
}


template<typename T>
void Voicer<T>::noteOff( T noteNumber, T amplitude, int group )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].noteNumber == noteNumber && voices_[i].group == group ) {
      voices_[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices_[i].sounding = -muteTime_;
    }
  }
}


template<typename T>
void Voicer<T>::noteOff( long tag, T amplitude )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices_[i].sounding = -muteTime_;
      break;
    }
  }
}


template<typename T>
void Voicer<T>::setFrequency( T noteNumber, int group )
{
  T frequency = (T) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].group == group ) {
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
      voices_[i].instrument->setFrequency( frequency );
    }
  }
}


template<typename T>
void Voicer<T>::setFrequency( long tag, T noteNumber )
{
  T frequency = (T) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
      voices_[i].instrument->setFrequency( frequency );
      break;
    }
  }
}


template<typename T>
void Voicer<T>::pitchBend( T value, int group )
{
  T pitchScaler;
  if ( value < 8192.0 )
    pitchScaler = pow( 0.5, (8192.0-value) / 8192.0 );
  else
    pitchScaler = pow( 2.0, (value-8192.0) / 8192.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].group == group )
      voices_[i].instrument->setFrequency( (T) (voices_[i].frequency * pitchScaler) );
  }
}

template<typename T>
void Voicer<T>::pitchBend( long tag, T value )
{
  T pitchScaler;
  if ( value < 8192.0 )
    pitchScaler = pow( 0.5, (8192.0-value) / 8192.0 );
  else
    pitchScaler = pow( 2.0, (value-8192.0) / 8192.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->setFrequency( (T) (voices_[i].frequency * pitchScaler) );
      break;
    }
  }
}


template<typename T>
void Voicer<T>::controlChange( int number, T value, int group )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].group == group )
      voices_[i].instrument->controlChange( number, value );
  }
}

template<typename T>
void Voicer<T>::controlChange( long tag, int number, T value )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->controlChange( number, value );
      break;
    }
  }
}

template<typename T>
void Voicer<T>::silence( void )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].sounding > 0 )
      voices_[i].instrument->noteOff( 0.5 );
  }
}

} // stk namespace
