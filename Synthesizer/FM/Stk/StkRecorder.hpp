#pragma once
#include "StkInstrmnt.hpp"
#include "StkIir.hpp"
#include "StkDelayL.hpp"
#include "StkNoise.hpp"
#include "StkSineWave.hpp"
#include "StkADSR.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Recorder
    \brief A recorder / flute physical model.

    This class implements a physical model of a recorder / 
    flute instrument, based on the paper "Sound production
    in recorderlike instruments. II. A simulation model."
    by M.P. Verge, A. Hirschberg and R. Causse, Journal of
    the Acoustical Society of America, 1997.

    Control Change Numbers: 
       - Softness = 2
       - Noise Gain = 4
       - Noise Cutoff = 16
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Mathias Bredholt, McGill University.
    Formatted for STK by Gary Scavone, 2021.
*/
/***************************************************/


template<typename T>
class Recorder : public Instrmnt<T>
{
 public:
  //! Class constructor.
  Recorder( void );

  //! Class destructor.
  ~Recorder( void );

  //! Reset and clear all internal state.
  void clear( void );
  
  //! Set instrument parameters for a particular frequency.
  void setFrequency( T val );

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing( T amplitude, T rate );

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing( T rate );
  
  //! Start a note with the given frequency and amplitude.
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( T amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, T value );

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

  void setBlowPressure( T val );
  void setVibratoGain( T val );
  void setVibratoFrequency( T val );
  void setNoiseGain( T val );
  void setBreathCutoff( T val );
  void setSoftness( T val );

 private:
    DelayL<T> pinDelay_;
    DelayL<T> poutDelay_;
    DelayL<T> jetDelay_;
    Iir<T> radiation_filter_;
    Iir<T> visco_in_filter_;
    Iir<T> visco_out_filter_;
    Iir<T> jetFilter_;
    Noise<T> turb_;
    Iir<T> turbFilter_;
    SineWave<T> vibrato_;
    ADSR<T> adsr_;

    //T M{ 0 };
    //T maxPressure_( 0 );
    double maxPressure_;
    //T blow{ 0 };
    T vibratoGain_;
    T noiseGain_;
    T breathCutoff_;
    T outputGain_;
    T psi_;

    T poutL_;
    T pout_;
    T poutm1_;
    T poutm2_;
    T pin_;
    T pinm1_;
    T pinm2_;

    T b1;
    T b3;
    T b4;

    T Uj_;
    T Ujm1_;

    T Qj_;
    T Qjm1_;
    T Qjm2_;

    T Q1_;
    T Q1m1_;
    T Q1m2_;

    T Qp_;
    T Qpm1_;

    T pm_;
};

template<typename T>
inline StkFrames<T>& Recorder<T> :: tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Recorder<T>::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Recorder
    \brief A recorder / flute physical model.

    This class implements a physical model of a recorder / 
    flute instrument, based on the paper "Sound production
    in recorderlike instruments. II. A simulation model."
    by M.P. Verge, A. Hirschberg and R. Causse, Journal of
    the Acoustical Society of America, 1997.

    Control Change Numbers: 
       - Softness = 2
       - Noise Gain = 4
       - Noise Cutoff = 16
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Mathias Bredholt, McGill University.
    Formatted for STK by Gary Scavone, 2021.
*/
/***************************************************/





// Air constants
const double rho = 1.2041; // density of air
const double c0 = 343.21;  // speed of sound in air

// Flute constants
const double lc = 0.02;     // length of flue canal
const double h = 0.001;     // height of flue exit
const double H = 0.02;      // pipe diameter
const double W = 4 * h;     // width of mouth
const double Sp = H * H;    // cross-section of pipe
const double Sm = W * H;    // cross-section of mouth
const double din = 0.0030;  // end correction
const double dout = 0.0063; // end correction
const double dm = din + dout;  // end correction of mouth
const double dd = 0.0035;   // acoustic distance between Q1 and Q2
const double rp = sqrt(Sp / stk::PI);
const double b = 0.4 * h;   // jet width

// Calculation coefficients
const double b2 = Sp / (rho * c0);

template<typename X>
Recorder<X>::Recorder()
{
  vibratoGain_ = 0.0;
  noiseGain_ = 0.2;
  breathCutoff_ = 0.0;
  outputGain_ = 0.0;
  psi_ = 1.0;
  poutL_ = 0;
  pout_ = 0;
  poutm1_ = 0;
  poutm2_ = 0;
  pin_ = 0;
  pinm1_ = 0;
  pinm2_ = 0;
  Uj_ = 0;
  Ujm1_ = 0;
  Qj_ = 0;
  Qjm1_ = 0;
  Qjm2_ = 0;
  Q1_ = 0;
  Q1m1_ = 0;
  Q1m2_ = 0;
  Qp_ = 0;
  Qpm1_ = 0;
  pm_ = 0;
  
  pinDelay_.tick( 0 );
  poutDelay_.tick( 0 );
  jetDelay_.tick( 0 );

  jetDelay_.setDelay(200);
  vibrato_.setFrequency(4);

  // Calculation coefficients ... would need to be recalculated if sample rate changes
  X T = 1.0 / stk::sampleRate();
  b1 = rho / (4.0 * PI * c0 * T * T);
  b3 = dm * Sp / (T * Sm * c0);
  b4 = rho * dout / (Sm * T);

  // Radiation loss filter
  X A = rp * rp / (4 * c0 * c0 * T * T);
  X B = 0.82 * rp / (c0*T);
  X b_rad[3] = { 1 + A - B, B - 2 * A, A };
  X a_rad[3] = { A - B - 1, B - 2 * A, A };
  std::vector<X> b_coeffs( &b_rad[0], &b_rad[0]+3 );
  std::vector<X> a_coeffs( &a_rad[0], &a_rad[0]+3 );
  radiation_filter_.setCoefficients(b_coeffs, a_coeffs);
  
  // Visco-thermal loss filter
  X b_visco[4] = { 0.83820223947141, -0.16888603248373, -0.64759781930259, 0.07424498608506 };
  X a_visco[4] = { 1.0, -0.33623476246554, -0.71257915055968, 0.14508304017256 };
  b_coeffs.clear();
  b_coeffs.assign( &b_visco[0], &b_visco[0]+4 );
  a_coeffs.clear();
  a_coeffs.assign( &a_visco[0], &a_visco[0]+4 );
  visco_in_filter_.setCoefficients(b_coeffs, a_coeffs);
  visco_out_filter_.setCoefficients(b_coeffs, a_coeffs);

  setBreathCutoff( 500 );
  setFrequency( 880 );
}


template<typename T>
Recorder<T>::~Recorder( void )
{
}


template<typename T>
void Recorder<T>::clear( void )
{
  pinDelay_.clear();
  poutDelay_.clear();
  jetDelay_.clear();
  radiation_filter_.clear();
  visco_in_filter_.clear();
  visco_out_filter_.clear();
  turbFilter_.clear();
}


template<typename T>
void Recorder<T>::setFrequency( T val )
{
#if defined(_STK_DEBUG_)
  if ( val <= 0.0 ) {
    oStream_ << "Recorder<T>::setFrequency: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }
#endif
  
  T M = stk::sampleRate() / val - 4 - 3;
  pinDelay_.setDelay( M );
  poutDelay_.setDelay( M );
}

template<typename T>
void Recorder<T>::setBlowPressure( T val )
{
  maxPressure_ = val;
}

template<typename T>
void Recorder<T>::setVibratoGain( T val )
{
  vibratoGain_ = val;
}

template<typename T>
void Recorder<T>::setVibratoFrequency( T val )
{
  vibrato_.setFrequency( val );
}

template<typename T>
void Recorder<T>::setNoiseGain( T val )
{
  noiseGain_ = val;
}

template<typename T>
void Recorder<T>::setBreathCutoff( T val )
{
  // The gain of this filter is quite high
  breathCutoff_ = val;
  T Q = 0.99;
  T r = 2.0 * sin(PI * val / sampleRate());
  T q = 1.0 - r * Q;
  T as[3] = { 1.0, r * r - q - 1, q };
  std::vector<T> b_turb(1, r*r);
  std::vector<T> a_turb( &as[0], &as[0]+3 );
  turbFilter_.setCoefficients(b_turb, a_turb);
}

template<typename T>
void Recorder<T>::setSoftness( T val )
{
  psi_ = val;
}

template<typename T>
void Recorder<T>::startBlowing( T amplitude, T rate )
{
  if ( amplitude <= 0.0 || rate <= 0.0 ) {
    oStream_ << "Recorder<T>::startBlowing: one or more arguments is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setAttackRate( rate );
  //maxPressure_ = amplitude / (T) 0.8;
  maxPressure_ = 35 * amplitude;
  adsr_.keyOn();
}

template<typename T>
void Recorder<T>::stopBlowing( T rate )
{
  if ( rate <= 0.0 ) {
    oStream_ << "Recorder<T>::stopBlowing: argument is less than or equal to zero!";
    handleError( StkError::WARNING ); return;
  }

  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

template<typename T>
void Recorder<T>::noteOn( T frequency, T amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02 );
  outputGain_ = amplitude / 40.0;
}

template<typename T>
void Recorder<T>::noteOff( T amplitude )
{
  this->stopBlowing( amplitude * 0.02 );
}

template<typename T>
void Recorder<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Recorder<T>::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if (number == 2) // 2
    psi_ = 2.0 * normalizedValue;
  else if (number == 16)
    setBreathCutoff( normalizedValue * 2000 );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = normalizedValue;
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( normalizedValue * 12.0);
  else if (number == __SK_ModWheel_) // 1
    vibratoGain_ = ( normalizedValue * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    maxPressure_ = 35.0 * normalizedValue;
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Recorder<T>::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

template<typename X>
X Recorder<X>::tick( unsigned int )
{
  // Read in from delay lines
  pinm2_ = pinm1_;
  pinm1_ = pin_;
  pin_ = pinDelay_.lastOut();

  poutm2_ = poutm1_;
  poutm1_ = pout_;
  poutL_ = poutDelay_.lastOut();

  // Filter wave components for visco-thermal losses
  pin_ = visco_in_filter_.tick(pin_);
  poutL_ = visco_out_filter_.tick(poutL_);

  // Get input blow pressure
  X pf = maxPressure_ * adsr_.tick() * (vibrato_.tick() * vibratoGain_ + (1 - vibratoGain_));

  X T = 1.0 / sampleRate();
  
  // Jet velocity at flue exit
  Ujm1_ = Uj_;
  Uj_ = Ujm1_ + T / (rho * lc) * (pf - pm_ - 0.5 * rho * Ujm1_ * Ujm1_);

  // Jet flow at flue exit
  Qjm2_ = Qjm1_;
  Qjm1_ = Qj_;
  Qj_ = h * H * Uj_;

  // Jet drive
  X Uj_steady = fmax(sqrt(2 * pf / rho), 0.1);
  X fc_jet = 0.36 / W * Uj_steady;
  X g_jet = 0.002004 * exp(-0.06046 * Uj_steady);
  X r_jet = 0.95 - Uj_steady * 0.015;
  X b0_jet = g_jet * (1 - r_jet * r_jet) / 2;

  // Calculate coefficients for resonant filter
  X b_jet[3] = { b0_jet, 0, -b0_jet };
  X a_jet[3] = { 1, X(-2.0 * r_jet * cos(2 * PI * fc_jet * T)), r_jet * r_jet };
  std::vector<X> b_jetcoeffs( &b_jet[0], &b_jet[0]+3 );
  std::vector<X> a_jetcoeffs( &a_jet[0], &a_jet[0]+3 );
  jetFilter_.setCoefficients( b_jetcoeffs, a_jetcoeffs );
  X eta = jetFilter_.tick(jetDelay_.lastOut());

  // Calculate flow source Q1
  Q1m1_ = Q1_;
  Q1_ = b * H * Uj_ * (1 + tanh(eta / (psi_ * b)));

  // Calculate pressure pulse modeling the jet drive
  X pjd = -rho * dd / Sm * (Q1_ - Q1m1_) / T;

  // Vortex shedding
  int Qp_sign = 0;
  if (Qp_ < 0) Qp_sign = -1;
  else if (Qp_ > 0) Qp_sign = 1;

  X pa = -0.5 * rho * (Qp_ / (0.6 * Sm)) * (Qp_ / (0.6 * Sm)) * Qp_sign;

  // Turbulence
  X pt = turbFilter_.tick(noiseGain_ * turb_.tick() * 0.5 * rho * Uj_ * Uj_);

  // Pressure pulse delta p
  X dp = pjd + pa + pt;

  // Calculate outgoing pressure pout
  pout_ = ((b3 - b1 * b2 - 1) * pin_ +
            (2 * b1 * b2 - b3) * (pinm1_ - poutm1_) +
            b1 * b2 * (poutm2_ - pinm2_) -
            b1 * (Qj_ - 2 * Qjm1_ + Qjm2_) +
            b4 * (Qj_ - Qjm1_) + dp) / (1 - b1 * b2 + b3);

  // Flow in the pipe
  Qpm1_ = Qp_;
  Qp_ = Sp / (rho * c0) * (pout_ - pin_);

  // Mouth pressure
  pm_ = pout_ + pin_ - dp + rho * din / Sm * (Qp_ - Qpm1_)/T;

  // Calculate transverse acoustic velocity
  X Q1d = Q1_ - 0.5 * b * H * Uj_;
  X Vac = 2.0 / PI * Qp_ / Sm - 0.38 * Q1d / Sm;
  jetDelay_.tick(Vac);

  // Calculate new jet delay line length
  //jet_.setDelay(fmin(W / (0.6 * Uj_steady) * sampleRate(), 200.0));
  jetDelay_.setDelay(fmin(W / (0.6 * Uj_steady * T), 200.0));

  // Radiation loss filtering
  X pin_L = radiation_filter_.tick(poutL_);

  // Write to delay lines
  poutDelay_.tick(pout_);
  pinDelay_.tick(pin_L);

  this->lastFrame_[0] = outputGain_ * (pout_ + pin_);
  return this->lastFrame_[0];

  //return (pout_0 + pin_0) * 0.01;
}


} // stk namespace



