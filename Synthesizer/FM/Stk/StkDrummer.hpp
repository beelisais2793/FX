#pragma once
#include "StkInstrmnt.hpp"
#include "StkFileWvIn.hpp"
#include "StkOnePole.hpp"

namespace stk {

/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

const int DRUM_NUMWAVES = 11;
const int DRUM_POLYPHONY = 4;

template<typename T>
class Drummer : public Instrmnt<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Drummer( void );

  //! Class destructor.
  ~Drummer( void );

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a particular
    instrument.  An StkError will be thrown if the rawwave path is
    incorrectly set.
  */
  void noteOn( T instrument, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

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

  FileWvIn<T> waves_[DRUM_POLYPHONY];
  OnePole<T>  filters_[DRUM_POLYPHONY];
  std::vector<int> soundOrder_;
  std::vector<int> soundNumber_;
  int      nSounding_;
};

template<typename T>
inline T Drummer<T>::tick( unsigned int )
{
  this->lastFrame_[0] = 0.0;
  if ( nSounding_ == 0 ) return this->lastFrame_[0];

  for ( int i=0; i<DRUM_POLYPHONY; i++ ) {
    if ( soundOrder_[i] >= 0 ) {
      if ( waves_[i].isFinished() ) {
        // Re-order the list.
        for ( int j=0; j<DRUM_POLYPHONY; j++ ) {
          if ( soundOrder_[j] > soundOrder_[i] )
            soundOrder_[j] -= 1;
        }
        soundOrder_[i] = -1;
        nSounding_--;
      }
      else
        this->lastFrame_[0] += filters_[i].tick( waves_[i].tick() );
    }
  }

  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& Drummer<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Drummer::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using FileWvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

//extern unsigned char genMIDIMap[128];
//extern char waveNames[DRUM_NUMWAVES][16];
// Not really General MIDI yet.
unsigned char genMIDIMap[128] =
  { 0,0,0,0,0,0,0,0,		// 0-7
    0,0,0,0,0,0,0,0,		// 8-15
    0,0,0,0,0,0,0,0,		// 16-23
    0,0,0,0,0,0,0,0,		// 24-31
    0,0,0,0,1,0,2,0,		// 32-39
    2,3,6,3,6,4,7,4,		// 40-47
    5,8,5,0,0,0,10,0,		// 48-55
    9,0,0,0,0,0,0,0,		// 56-63
    0,0,0,0,0,0,0,0,		// 64-71
    0,0,0,0,0,0,0,0,		// 72-79
    0,0,0,0,0,0,0,0,		// 80-87
    0,0,0,0,0,0,0,0,		// 88-95
    0,0,0,0,0,0,0,0,		// 96-103
    0,0,0,0,0,0,0,0,		// 104-111
    0,0,0,0,0,0,0,0,		// 112-119
    0,0,0,0,0,0,0,0     // 120-127
  };
				  
char waveNames[DRUM_NUMWAVES][16] =
  { 
    "dope.raw",
    "bassdrum.raw",
    "snardrum.raw",
    "tomlowdr.raw",
    "tommiddr.raw",
    "tomhidrm.raw",
    "hihatcym.raw",
    "ridecymb.raw",
    "crashcym.raw", 
    "cowbell1.raw", 
    "tambourn.raw"
  };


template<typename T>
Drummer<T>::Drummer( void ) : Instrmnt<T>()
{
  // This counts the number of sounding voices.
  nSounding_ = 0;
  soundOrder_ = std::vector<int> (DRUM_POLYPHONY, -1);
  soundNumber_ = std::vector<int> (DRUM_POLYPHONY, -1);
}

template<typename T>
Drummer<T>::~Drummer( void )
{
}

template<typename T>
void Drummer<T>::noteOn( T instrument, T amplitude )
{
  if ( amplitude < 0.0 || amplitude > 1.0 ) {
    oStream_ << "Drummer::noteOn: amplitude parameter is out of bounds!";
    handleError( StkError::WARNING ); return;
  }

  // Yes, this is tres kludgey.
  int noteNumber = (int) ( ( 12 * log( instrument / 220.0 ) / log( 2.0 ) ) + 57.01 );

  // If we already have a wave of this note number loaded, just reset
  // it.  Otherwise, look first for an unused wave or preempt the
  // oldest if already at maximum polyphony.
  int iWave;
  for ( iWave=0; iWave<DRUM_POLYPHONY; iWave++ ) {
    if ( soundNumber_[iWave] == noteNumber ) {
      if ( waves_[iWave].isFinished() ) {
        soundOrder_[iWave] = nSounding_;
        nSounding_++;
      }
      waves_[iWave].reset();
      filters_[iWave].setPole( 0.999 - (amplitude * 0.6) );
      filters_[iWave].setGain( amplitude );
      break;
    }
  }

  if ( iWave == DRUM_POLYPHONY ) { // This note number is not currently loaded.
    if ( nSounding_ < DRUM_POLYPHONY ) {
      for ( iWave=0; iWave<DRUM_POLYPHONY; iWave++ )
        if ( soundOrder_[iWave] < 0 ) break;
      nSounding_ += 1;
    }
    else { // interrupt oldest voice
      for ( iWave=0; iWave<DRUM_POLYPHONY; iWave++ )
        if ( soundOrder_[iWave] == 0 ) break;
      // Re-order the list.
      for ( int j=0; j<DRUM_POLYPHONY; j++ ) {
        if ( soundOrder_[j] > soundOrder_[iWave] )
          soundOrder_[j] -= 1;
      }
    }
    soundOrder_[iWave] = nSounding_ - 1;
    soundNumber_[iWave] = noteNumber;
    //std::cout << "iWave = " << iWave << ", nSounding = " << nSounding_ << ", soundOrder[] = " << soundOrder_[iWave] << std::endl;

    // Concatenate the STK rawwave path to the rawwave file
    waves_[iWave].openFile( (rawwavePath() + waveNames[ genMIDIMap[ noteNumber ] ]).c_str(), true );
    if ( sampleRate() != 22050.0 )
      waves_[iWave].setRate( 22050.0 / sampleRate() );
    filters_[iWave].setPole( 0.999 - (amplitude * 0.6) );
    filters_[iWave].setGain( amplitude );
  }

  /*
#if defined(_STK_DEBUG_)
  oStream_ << "Drummer::noteOn: number sounding = " << nSounding_ << ", notes: ";
  for ( int i=0; i<nSounding_; i++ ) oStream_ << soundNumber_[i] << "  ";
  oStream_ << '\n';
  handleError( StkError::WARNING );
#endif
  */
}

template<typename T>
void Drummer<T>::noteOff( T amplitude )
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while ( i < nSounding_ ) filters_[i++].setGain( amplitude * 0.01 );
}

} // stk namespace

