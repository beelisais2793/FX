#pragma once
#include <vector>
#include "StkGenerator.hpp"
#include "StkEnvelope.hpp"
#include "StkNoise.hpp"
#include "StkFileRead.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class Granulate
    \brief STK granular synthesis class.

    This class implements a real-time granular synthesis algorithm
    that operates on an input soundfile.  Multi-channel files are
    supported.  Various functions are provided to allow control over
    voice and grain parameters.

    The functionality of this class is based on the program MacPod by
    Chris Rolfe and Damian Keller, though there are likely to be a
    number of differences in the actual implementation.

    by Gary Scavone, 2005--2021.
*/
/***************************************************/

template<typename T>
class Granulate: public Generator<T>
{
 public:
  //! Default constructor.
  Granulate( void );

  //! Constructor taking input audio file and number of voices arguments.
  Granulate( unsigned int nVoices, std::string fileName, bool typeRaw = false );

  //! Class destructor.
  ~Granulate( void );

  //! Load a monophonic soundfile to be "granulated".
  /*!
    An StkError will be thrown if the file is not found, its format
    is unknown or unsupported, or the file has more than one channel.
  */
  void openFile( std::string fileName, bool typeRaw = false );

  //! Reset the file pointer and all existing grains to the file start.
  /*!
    Multiple grains are offset from one another in time by grain
    duration / nVoices.
  */
  void reset( void );

  //! Set the number of simultaneous grain "voices" to use.
  /*!
    Multiple grains are offset from one another in time by grain
    duration / nVoices.  For this reason, it is best to set the grain
    parameters before calling this function (during initialization).
  */
  void setVoices( unsigned int nVoices = 1 );

  //! Set the stretch factor used for grain playback (1 - 1000).
  /*!
    Granular synthesis allows for time-stetching without affecting
    the original pitch of a sound.  A stretch factor of 4 will produce
    a resulting sound of length 4 times the orignal sound.  The
    default parameter of 1 produces no stretching.
  */
  void setStretch( unsigned int stretchFactor = 1 );

  //! Set global grain parameters used to determine individual grain settings.
  /*!
    Each grain is defined as having a length of \e duration
    milliseconds which must be greater than zero.  For values of \e
    rampPercent (0 - 100) greater than zero, a linear envelope will be
    applied to each grain.  If \e rampPercent = 100, the resultant
    grain "window" is triangular while \e rampPercent = 50 produces a
    trapezoidal window.  In addition, each grain can have a time delay
    of length \e delay and a grain pointer increment of length \e
    offset, which can be negative, before the next ramp onset (in
    milliseconds).  The \e offset parameter controls grain pointer
    jumps between enveloped grain segments, while the \e delay
    parameter causes grain calculations to pause between grains.  The
    actual values calculated for each grain will be randomized by a
    factor set using the setRandomFactor() function.
  */
  void setGrainParameters( unsigned int duration = 30, unsigned int rampPercent = 50,
                           int offset = 0, unsigned int delay = 0 );

  //! This factor is used when setting individual grain parameters (0.0 - 1.0).
  /*!
    This random factor is applied when all grain state durations
    are calculated.  If set to 0.0, no randomness occurs.  When
    randomness = 1.0, a grain segment of length \e duration will be
    randomly augmented by up to +- \e duration seconds (i.e., a 30
    millisecond length will be augmented by an extra length of up to
    +30 or -30 milliseconds).
   */
  void setRandomFactor( T randomness = 0.1 );

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

  //! Compute one sample frame and return the specified \c channel value.
  T tick( unsigned int channel = 0 );

  //! Fill the StkFrames<T> object with computed sample frames, starting at the specified channel.
  /*!
    The \c channel argument plus the number of output channels must
    be less than the number of channels in the StkFrames<T> argument (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  enum GrainState {
    GRAIN_STOPPED,
    GRAIN_FADEIN,
    GRAIN_SUSTAIN,
    GRAIN_FADEOUT
  };

 protected:
   
  struct Grain {
    T eScaler;
    T eRate;
    unsigned long attackCount;
    unsigned long sustainCount;
    unsigned long decayCount;
    unsigned long delayCount;
    unsigned long counter;
    //unsigned long pointer;
    T pointer;
    unsigned long startPointer;
    unsigned int repeats;
    GrainState state;

    // Default constructor.
    Grain()
      :eScaler(0.0), eRate(0.0), attackCount(0), sustainCount(0), decayCount(0),
       delayCount(0), counter(0), pointer(0), startPointer(0), repeats(0), state(GRAIN_STOPPED) {}
  };

  void calculateGrain( Granulate<T>::Grain& grain );

  StkFrames<T> data_;
  std::vector<Grain> grains_;
  Noise<T> noise;
  //long gPointer_;
  T gPointer_;

  // Global grain parameters.
  unsigned int gDuration_;
  unsigned int gRampPercent_;
  unsigned int gDelay_;
  unsigned int gStretch_;
  unsigned int stretchCounter_;
  int gOffset_;
  T gRandomFactor_;
  T gain_;

};

template<typename T>
inline T Granulate<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= this->lastFrame_.channels() ) {
    oStream_ << "Granulate::lastOut(): channel argument is invalid!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}

template<typename T>
inline StkFrames<T>& Granulate<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Granulate::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples++ = tick();
    for ( j=1; j<nChannels; j++ )
      *samples++ = this->lastFrame_[j];
  }

  return frames;
}

/***************************************************/
/*! \class Granulate
    \brief STK granular synthesis class.

    This class implements a real-time granular synthesis algorithm
    that operates on an input soundfile.  Multi-channel files are
    supported.  Various functions are provided to allow control over
    voice and grain parameters.

    The functionality of this class is based on the program MacPod by
    Chris Rolfe and Damian Keller, though there are likely to be a
    number of differences in the actual implementation.

    by Gary Scavone, 2005--2021.
*/
/***************************************************/

template<typename T>
Granulate<T>::Granulate( void )
{
  this->setGrainParameters(); // use default values
  this->setRandomFactor();
  gStretch_ = 0;
  stretchCounter_ = 0;
  gain_ = 1.0;
}

template<typename T>
Granulate<T>::Granulate( unsigned int nVoices, std::string fileName, bool typeRaw )
{
  this->setGrainParameters(); // use default values
  this->setRandomFactor();
  gStretch_ = 0;
  stretchCounter_ = 0;
  this->openFile( fileName, typeRaw );
  this->setVoices( nVoices );
}

template<typename T>
Granulate<T>::~Granulate( void )
{
}

template<typename T>
void Granulate<T>::setStretch( unsigned int stretchFactor )
{
  if ( stretchFactor <= 1 )
    gStretch_ = 0;
  else if ( gStretch_ >= 1000 )
    gStretch_ = 1000;
  else
    gStretch_ = stretchFactor - 1;
}

template<typename T>
void Granulate<T>::setGrainParameters( unsigned int duration, unsigned int rampPercent,
                                      int offset, unsigned int delay )
{
  gDuration_ = duration;
  if ( gDuration_ == 0 ) {
    gDuration_ = 1;
    oStream_ << "Granulate::setGrainParameters: duration argument cannot be zero ... setting to 1 millisecond.";
    handleError( StkError::WARNING );
  }

  gRampPercent_ = rampPercent;
  if ( gRampPercent_ > 100 ) {
    gRampPercent_ = 100;
    oStream_ << "Granulate::setGrainParameters: rampPercent argument cannot be greater than 100 ... setting to 100.";
    handleError( StkError::WARNING );
  }

  gOffset_ = offset;
  gDelay_ = delay;
}

template<typename T>
void Granulate<T>::setRandomFactor( T randomness )
{
  if ( randomness < 0.0 ) gRandomFactor_ = 0.0;
  else if ( randomness > 1.0 ) gRandomFactor_ = 0.97;
  else gRandomFactor_ = 0.97 * randomness;
}

template<typename T>
void Granulate<T>::openFile( std::string fileName, bool typeRaw )
{
  // Attempt to load the soundfile data.
  FileRead file( fileName, typeRaw );
  data_.resize( file.fileSize(), file.channels() );
  file.read( data_ );
  this->lastFrame_.resize( 1, file.channels(), 0.0 );

  this->reset();

#if defined(_STK_DEBUG_)
  std::ostringstream message;
  message << "Granulate::openFile: file = " << fileName << ", file frames = " << file.fileSize() << '.';
  handleError( message.str(), StkError::DEBUG_PRINT );
#endif

}

template<typename T>
void Granulate<T>::reset( void )
{
  gPointer_ = 0;

  // Reset grain parameters.
  size_t count;
  size_t nVoices = (unsigned int)grains_.size();
  for ( unsigned int i=0; i<grains_.size(); i++ ) {
    grains_[i].repeats = 0;
    count = ( i * gDuration_ * 0.001 * stk::sampleRate() / nVoices );
    grains_[i].counter = count;
    grains_[i].state = GRAIN_STOPPED;
  }

  for ( unsigned int i=0; i<this->lastFrame_.channels(); i++ )
    this->lastFrame_[i] = 0.0;
}

template<typename T>
void Granulate<T>::setVoices( unsigned int nVoices )
{
#if defined(_STK_DEBUG_)
  std::ostringstream message;
  message << "Granulate::setVoices: nVoices = " << nVoices << ", existing voices = " << grains_.size() << '.';
  handleError( message.str(), StkError::DEBUG_PRINT );
#endif

  size_t oldSize = grains_.size();
  grains_.resize( nVoices );

  // Initialize new grain voices.
  size_t count;
  for ( size_t i=oldSize; i<nVoices; i++ ) {
    grains_[i].repeats = 0;
    count = ( i * gDuration_ * 0.001 * stk::sampleRate() / nVoices );
    grains_[i].counter = count;
    grains_[i].pointer = gPointer_;
    grains_[i].state = GRAIN_STOPPED;
  }

  gain_ = 1.0 / grains_.size();
}

template<typename T>
void Granulate<T>::calculateGrain( Granulate<T>::Grain& grain )
{
  if ( grain.repeats > 0 ) {
    grain.repeats--;
    grain.pointer = grain.startPointer;
    if ( grain.attackCount > 0 ) {
      grain.eScaler = 0.0;
      grain.eRate = -grain.eRate;
      grain.counter = grain.attackCount;
      grain.state = GRAIN_FADEIN;
    }
    else {
      grain.counter = grain.sustainCount;
      grain.state = GRAIN_SUSTAIN;
    }
    return;
  }

  // Calculate duration and envelope parameters.
  T seconds = gDuration_ * 0.001;
  seconds += ( seconds * gRandomFactor_ * noise.tick() );
  unsigned long count = (unsigned long) ( seconds * stk::sampleRate() );
  grain.attackCount = (unsigned int) ( gRampPercent_ * 0.005 * count );
  grain.decayCount = grain.attackCount;
  grain.sustainCount = count - 2 * grain.attackCount;
  grain.eScaler = 0.0;
  if ( grain.attackCount > 0 ) {
    grain.eRate = 1.0 / grain.attackCount;
    grain.counter = grain.attackCount;
    grain.state = GRAIN_FADEIN;
  }
  else {
    grain.counter = grain.sustainCount;
    grain.state = GRAIN_SUSTAIN;
  }

  // Calculate delay parameter.
  seconds = gDelay_ * 0.001;
  seconds += ( seconds * gRandomFactor_ * noise.tick() );
  count = (unsigned long) ( seconds * stk::sampleRate() );
  grain.delayCount = count;

  // Save stretch parameter.
  grain.repeats = gStretch_;

  // Calculate offset parameter.
  seconds = gOffset_ * 0.001;
  seconds += ( seconds * gRandomFactor_ * std::abs( noise.tick() ) );
  int offset = (int) ( seconds * stk::sampleRate() );

  // Add some randomization to the pointer start position.
  seconds = gDuration_ * 0.001 * gRandomFactor_ * noise.tick();
  offset += (int) ( seconds * stk::sampleRate() );
  grain.pointer += offset;
  while ( grain.pointer >= data_.frames() ) grain.pointer -= data_.frames();
  if ( grain.pointer <  0 ) grain.pointer = 0;
  grain.startPointer = grain.pointer;
}

template<typename T>
T Granulate<T>::tick( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "Granulate::tick(): channel argument and soundfile data are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  unsigned int i, j, nChannels = this->lastFrame_.channels();
  for ( j=0; j<nChannels; j++ ) this->lastFrame_[j] = 0.0;

  if ( data_.size() == 0 ) return 0.0;

  T sample;
  for ( i=0; i<grains_.size(); i++ ) {

    if ( grains_[i].counter == 0 ) { // Update the grain state.

      switch ( grains_[i].state ) {

      case GRAIN_STOPPED:
        // We're done waiting between grains ... setup for new grain
        this->calculateGrain( grains_[i] );
        break;

      case GRAIN_FADEIN:
        // We're done ramping up the envelope
        if ( grains_[i].sustainCount > 0 ) {
          grains_[i].counter = grains_[i].sustainCount;
          grains_[i].state = GRAIN_SUSTAIN;
          break;
        }
        // else no sustain state (i.e. perfect triangle window)

      case GRAIN_SUSTAIN:
        // We're done with flat part of envelope ... setup to ramp down
        if ( grains_[i].decayCount > 0 ) {
          grains_[i].counter = grains_[i].decayCount;
          grains_[i].eRate = -grains_[i].eRate;
          grains_[i].state = GRAIN_FADEOUT;
          break;
        }
        // else no fade out state (gRampPercent = 0)

      case GRAIN_FADEOUT:
        // We're done ramping down ... setup for wait between grains
        if ( grains_[i].delayCount > 0 ) {
          grains_[i].counter = grains_[i].delayCount;
          grains_[i].state = GRAIN_STOPPED;
          break;
        }
        // else no delay (gDelay = 0)

        this->calculateGrain( grains_[i] );
      }
    }

    // Accumulate the grain outputs.
    if ( grains_[i].state > 0 ) {
      for ( j=0; j<nChannels; j++ ) {
        sample = data_[ nChannels * grains_[i].pointer + j ];

        if ( grains_[i].state == GRAIN_FADEIN || grains_[i].state == GRAIN_FADEOUT ) {
          sample *= grains_[i].eScaler;
          grains_[i].eScaler += grains_[i].eRate;
        }

        this->lastFrame_[j] += sample;
      }


      // Increment and check pointer limits.
      grains_[i].pointer++;
      if ( grains_[i].pointer >= data_.frames() )
        grains_[i].pointer = 0;
    }

    // Decrement counter for all states.
    grains_[i].counter--;
  }

  // Increment our global file pointer at the stretch rate.
  if ( stretchCounter_++ == gStretch_ ) {
    gPointer_++;
    if ( (unsigned long) gPointer_ >= data_.frames() ) gPointer_ = 0;
    stretchCounter_ = 0;
  }

  return this->lastFrame_[channel];
}

} // stk namespace

