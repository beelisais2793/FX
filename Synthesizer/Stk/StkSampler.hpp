#pragma once
#include "StkInstrmnt.hpp"
#include "StkADSR.hpp"
#include "StkFileLoop.hpp"
#include "StkOnePole.hpp"

namespace stk {

/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument provides an ADSR envelope, a one-pole filter, and
    structures for an arbitrary number of attack and looped files.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Sampler : public Instrmnt<T>
{
 public:
  //! Default constructor.
  Sampler( void );

  //! Class destructor.
  virtual ~Sampler( void );

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency( T frequency ) = 0;

  //! Initiate the envelopes with a key-on event and reset the attack waves.
  void keyOn( void );

  //! Signal a key-off event to the envelopes.
  void keyOff( void );

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange( int number, T value ) = 0;

  //! Compute and return one output sample.
  virtual T tick( unsigned int channel = 0 ) = 0;

  //! Fill a channel of the StkFrames<T> object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames<T> argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  virtual StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 ) = 0;

 protected:

  ADSR<T> adsr_;
  std::vector<FileWvIn<T> *> attacks_;
  std::vector<FileLoop<T> *> loops_;
  OnePole<T> filter_;
  T baseFrequency_;
  std::vector<T> attackRatios_;
  std::vector<T> loopRatios_;
  T attackGain_;
  T loopGain_;

};

/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument provides an ADSR envelope, a one-pole filter, and
    structures for an arbitrary number of attack and looped files.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
Sampler<T>::Sampler( void )
{
  // We don't make the waves here yet, because
  // we don't know what they will be.
  baseFrequency_ = 440.0;
  attackGain_ = 0.25;
  loopGain_ = 0.25;
}  


template<typename T>
Sampler<T>::~Sampler( void )
{
  unsigned int i;
  for ( i=0; i<attacks_.size(); i++ ) delete attacks_[i];
  for ( i=0; i<loops_.size(); i++ ) delete loops_[i];
}


template<typename T>
void Sampler<T>::keyOn( void )
{
  // Reset all attack waves.
  for ( unsigned int i=0; i<attacks_.size(); i++ )
    attacks_[i]->reset();

  // Start the envelope.
  adsr_.keyOn();

}


template<typename T>
void Sampler<T>::keyOff( void )
{
  adsr_.keyOff();
}



template<typename T>
void Sampler<T>::noteOff( T amplitude )
{
  this->keyOff();
}

} // stk namespace
