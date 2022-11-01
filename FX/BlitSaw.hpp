#pragma once


#include <cmath>
#include <limits>
#include "SoundObject.hpp"

extern DspFloatType sampleRate;
extern DspFloatType invSampleRate;

namespace Analog::Oscillators
{
  class BlitSaw : public OscillatorProcessor
  {
  public:
    //! Class constructor.
    BlitSaw( DspFloatType frequency = 220.0 );

    //! Class destructor.
    ~BlitSaw();

    //! Resets the oscillator state and phase to 0.
    void reset();

    //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
    void setFrequency( DspFloatType frequency );

    //! Set the number of harmonics generated in the signal.
    /*!
      This function sets the number of harmonics contained in the
      resulting signal.  It is equivalent to (2 * M) + 1 in the BLIT
      algorithm.  The default value of 0 sets the algorithm for maximum
      harmonic content (harmonics up to half the sample rate).  This
      parameter is not checked against the current sample rate and
      fundamental frequency.  Thus, aliasing can result if one or more
      harmonics for a given fundamental frequency exceeds fs / 2.  This
      behavior was chosen over the potentially more problematic solution
      of automatically modifying the M parameter, which can produce
      audible clicks in the signal.
    */
    void setHarmonics( unsigned int nHarmonics = 0 );

    void setPhaseOffset(DspFloatType o) {
      offset = o;
    }
    //! Return the last computed output value.
    DspFloatType lastOut( void ) const { return y; };

    enum {
        PORT_FREQ,
        PORT_HARMONICS,
        PORT_GAIN,
        PORT_PHASE,
        PORT_RESET,
    };
    void setPort(int port, DspFloatType v) {
        switch(port) {
            case PORT_FREQ: setFrequency(v); break;
            case PORT_HARMONICS: setHarmonics(v); break;
            case PORT_GAIN: setGain(v); break;
            case PORT_PHASE: setPhase(v); break;
            case PORT_RESET: reset(); break;
            default: printf("No port %d\n",port);
        }
    }      
    //! Compute and return one output sample.
    DspFloatType tick( void );

    DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
        return tick();
      }
  protected:

    void updateHarmonics( void );

    unsigned int nHarmonics_;
    unsigned int m_;
    DspFloatType rate_;
    DspFloatType phase_;
    DspFloatType offset;
    DspFloatType p_;
    DspFloatType C2_;
    DspFloatType a_;
    DspFloatType state_;
    DspFloatType y;
  };

  inline DspFloatType BlitSaw :: tick( void )
  {  
    //DspFloatType x = phase_;
    //phase_ += offset;
    DspFloatType tmp, denominator = sin( phase_ );
    if ( fabs(denominator) <= std::numeric_limits<DspFloatType>::epsilon() )
      tmp = a_;
    else {
      tmp =  sin( m_ * phase_ );
      tmp /= p_ * denominator;
    }

    tmp += state_ - C2_;
    state_ = tmp * 0.995;
    //phase_   = x;
    phase_ += rate_;
    if ( phase_ >= M_PI ) phase_ -= M_PI;
      
    y = tmp;
    return y;
  }


  inline BlitSaw:: BlitSaw( DspFloatType frequency ) : OscillatorProcessor()
  {
    nHarmonics_ = 0;
    offset = 0;
    this->reset();
    this->setFrequency( frequency );
  }

  inline BlitSaw :: ~BlitSaw()
  {
  }

  inline void BlitSaw :: reset()
  {
    phase_ = 0.0f;
    state_ = 0.0;
    y = 0.0;
  }

  inline void BlitSaw :: setFrequency( DspFloatType frequency )
  {
    p_ = sampleRate / frequency;
    C2_ = 1 / p_;
    rate_ = M_PI * C2_;
    this->updateHarmonics();
  }

  inline void BlitSaw :: setHarmonics( unsigned int nHarmonics )
  {
    nHarmonics_ = nHarmonics;
    this->updateHarmonics();

    // I found that the initial DC offset could be minimized with an
    // initial state setting as given below.  This initialization should
    // only happen before starting the oscillator for the first time
    // (but after setting the frequency and number of harmonics).  I
    // struggled a bit to decide where best to put this and finally
    // settled on here.  In general, the user shouldn't be messing with
    // the number of harmonics once the oscillator is running because
    // this is automatically taken care of in the setFrequency()
    // function.  (GPS - 1 October 2005)
    state_ = -0.5 * a_;
  }

  inline void BlitSaw :: updateHarmonics( void )
  {
    if ( nHarmonics_ <= 0 ) {
      unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
      m_ = 2 * maxHarmonics + 1;
    }
    else
      m_ = 2 * nHarmonics_ + 1;

    a_ = m_ / p_;
  }

}