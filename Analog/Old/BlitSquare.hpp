#pragma once
#include <cmath>
#include <limits>
#include "SoundObject.hpp"

extern double sampleRate;
extern double invSampleRate;

namespace Analog::Oscillators
{
  class BlitSquare : public OscillatorProcessor
  {
  public:
    //! Default constructor that initializes BLIT frequency to 220 Hz.
    BlitSquare( double frequency = 220.0 );

    //! Class destructor.
    ~BlitSquare();

    //! Resets the oscillator state and phase to 0.
    void reset();

    //! Set the phase of the signal.
    /*!
      Set the phase of the signal, in the range 0 to 1.
    */
    void setPhase( double phase ) { phase_ = M_PI * phase; };

    //! Get the current phase of the signal.
    /*!
      Get the phase of the signal, in the range [0 to 1.0).
    */
    double getPhase() const { return phase_ / M_PI; };

    //! Set the impulse train rate in terms of a frequency in Hz.
    void setFrequency( double frequency );
    void setDuty(double d) { k = d; }
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

    //! Return the last computed output value.
    double lastOut( void ) const { return y; };

    //! Compute and return one output sample.
    double tick( void );

    double Tick(double I=1, double A=1, double X=0, double Y=0) {
        return tick();
      }
  protected:

    void updateHarmonics( void );

    unsigned int nHarmonics_;
    unsigned int m_;
    double rate_;
    double phase_;
    double k;
    double p_;
    double a_;
    double lastBlitOutput_;
    double dcbState_;
    double y;
  };

  inline double BlitSquare :: tick( void )
  {
    double temp = lastBlitOutput_;

    // A fully  optimized version of this would replace the two sin calls
    // with a pair of fast sin oscillators, for which stable fast 
    // two-multiply algorithms are well known. In the spirit of STK,
    // which favors clarity over performance, the optimization has 
    // not been made here.

    // Avoid a divide by zero, or use of a denomralized divisor
    // at the sinc peak, which has a limiting value of 1.0.
    double denominator = sin( phase_ );
    
    if ( fabs( denominator )  < std::numeric_limits<double>::epsilon()) {
      // Inexact comparison safely distinguishes betwen *close to zero*, and *close to M_PI*.
      if ( phase_ < 0.1f || phase_ > 2*M_PI - 0.1f )
        lastBlitOutput_ = a_;
      else
        lastBlitOutput_ = -a_;
    }
    else {
      lastBlitOutput_ =  sin( m_ * phase_ );        
      lastBlitOutput_ /= (p_ * denominator);
      
    }
    
    lastBlitOutput_ += temp;
    
    // Now apply DC blocker.
    y = lastBlitOutput_ - dcbState_ + 0.999 * y;
    dcbState_ = lastBlitOutput_;

    phase_ += rate_;
    if ( phase_ >= 2*M_PI ) phase_ -= 2*M_PI;

    return y;
  }


  BlitSquare:: BlitSquare( double frequency ) : OscillatorProcessor()
  {
    nHarmonics_ = 0;
    this->setFrequency( frequency );
    this->reset();
  }

  BlitSquare :: ~BlitSquare()
  {
  }

  void BlitSquare :: reset()
  {
    phase_ = 0.0;
    y = 0.0;
    dcbState_ = 0.0;
    k = 0.1;
    lastBlitOutput_ = 0;
  }

  void BlitSquare :: setFrequency( double frequency )
  {

    // By using an even value of the parameter M, we get a bipolar blit
    // waveform at half the blit frequency.  Thus, we need to scale the
    // frequency value here by 0.5. (GPS, 2006).
    p_ = 0.5 * sampleRate / frequency;
    rate_ = M_PI / p_;
    this->updateHarmonics();
  }

  void BlitSquare :: setHarmonics( unsigned int nHarmonics )
  {
    nHarmonics_ = nHarmonics;
    this->updateHarmonics();
  }

  void BlitSquare :: updateHarmonics( void )
  {
    // Make sure we end up with an even value of the parameter M here.
    if ( nHarmonics_ <= 0 ) {
      unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
      m_ = 2 * (maxHarmonics + 1);
    }
    else
      m_ = 2 * (nHarmonics_ + 1);

    a_ = m_ / p_;
  }
}
