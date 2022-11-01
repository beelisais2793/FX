#pragma once

#include <cmath>
#include <limits>
#include "SoundObject.hpp"

extern DspFloatType sampleRate;
extern DspFloatType invSampleRate;

namespace Analog::Oscillators
{
    class Blit : public OscillatorProcessor
    {
    public:
      //! Default constructor that initializes BLIT frequency to 220 Hz.
      Blit( DspFloatType frequency = 220.0 );

      //! Class destructor.
      ~Blit();

      //! Resets the oscillator state and phase to 0.
      void reset();

      //! Set the phase of the signal.
      /*!
        Set the phase of the signal, in the range 0 to 1.
      */
      void setPhase( DspFloatType phase ) { phase_ = M_PI * phase; };

      //! Get the current phase of the signal.
      /*!
        Get the phase of the signal, in the range [0 to 1.0).
      */
      DspFloatType getPhase() const { return phase_ / M_PI; };

      //! Set the impulse train rate in terms of a frequency in Hz.
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
      DspFloatType p_;
      
      DspFloatType y;

    };

    inline DspFloatType Blit :: tick( void )
    {
      // The code below implements the SincM algorithm of Stilson and
      // Smith with an additional scale factor of P / M applied to
      // normalize the output.

      // A fully optimized version of this code would replace the two sin
      // calls with a pair of fast sin oscillators, for which stable fast
      // two-multiply algorithms are well known. In the spirit of STK,
      // which favors clarity over performance, the optimization has not
      // been made here.

      // Avoid a divide by zero at the sinc peak, which has a limiting
      // value of 1.0.
      DspFloatType tmp, denominator = sin( phase_ );
      if ( denominator <= std::numeric_limits<DspFloatType>::epsilon() )
        tmp = 1.0;
      else {
        tmp =  sin( m_ * phase_ );
        tmp /= m_ * denominator;
      }

      phase_ += rate_;
      if ( phase_ >= M_PI ) phase_ -= M_PI;

      y = tmp;
      return y;
    }


    inline Blit:: Blit( DspFloatType frequency ) : OscillatorProcessor()
    {
      nHarmonics_ = 0;
      this->setFrequency( frequency );
      this->reset();
    }

    inline Blit :: ~Blit()
    {
    }

    inline void Blit :: reset()
    {
      phase_ = 0.0;
      y = 0.0;
    }

    inline void Blit :: setFrequency( DspFloatType frequency )
    {

      p_ = sampleRate / frequency;
      rate_ = M_PI / p_;
      this->updateHarmonics();
    }

    inline void Blit :: setHarmonics( unsigned int nHarmonics )
    {
      nHarmonics_ = nHarmonics;
      this->updateHarmonics();
    }

    inline void Blit :: updateHarmonics( void )
    {
      if ( nHarmonics_ <= 0 ) {
        unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
        m_ = 2 * maxHarmonics + 1;
      }
      else
        m_ = 2 * nHarmonics_ + 1;
    }

}