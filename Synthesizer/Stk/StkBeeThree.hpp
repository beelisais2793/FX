#include "StkFM.hpp"

namespace stk {

/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class BeeThree : public FM<T>
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BeeThree( void );

  //! Class destructor.
  ~BeeThree( void );

  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

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

};

template<typename T>
inline T BeeThree<T>::tick( unsigned int )
{
  T temp;

  if ( (this->modDepth_) > 0.0 )	{
    temp = 1.0 + ( this->modDepth_ * this->vibrato_.tick() * 0.1 );
    this->waves_[0]->setFrequency( this->baseFrequency_ * temp * this->ratios_[0] );
    this->waves_[1]->setFrequency( this->baseFrequency_ * temp * this->ratios_[1] );
    this->waves_[2]->setFrequency( this->baseFrequency_ * temp * this->ratios_[2] );
    this->waves_[3]->setFrequency( this->baseFrequency_ * temp * this->ratios_[3] );
  }

  this->waves_[3]->addPhaseOffset( this->twozero_.lastOut() );
  temp = this->control1_ * 2.0 * this->gains_[3] * this->adsr_[3]->tick() * this->waves_[3]->tick();
  this->twozero_.tick( temp );

  temp += this->control2_ * 2.0 * this->gains_[2] * this->adsr_[2]->tick() * this->waves_[2]->tick();
  temp += this->gains_[1] * this->adsr_[1]->tick() * this->waves_[1]->tick();
  temp += this->gains_[0] * this->adsr_[0]->tick() * this->waves_[0]->tick();

  this->lastFrame_[0] = temp * 0.125;
  return this->lastFrame_[0];
}

template<typename T>
inline StkFrames<T>& BeeThree<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "BeeThree::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
BeeThree<T>::BeeThree( void )
  : FM<T>()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    this->waves_[i] = new FileLoop<T>( (rawwavePath() + "sinewave.raw").c_str(), true );
  this->waves_[3] = new FileLoop<T>( (rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio( 0, 0.999 );
  this->setRatio( 1, 1.997 );
  this->setRatio( 2, 3.006 );
  this->setRatio( 3, 6.009 );

  this->gains_[0] = this->fmGains_[95];
  this->gains_[1] = this->fmGains_[95];
  this->gains_[2] = this->fmGains_[99];
  this->gains_[3] = this->fmGains_[95];

  this->adsr_[0]->setAllTimes( 0.005, 0.003, 1.0, 0.01 );
  this->adsr_[1]->setAllTimes( 0.005, 0.003, 1.0, 0.01 );
  this->adsr_[2]->setAllTimes( 0.005, 0.003, 1.0, 0.01 );
  this->adsr_[3]->setAllTimes( 0.005, 0.001, 0.4, 0.03 );

  this->twozero_.setGain( 0.1 );
}  

template<typename T>
BeeThree<T>::~BeeThree( void )
{
}

template<typename T>
void BeeThree<T>::noteOn( T frequency, T amplitude )
{
  this->gains_[0] = amplitude * this->fmGains_[95];
  this->gains_[1] = amplitude * this->fmGains_[95];
  this->gains_[2] = amplitude * this->fmGains_[99];
  this->gains_[3] = amplitude * this->fmGains_[95];
  this->setFrequency( frequency );
  this->keyOn();
}

} // stk namespace

