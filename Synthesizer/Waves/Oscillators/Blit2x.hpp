#pragma once

class Blit
{
 public:
  //! Default constructor that initializes BLIT frequency to 220 Hz.
  Blit( double frequency = 220.0 );

  //! Class destructor.
  ~Blit();

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
  
  //! Compute and return one output sample.
  double Tick( double I = 0, double A = 1, double X = 0, double Y = 0);

 protected:

    double Sawtooth();
    double Square();
    double Triangle();

  void updateHarmonics( void );

  RBJFilter *lowpass;
  Decimateur5 decimator[4];

  unsigned int nHarmonics_;
  unsigned int m_;
  double rate_;
  double phase_;
  double p_;
  double y;
  double saw,square,triangle;
  double duty;
  double freq;
  double C2_;
  double a_;
  double state_;
  double lastBlitOutput_;
  double lastBPBlitOutput_;
  double dcbState_;
  
};

inline double Blit::Sawtooth( void )
{  
  double tmp = lastBlitOutput_;
  tmp += state_ - C2_;
  state_ = tmp * 0.995;  
  return tmp;
}

inline double Blit::Square( void )
{
    

    // A fully  optimized version of this would replace the two sin calls
    // with a pair of fast sin oscillators, for which stable fast 
    // two-multiply algorithms are well known. In the spirit of STK,
    // which favors clarity over performance, the optimization has 
    // not been made here.
    
    // Now apply DC blocker.
    double temp = lastBPBlitOutput_ - dcbState_ + 0.999 * y;
    dcbState_   = lastBPBlitOutput_;    
    return temp;
}

inline double Blit:: Triangle()
{
    double period = (2*sampleRate)/freq;    
    double g = 0.99f / (period *(duty)*(1-duty));
    float out = triangle + g * (square + duty);
    return out;
}

inline double Blit :: Tick(double I, double A, double X, double Y)
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
    double tmp, denominator = sin( phase_ );
    if ( fabs(denominator) <= std::numeric_limits<double>::epsilon() )
        tmp = 1.0;
    else {
        tmp =  sin( m_ * phase_ );
        tmp /= m_ * denominator;
    }

    double old = lastBPBlitOutput_;
    

    if ( fabs( denominator )  < std::numeric_limits<double>::epsilon() ) {

    // Inexact comparison safely distinguishes betwen *close to zero*, and *close to M_PI*.
    if ( phase_ < 0.1f || phase_ > 2*M_PI - 0.1f )
        lastBPBlitOutput_ = a_;
    else
        lastBPBlitOutput_ = -a_;
    }
    else {
        lastBPBlitOutput_ =  sin( m_ * phase_ );
        lastBPBlitOutput_ /= p_ * denominator;
    }

    lastBPBlitOutput_ += old;

    phase_ = fmod(phase_ + rate_, M_PI);
    
    lastBlitOutput_ = tmp;

    saw      = decimator[0].Calc(saw,Sawtooth());
    square   = decimator[1].Calc(square,Square());
    triangle = decimator[2].Calc(triangle,Triangle());
    y = tmp;
    return saw;
}



inline Blit:: Blit( double frequency )
{
  lowpass = new RBJFilter(RBJFilter::FilterType::LOWPASS,15000.0f,2*sampleRate);
  nHarmonics_ = 0;  
  dcbState_ = 0.0;
  lastBlitOutput_ = 0;
  duty = 0.5;  
  this->setFrequency( frequency );
  this->reset();
}

inline Blit :: ~Blit()
{
    if(lowpass) delete lowpass;
}

inline void Blit :: reset()
{
  phase_ = 0.0;
  y = 0.0;
}

inline void Blit :: setFrequency( double frequency )
{

  freq = frequency;
  p_ = (2*sampleRate) / frequency;
  C2_ = 1 / p_;
  rate_ = M_PI / p_;
  this->updateHarmonics();
}

inline void Blit :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;  
  this->updateHarmonics();
  state_ = -0.5 * a_;
}

inline void Blit :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

  a_ = m_ / p_;
}
