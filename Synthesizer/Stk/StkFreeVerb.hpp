#pragma once
#include "StkEffect.hpp"
#include "StkDelay.hpp"
#include "StkOnePole.hpp"

namespace stk {

/***********************************************************************/
/*! \class FreeVerb
    \brief Jezar at Dreampoint's FreeVerb, implemented in STK.

    Freeverb is a free and open-source Schroeder reverberator
    originally implemented in C++. The parameters of the reverberation
    model are exceptionally well tuned. FreeVerb uses 8
    lowpass-feedback-comb-filters in parallel, followed by 4 Schroeder
    allpass filters in series.  The input signal can be either mono or
    stereo, and the output signal is stereo.  The delay lengths are
    optimized for a sample rate of 44100 Hz.

    Ported to STK by Gregory Burlet, 2012.
*/
/***********************************************************************/

template<typename T>
class FreeVerb : public Effect<T>
{   
 public:
  //! FreeVerb Constructor
  /*!
    Initializes the effect with default parameters. Note that these defaults
    are slightly different than those in the original implementation of
    FreeVerb [Effect Mix: 0.75; Room Size: 0.75; Damping: 0.25; Width: 1.0;
    Mode: freeze mode off].
  */
  FreeVerb();

  //! Destructor
  ~FreeVerb();

  //! Set the effect mix [0 = mostly dry, 1 = mostly wet].
  void setEffectMix( T mix );

  //! Set the room size (comb filter feedback gain) parameter [0,1].
  void setRoomSize( T value );

  //! Get the room size (comb filter feedback gain) parameter.
  T getRoomSize( void );

  //! Set the damping parameter [0=low damping, 1=higher damping].
  void setDamping( T value );

  //! Get the damping parameter.
  T getDamping( void );

  //! Set the width (left-right mixing) parameter [0,1].
  void setWidth( T value );

  //! Get the width (left-right mixing) parameter.
  T getWidth( void );

  //! Set the mode [frozen = 1, unfrozen = 0].
  void setMode( bool isFrozen );

  //! Get the current freeze mode [frozen = 1, unfrozen = 0].
  T getMode( void );

  //! Clears delay lines, etc.
  void clear( void );

  //! Return the specified channel value of the last computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the last
    computed stereo frame.  The \c channel argument must be 0 or 1
    (the first channel is specified by 0).  However, range checking is
    only performed if _STK_DEBUG_ is defined during compilation, in
    which case an out-of-range value will trigger an StkError
    exception.
  */
  T lastOut( unsigned int channel = 0 );

  //! Input one or two samples to the effect and return the specified \c channel value of the computed stereo frame.
  /*!
    Use the lastFrame() function to get both values of the computed
    stereo output frame. The \c channel argument must be 0 or 1 (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  T tick( T inputL, T inputR = 0.0, unsigned int channel = 0 );

  //! Take two channels of the StkFrames<T> object as inputs to the effect and replace with stereo outputs.
  /*!
    The StkFrames<T> argument reference is returned.  The stereo
    inputs are taken from (and written back to) the StkFrames<T> argument
    starting at the specified \c channel.  Therefore, the \c channel
    argument must be less than ( channels() - 1 ) of the StkFrames<T>
    argument (the first channel is specified by 0).  However, range
    checking is only performed if _STK_DEBUG_ is defined during
    compilation, in which case an out-of-range value will trigger an
    StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

  //! Take one or two channels of the \c iFrames object as inputs to the effect and write stereo outputs to the \c oFrames object.
  /*!
    The \c iFrames object reference is returned.  The \c iChannel
    argument must be less than the number of channels in the \c
    iFrames argument (the first channel is specified by 0).  If more
    than one channel of data exists in \c iFrames starting from \c
    iChannel, stereo data is input to the effect.  The \c oChannel
    argument must be less than ( channels() - 1 ) of the \c oFrames
    argument.  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  StkFrames<T>& tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel = 0, unsigned int oChannel = 0 );

 protected:
  //! Update interdependent parameters.
  void update( void );

  // Clamp very small floats to zero, version from
  // http://music.columbia.edu/pipermail/linux-audio-user/2004-July/013489.html .
  // However, this is for 32-bit floats only.
  //static inline T undenormalize( volatile T s ) { 
  //  s += 9.8607615E-32f; 
  //  return s - 9.8607615E-32f; 
  //}
    
  static const int nCombs = 8;
  static const int nAllpasses = 4;
  static const int stereoSpread = 23;
  static const T fixedGain;
  static const T scaleWet;
  static const T scaleDry;
  static const T scaleDamp;
  static const T scaleRoom;
  static const T offsetRoom;

  // Delay line lengths for 44100Hz sampling rate.
  static int cDelayLengths[nCombs];
  static int aDelayLengths[nAllpasses];

  T g_;        // allpass coefficient
  T gain_;
  T roomSizeMem_, roomSize_;
  T dampMem_, damp_;
  T wet1_, wet2_;
  T dry_;
  T width_;
  bool frozenMode_;

  // LBFC: Lowpass Feedback Comb Filters
  Delay<T> combDelayL_[nCombs];
  Delay<T> combDelayR_[nCombs];
  OnePole<T> combLPL_[nCombs];
  OnePole<T> combLPR_[nCombs];
        
  // AP: Allpass Filters
  Delay<T> allPassDelayL_[nAllpasses];
  Delay<T> allPassDelayR_[nAllpasses];
};

template<typename T>
inline T FreeVerb<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "FreeVerb<T>::lastOut(): channel argument must be less than 2!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  return this->lastFrame_[channel];
}

template<typename T>
inline T FreeVerb<T>::tick( T inputL, T inputR, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel > 1 ) {
    oStream_ << "FreeVerb<T>::tick(): channel argument must be less than 2!";
    handleError(StkError::FUNCTION_ARGUMENT);
  }
#endif

  T fInput = (inputL + inputR) * gain_;
  T outL = 0.0;
  T outR = 0.0;

  // Parallel LBCF filters
  for ( int i = 0; i < nCombs; i++ ) {
    // Left channel
    //T yn = fInput + (roomSize_ * FreeVerb<T>::undenormalize(combLPL_[i].tick(FreeVerb<T>::undenormalize(combDelayL_[i].nextOut()))));
    T yn = fInput + (roomSize_ * combLPL_[i].tick( combDelayL_[i].nextOut() ) );
    combDelayL_[i].tick(yn);
    outL += yn;

    // Right channel
    //yn = fInput + (roomSize_ * FreeVerb<T>::undenormalize(combLPR_[i].tick(FreeVerb<T>::undenormalize(combDelayR_[i].nextOut()))));
    yn = fInput + (roomSize_ * combLPR_[i].tick( combDelayR_[i].nextOut() ) );
    combDelayR_[i].tick(yn);
    outR += yn;
  }

  // Series allpass filters
  for ( int i = 0; i < nAllpasses; i++ ) {
    // Left channel
    //T vn_m = FreeVerb<T>::undenormalize(allPassDelayL_[i].nextOut());
    T vn_m = allPassDelayL_[i].nextOut();
    T vn = outL + (g_ * vn_m);
    allPassDelayL_[i].tick(vn);
        
    // calculate output
    outL = -vn + (1.0 + g_)*vn_m;

    // Right channel
    //vn_m = FreeVerb<T>::undenormalize(allPassDelayR_[i].nextOut());
    vn_m = allPassDelayR_[i].nextOut();
    vn = outR + (g_ * vn_m);
    allPassDelayR_[i].tick(vn);

    // calculate output
    outR = -vn + (1.0 + g_)*vn_m;
  }

  // Mix output
  this->lastFrame_[0] = outL*wet1_ + outR*wet2_ + inputL*dry_;
  this->lastFrame_[1] = outR*wet1_ + outL*wet2_ + inputR*dry_;

  /*
  // Hard limiter ... there's not much else we can do at this point
  if ( this->lastFrame_[0] >= 1.0 ) {
    this->lastFrame_[0] = 0.9999;
  }
  if ( this->lastFrame_[0] <= -1.0 ) {
    this->lastFrame_[0] = -0.9999;
  }
  if ( this->lastFrame_[1] >= 1.0 ) {
    this->lastFrame_[1] = 0.9999;
  }
  if ( this->lastFrame_[1] <= -1.0 ) {
    this->lastFrame_[1] = -0.9999;
  }
  */

  return this->lastFrame_[channel];
}

// Set static delay line lengths
template<typename T> const T FreeVerb<T>::fixedGain = 0.015;
template<typename T> const T FreeVerb<T>::scaleWet = 3;
template<typename T> const T FreeVerb<T>::scaleDry = 2;
template<typename T> const T FreeVerb<T>::scaleDamp = 0.4;
template<typename T> const T FreeVerb<T>::scaleRoom = 0.28;
template<typename T> const T FreeVerb<T>::offsetRoom = 0.7;
template<typename T> int FreeVerb<T>::cDelayLengths[] = {1617, 1557, 1491, 1422, 1356, 1277, 1188, 1116};
template<typename T> int FreeVerb<T>::aDelayLengths[] = {225, 556, 441, 341};

template<typename T>
FreeVerb<T>::FreeVerb( void )
{
  // Resize this->lastFrame_ for stereo output
  this->lastFrame_.resize( 1, 2, 0.0 );

  // Initialize parameters
  Effect<T>::setEffectMix( 0.75 ); // set initially to 3/4 wet 1/4 dry signal (different than original freeverb) 
  roomSizeMem_ = (0.75 * scaleRoom) + offsetRoom; // feedback attenuation in LBFC
  dampMem_ = 0.25 * scaleDamp;                    // pole of lowpass filters in the LBFC
  width_ = 1.0;
  frozenMode_ = false;
  update();

  gain_ = fixedGain;      // input gain before sending to filters
  g_ = 0.5;               // allpass coefficient, immutable in FreeVerb

  // Scale delay line lengths according to the current sampling rate
  double fsScale = stk::sampleRate() / 44100.0;
  if ( fsScale != 1.0 ) {
    // scale comb filter delay lines
    for ( int i = 0; i < nCombs; i++ ) {
      cDelayLengths[i] = (int) floor(fsScale * cDelayLengths[i]);
    }

    // Scale allpass filter delay lines
    for ( int i = 0; i < nAllpasses; i++ ) {
      aDelayLengths[i] = (int) floor(fsScale * aDelayLengths[i]);
    }
  }

  // Initialize delay lines for the LBFC filters
  for ( int i = 0; i < nCombs; i++ ) {
    combDelayL_[i].setMaximumDelay( cDelayLengths[i] );
    combDelayL_[i].setDelay( cDelayLengths[i] );
    combDelayR_[i].setMaximumDelay( cDelayLengths[i] + stereoSpread );
    combDelayR_[i].setDelay( cDelayLengths[i] + stereoSpread );
  }

  // initialize delay lines for the allpass filters
  for (int i = 0; i < nAllpasses; i++) {
    allPassDelayL_[i].setMaximumDelay( aDelayLengths[i] );
    allPassDelayL_[i].setDelay( aDelayLengths[i] );
    allPassDelayR_[i].setMaximumDelay( aDelayLengths[i] + stereoSpread );
    allPassDelayR_[i].setDelay( aDelayLengths[i] + stereoSpread );
  }
}

template<typename T>
FreeVerb<T>::~FreeVerb()
{
}

template<typename T>
void FreeVerb<T>::setEffectMix( T mix )
{
  Effect<T>::setEffectMix( mix );
  update();    
}

template<typename T>
void FreeVerb<T>::setRoomSize( T roomSize )
{
  roomSizeMem_ = (roomSize * scaleRoom) + offsetRoom;
  update();
}

template<typename T>
T FreeVerb<T>::getRoomSize()
{
  return (roomSizeMem_ - offsetRoom) / scaleRoom;
}

template<typename T>
void FreeVerb<T>::setDamping( T damping )
{
  dampMem_ = damping * scaleDamp;
  update();
}

template<typename T>
T FreeVerb<T>::getDamping()
{
  return dampMem_ / scaleDamp;
}

template<typename T>
void FreeVerb<T>::setWidth( T width )
{
  width_ = width;
  update();
}

template<typename T>
T FreeVerb<T>::getWidth()
{
  return width_;
}

template<typename T>
void FreeVerb<T>::setMode( bool isFrozen )
{
  frozenMode_ = isFrozen;
  update();
}

template<typename T>
T FreeVerb<T>::getMode()
{
  return frozenMode_;
}

template<typename T>
void FreeVerb<T>::update()
{
  T wet = scaleWet * this->effectMix_;
  dry_ = scaleDry * (1.0-this->effectMix_);

  // Use the L1 norm so the output gain will sum to one while still
  // preserving the ratio of scalings in original FreeVerb
  wet /= (wet + dry_);
  dry_ /= (wet + dry_);

  wet1_ = wet * (width_/2.0 + 0.5);
  wet2_ = wet * (1.0 - width_)/2.0;

  if ( frozenMode_ ) {
    // put into freeze mode
    roomSize_ = 1.0;
    damp_ = 0.0;
    gain_ = 0.0;
  }
  else {
    roomSize_ = roomSizeMem_;
    damp_ = dampMem_;
    gain_ = fixedGain;
  }

  for ( int i=0; i<nCombs; i++ ) {
    // set low pass filter for delay output
    combLPL_[i].setCoefficients(1.0 - damp_, -damp_);
    combLPR_[i].setCoefficients(1.0 - damp_, -damp_);
  }
}

template<typename T>
void FreeVerb<T>::clear()
{
  // Clear LBFC delay lines
  for ( int i = 0; i < nCombs; i++ ) {
    combDelayL_[i].clear();
    combDelayR_[i].clear();
  }

  // Clear allpass delay lines
  for ( int i = 0; i < nAllpasses; i++ ) {
    allPassDelayL_[i].clear();
    allPassDelayR_[i].clear();
  }

  this->lastFrame_[0] = 0.0;
  this->lastFrame_[1] = 0.0;
}

template<typename T>
StkFrames<T>& FreeVerb<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() - 1 ) {
    oStream_ << "FreeVerb<T>::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
    *samples = tick( *samples, *(samples+1) );
    *(samples+1) = this->lastFrame_[1];
  }

  return frames;
}

template<typename T>
StkFrames<T>& FreeVerb<T>::tick( StkFrames<T>& iFrames, StkFrames<T> &oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() - 1 ) {
    oStream_ << "FreeVerb<T>::tick(): channel and StkFrames<T> arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  T *iSamples = &iFrames[iChannel];
  T *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels();
  unsigned int oHop = oFrames.channels();
  bool stereoInput = ( iFrames.channels() > iChannel+1 ) ? true : false;
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop) {
    if ( stereoInput )
      *oSamples = tick( *iSamples, *(iSamples+1) );
    else
      *oSamples = tick( *iSamples );

    *(oSamples+1) = this->lastFrame_[1];
  }

  return oFrames;
}

}