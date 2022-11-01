#pragma once
#include "StkInstrmnt.hpp"
#include <cmath>
#include <stdlib.h>
#include "SKINImsg.h"


namespace stk {

/***************************************************/
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event Modeling) is an
    algorithmic approach for simulating collisions of multiple
    independent sound producing objects.  This class is a meta-model
    that can simulate a Maraca, Sekere, Cabasa, Bamboo Wind Chimes,
    Water Drops, Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of Imitated Environmental
    Sounds) is a similar approach for the synthesis of environmental
    sounds.  This class implements simulations of breaking sticks,
    crunchy snow (or not), a wrench, sandpaper, and more.

    Control Change Numbers: 
    - Shake Energy = 2
    - System Decay = 4
    - Number Of Objects = 11
    - Resonance Frequency = 1
    - Shake Energy = 128
    - Instrument Selection = 1071
    - Maraca = 0
    - Cabasa = 1
    - Sekere = 2
    - Tambourine = 3
    - Sleigh Bells = 4
    - Bamboo Chimes = 5
    - Sand Paper = 6
    - Coke Can = 7
    - Sticks = 8
    - Crunch = 9
    - Big Rocks = 10
    - Little Rocks = 11
    - Next Mug = 12
    - Penny + Mug = 13
    - Nickle + Mug = 14
    - Dime + Mug = 15
    - Quarter + Mug = 16
    - Franc + Mug = 17
    - Peso + Mug = 18
    - Guiro = 19
    - Wrench = 20
    - Water Drops = 21
    - Tuned Bamboo Chimes = 22

    by Perry R. Cook with updates by Gary Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class Shakers : public Instrmnt<T>
{
 public:
  //! Class constructor taking instrument type argument.
  Shakers( int type = 0 );

  //! Start a note with the given instrument and amplitude.
  /*!
    Use the instrument numbers above, converted to frequency values
    as if MIDI note numbers, to select a particular instrument.
  */
  void noteOn( T instrument, T amplitude );

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

  struct BiQuad {
    T gain;
    T b[3];
    T a[3]; // a0 term assumed equal to 1.0
    T inputs[3];
    T outputs[3];

    // Default constructor.
    BiQuad()
    {
      gain = 0.0;
      for ( int i=0; i<3; i++ ) {
        b[i] = 0.0;
        a[i] = 0.0;
        inputs[i] = 0.0;
        outputs[i] = 0.0;
      }
    }
  };

 protected:

  void setType( int type );
  void setResonance( BiQuad &filter, T frequency, T radius );
  T tickResonance( BiQuad &filter, T input );
  void setEqualization( T b0, T b1, T b2 );
  T tickEqualize( T input );
  int randomInt( int max );
  T randomFloat( T max = 1.0 );
  T noise( void );
  void waterDrop( void );

  int shakerType_;
  unsigned int nResonances_;
  T shakeEnergy_;
  T soundDecay_;
  T systemDecay_;
  T nObjects_;
  T sndLevel_;
  T baseGain_;
  T currentGain_;
  T baseDecay_;
  T baseObjects_;
  T decayScale_;
  BiQuad equalizer_;
  T ratchetCount_;
  T ratchetDelta_;
  T baseRatchetDelta_;
  int lastRatchetValue_;

  std::vector< BiQuad > filters_;
  std::vector< T > baseFrequencies_;
  std::vector< T > baseRadii_;
  std::vector< bool > doVaryFrequency_;
  std::vector< T > tempFrequencies_;
  T varyFactor_;
};


template<typename T>
inline void Shakers<T>::setResonance( BiQuad &filter, T frequency, T radius )
{
  filter.a[1] = -2.0 * radius * cos( TWO_PI * frequency / stk::sampleRate());
  filter.a[2] = radius * radius;
}


template<typename T>
inline T Shakers<T>::tickResonance( BiQuad &filter, T input )
{
  filter.outputs[0] = input * filter.gain * currentGain_;
  filter.outputs[0] -= filter.a[1] * filter.outputs[1] + filter.a[2] * filter.outputs[2];
  filter.outputs[2] = filter.outputs[1];
  filter.outputs[1] = filter.outputs[0];
  return filter.outputs[0];
}


template<typename T>
inline void Shakers<T>::setEqualization( T b0, T b1, T b2 )
{
  equalizer_.b[0] = b0;
  equalizer_.b[1] = b1;
  equalizer_.b[2] = b2;
}


template<typename T>
inline T Shakers<T>::tickEqualize( T input )
{
  equalizer_.inputs[0] = input;
  equalizer_.outputs[0] = equalizer_.b[0] * equalizer_.inputs[0] + equalizer_.b[1] * equalizer_.inputs[1] + equalizer_.b[2] * equalizer_.inputs[2];
  equalizer_.inputs[2] = equalizer_.inputs[1];
  equalizer_.inputs[1] = equalizer_.inputs[0];
  return equalizer_.outputs[0];
}


template<typename T>
inline int Shakers<T>::randomInt( int max ) //  Return random integer between 0 and max-1
{
  return (int) ((float)max * rand() / (RAND_MAX + 1.0) );
}


template<typename T>
inline T Shakers<T>::randomFloat( T max ) // Return random float between 0.0 and max
{	
  return (T) (max * rand() / (RAND_MAX + 1.0) );
}


template<typename T>
inline T Shakers<T>::noise( void ) //  Return random T float between -1.0 and 1.0
{
  return ( (T) ( 2.0 * rand() / (RAND_MAX + 1.0) ) - 1.0 );
}

const double MIN_ENERGY = 0.001;
const double WATER_FREQ_SWEEP = 1.0001;


template<typename T>
inline void Shakers<T>::waterDrop( void )
{
  if ( randomInt( 32767 ) < nObjects_) {
    sndLevel_ = shakeEnergy_;   
    unsigned int j = randomInt( 3 );
    if ( j == 0 && filters_[0].gain == 0.0 ) { // don't change unless fully decayed
      tempFrequencies_[0] = baseFrequencies_[1] * (0.75 + (0.25 * noise()));
      filters_[0].gain = fabs( noise() );
    }
    else if (j == 1 && filters_[1].gain == 0.0) {
      tempFrequencies_[1] = baseFrequencies_[1] * (1.0 + (0.25 * noise()));
      filters_[1].gain = fabs( noise() );
    }
    else if ( filters_[2].gain == 0.0 ) {
      tempFrequencies_[2] = baseFrequencies_[1] * (1.25 + (0.25 * noise()));
      filters_[2].gain = fabs( noise() );
    }
  }

  // Sweep center frequencies.
  for ( unsigned int i=0; i<3; i++ ) { // WATER_RESONANCES = 3
    filters_[i].gain *= baseRadii_[i];
    if ( filters_[i].gain > 0.001 ) {
      tempFrequencies_[i] *= WATER_FREQ_SWEEP;
      filters_[i].a[1] = -2.0 * baseRadii_[i] * cos( TWO_PI * tempFrequencies_[i] / stk::sampleRate() );
    }
    else
      filters_[i].gain = 0.0;
  }
}

template<typename T>
inline T Shakers<T>::tick( unsigned int )
{
  unsigned int iTube = 0;
  T input = 0.0;
  if ( shakerType_ == 19 || shakerType_ == 20 ) {
    if ( ratchetCount_ <= 0 ) return this->lastFrame_[0] = 0.0;

    shakeEnergy_ -= ( ratchetDelta_ + ( 0.002 * shakeEnergy_ ) );
    if ( shakeEnergy_ < 0.0 ) {
      shakeEnergy_ = 1.0;
      ratchetCount_--;
    }

    if ( randomFloat( 1024 ) < nObjects_ )
      sndLevel_ += shakeEnergy_ * shakeEnergy_;

    // Sound is enveloped noise
    input = sndLevel_ * noise() * shakeEnergy_;
  }
  else { 
    if ( shakeEnergy_ < MIN_ENERGY ) return this->lastFrame_[0] = 0.0;

    // Exponential system decay
    shakeEnergy_ *= systemDecay_;

    // Random events
    if ( shakerType_ == 21 ) {
      waterDrop();
      input = sndLevel_;
    }
    else {
      if ( randomFloat( 1024.0 ) < nObjects_ ) {
        sndLevel_ += shakeEnergy_;
        input = sndLevel_;
        // Vary resonance frequencies if specified.
        for ( unsigned int i=0; i<nResonances_; i++ ) {
          if ( doVaryFrequency_[i] ) {
            T tempRand = baseFrequencies_[i] * ( 1.0 + ( varyFactor_ * noise() ) );
            filters_[i].a[1] = -2.0 * baseRadii_[i] * cos( TWO_PI * tempRand / stk::sampleRate() );
          }
        }
        if ( shakerType_ == 22 ) iTube = randomInt( 7 ); // ANGKLUNG_RESONANCES
      }
    }
  }

  // Exponential sound decay
  sndLevel_ *= soundDecay_;

  // Do resonance filtering
  this->lastFrame_[0] = 0.0;
  if ( shakerType_ == 22 ) {
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      if ( i == iTube )
        this->lastFrame_[0] += tickResonance( filters_[i], input );
      else
        this->lastFrame_[0] += tickResonance( filters_[i], 0.0 );
    }
  }
  else {
    for ( unsigned int i=0; i<nResonances_; i++ )
      this->lastFrame_[0] += tickResonance( filters_[i], input );
  }

  // Do final FIR filtering (lowpass or highpass)
  this->lastFrame_[0] = tickEqualize( this->lastFrame_[0] );

  //if ( std::abs(this->lastFrame_[0]) > 1.0 )
  //  std::cout << "lastOutput = " << this->lastFrame_[0] << std::endl;

  return this->lastFrame_[0];
}


template<typename T>
inline StkFrames<T>& Shakers<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Shakers::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event Modeling) is an
    algorithmic approach for simulating collisions of multiple
    independent sound producing objects.  This class is a meta-model
    that can simulate a Maraca, Sekere, Cabasa, Bamboo Wind Chimes,
    Water Drops, Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of Imitated Environmental
    Sounds) is a similar approach for the synthesis of environmental
    sounds.  This class implements simulations of breaking sticks,
    crunchy snow (or not), a wrench, sandpaper, and more.

    Control Change Numbers: 
      - Shake Energy = 2
      - System Decay = 4
      - Number Of Objects = 11
      - Resonance Frequency = 1
      - Shake Energy = 128
      - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Tambourine = 3
        - Sleigh Bells = 4
        - Bamboo Chimes = 5
        - Sand Paper = 6
        - Coke Can = 7
        - Sticks = 8
        - Crunch = 9
        - Big Rocks = 10
        - Little Rocks = 11
        - Next Mug = 12
        - Penny + Mug = 13
        - Nickle + Mug = 14
        - Dime + Mug = 15
        - Quarter + Mug = 16
        - Franc + Mug = 17
        - Peso + Mug = 18
        - Guiro = 19
        - Wrench = 20
        - Water Drops = 21
        - Tuned Bamboo Chimes = 22

    by Perry R. Cook with updates by Gary Scavone, 1995--2021.
*/
/***************************************************/



// Maraca
const double MARACA_SOUND_DECAY = 0.95;
const double MARACA_SYSTEM_DECAY = 0.999;
const double MARACA_GAIN = 4.0;
const double MARACA_NUM_BEANS = 25;
const int MARACA_RESONANCES = 1;
const double MARACA_FREQUENCIES[MARACA_RESONANCES] = { 3200 };
const double MARACA_RADII[MARACA_RESONANCES] = { 0.96 };
const double MARACA_GAINS[MARACA_RESONANCES] = { 1.0 };

// Cabasa
const double CABASA_SOUND_DECAY = 0.96;
const double CABASA_SYSTEM_DECAY = 0.997;
const double CABASA_GAIN = 8.0;
const double CABASA_NUM_BEADS = 512;
const int CABASA_RESONANCES = 1;
const double CABASA_FREQUENCIES[CABASA_RESONANCES] = { 3000 };
const double CABASA_RADII[CABASA_RESONANCES] = { 0.7 };
const double CABASA_GAINS[CABASA_RESONANCES] = { 1.0 };

// Sekere
const double SEKERE_SOUND_DECAY = 0.96;
const double SEKERE_SYSTEM_DECAY = 0.999;
const double SEKERE_GAIN = 4.0;
const double SEKERE_NUM_BEANS = 64;
const int SEKERE_RESONANCES = 1;
const double SEKERE_FREQUENCIES[SEKERE_RESONANCES] = { 5500 };
const double SEKERE_RADII[SEKERE_RESONANCES] = { 0.6 };
const double SEKERE_GAINS[SEKERE_RESONANCES] = { 1.0 };

// Bamboo Wind Chimes
const double BAMBOO_SOUND_DECAY = 0.9;
const double BAMBOO_SYSTEM_DECAY = 0.9999;
const double BAMBOO_GAIN = 0.4;
const double BAMBOO_NUM_TUBES = 1.2;
const int BAMBOO_RESONANCES = 3;
const double BAMBOO_FREQUENCIES[BAMBOO_RESONANCES] = { 2800, 0.8 * 2800.0, 1.2 * 2800.0 };
const double BAMBOO_RADII[BAMBOO_RESONANCES] = { 0.995, 0.995, 0.995 };
const double BAMBOO_GAINS[BAMBOO_RESONANCES] = { 1.0, 1.0, 1.0 };

// doubleambourine
const double TAMBOURINE_SOUND_DECAY = 0.95;
const double TAMBOURINE_SYSTEM_DECAY = 0.9985;
const double TAMBOURINE_GAIN = 1.0;
const double TAMBOURINE_NUM_TIMBRELS = 32;
const int TAMBOURINE_RESONANCES = 3; // Fixed shell + 2 moving cymbal resonances
const double TAMBOURINE_FREQUENCIES[TAMBOURINE_RESONANCES] = { 2300, 5600, 8100 };
const double TAMBOURINE_RADII[TAMBOURINE_RESONANCES] = { 0.96, 0.99, 0.99 };
const double TAMBOURINE_GAINS[TAMBOURINE_RESONANCES] = { 0.1, 0.8, 1.0 };

// Sleighbells
const double SLEIGH_SOUND_DECAY = 0.97;
const double SLEIGH_SYSTEM_DECAY = 0.9994;
const double SLEIGH_GAIN = 1.0;
const double SLEIGH_NUM_BELLS = 32;
const int SLEIGH_RESONANCES = 5;
const double SLEIGH_FREQUENCIES[SLEIGH_RESONANCES] = { 2500, 5300, 6500, 8300, 9800 };
const double SLEIGH_RADII[SLEIGH_RESONANCES] = { 0.99, 0.99, 0.99, 0.99, 0.99 };
const double SLEIGH_GAINS[SLEIGH_RESONANCES] = { 1.0, 1.0, 1.0, 0.5, 0.3 };

// Sandpaper
const double SANDPAPER_SOUND_DECAY = 0.999;
const double SANDPAPER_SYSTEM_DECAY = 0.999;
const double SANDPAPER_GAIN = 0.5;
const double SANDPAPER_NUM_GRAINS = 128;
const int SANDPAPER_RESONANCES = 1;
const double SANDPAPER_FREQUENCIES[SANDPAPER_RESONANCES] = { 4500 };
const double SANDPAPER_RADII[SANDPAPER_RESONANCES] = { 0.6 };
const double SANDPAPER_GAINS[SANDPAPER_RESONANCES] = { 1.0 };

// Cokecan
const double COKECAN_SOUND_DECAY = 0.97;
const double COKECAN_SYSTEM_DECAY = 0.999;
const double COKECAN_GAIN = 0.5;
const double COKECAN_NUM_PARTS = 48;
const int COKECAN_RESONANCES = 5; // Helmholtz + 4 metal resonances
const double COKECAN_FREQUENCIES[COKECAN_RESONANCES] = { 370, 1025, 1424, 2149, 3596 };
const double COKECAN_RADII[COKECAN_RESONANCES] = { 0.99, 0.992, 0.992, 0.992, 0.992 };
const double COKECAN_GAINS[COKECAN_RESONANCES] = { 1.0, 1.8, 1.8, 1.8, 1.8 };

// doubleuned Bamboo Wind Chimes (Angklung)
const double ANGKLUNG_SOUND_DECAY = 0.95;
const double ANGKLUNG_SYSTEM_DECAY = 0.9999;
const double ANGKLUNG_GAIN = 0.5;
const double ANGKLUNG_NUM_TUBES = 1.2;
const int ANGKLUNG_RESONANCES = 7;
const double ANGKLUNG_FREQUENCIES[ANGKLUNG_RESONANCES] = { 1046.6, 1174.8, 1397.0, 1568, 1760, 2093.3, 2350 };
const double ANGKLUNG_RADII[ANGKLUNG_RESONANCES] = { 0.996, 0.996, 0.996, 0.996, 0.996, 0.996, 0.996 };
const double ANGKLUNG_GAINS[ANGKLUNG_RESONANCES] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

// Guiro
const double GUIRO_SOUND_DECAY = 0.95;
const double GUIRO_GAIN = 0.4;
const double GUIRO_NUM_PARTS = 128;
const int GUIRO_RESONANCES = 2;
const double GUIRO_FREQUENCIES[GUIRO_RESONANCES] = { 2500, 4000 };
const double GUIRO_RADII[GUIRO_RESONANCES] = { 0.97, 0.97 };
const double GUIRO_GAINS[GUIRO_RESONANCES] = { 1.0, 1.0 };

// Wrench
const double WRENCH_SOUND_DECAY = 0.95;
const double WRENCH_GAIN = 0.4;
const double WRENCH_NUM_PARTS = 128;
const int WRENCH_RESONANCES = 2;
const double WRENCH_FREQUENCIES[WRENCH_RESONANCES] = { 3200, 8000 };
const double WRENCH_RADII[WRENCH_RESONANCES] = { 0.99, 0.992 };
const double WRENCH_GAINS[WRENCH_RESONANCES] = { 1.0, 1.0 };

// Water Drops
const double WATER_SOUND_DECAY = 0.95;
const double WATER_SYSTEM_DECAY = 0.996;
const double WATER_GAIN = 1.0;
const double WATER_NUM_SOURCES = 10;
const int WATER_RESONANCES = 3;
const double WATER_FREQUENCIES[WATER_RESONANCES] = { 450, 600, 750 };
const double WATER_RADII[WATER_RESONANCES] = { 0.9985, 0.9985, 0.9985 };
const double WATER_GAINS[WATER_RESONANCES] = { 1.0, 1.0, 1.0 };

// PhOLIES (Physically-Oriented Library of Imitated Environmental
// Sounds), Perry Cook, 1997-8

// Stix1
const double STIX1_SOUND_DECAY = 0.96;
const double STIX1_SYSTEM_DECAY = 0.998;
const double STIX1_GAIN = 6.0;
const double STIX1_NUM_BEANS = 2;
const int STIX1_RESONANCES = 1;
const double STIX1_FREQUENCIES[STIX1_RESONANCES] = { 5500 };
const double STIX1_RADII[STIX1_RESONANCES] = { 0.6 };
const double STIX1_GAINS[STIX1_RESONANCES] = { 1.0 };

// Crunch1
const double CRUNCH1_SOUND_DECAY = 0.95;
const double CRUNCH1_SYSTEM_DECAY = 0.99806;
const double CRUNCH1_GAIN = 4.0;
const double CRUNCH1_NUM_BEADS = 7;
const int CRUNCH1_RESONANCES = 1;
const double CRUNCH1_FREQUENCIES[CRUNCH1_RESONANCES] = { 800 };
const double CRUNCH1_RADII[CRUNCH1_RESONANCES] = { 0.95 };
const double CRUNCH1_GAINS[CRUNCH1_RESONANCES] = { 1.0 };

// Nextmug + Coins
const double NEXTMUG_SOUND_DECAY = 0.97;
const double NEXTMUG_SYSTEM_DECAY = 0.9995;
const double NEXTMUG_GAIN = 0.8;
const double NEXTMUG_NUM_PARTS = 3;
const int NEXTMUG_RESONANCES = 4;
const double NEXTMUG_FREQUENCIES[NEXTMUG_RESONANCES] = { 2123, 4518, 8856, 10753 };
const double NEXTMUG_RADII[NEXTMUG_RESONANCES] = { 0.997, 0.997, 0.997, 0.997 };
const double NEXTMUG_GAINS[NEXTMUG_RESONANCES] = { 1.0, 0.8, 0.6, 0.4 };

const int COIN_RESONANCES = 3;
const double PENNY_FREQUENCIES[COIN_RESONANCES] = { 11000, 5200, 3835 };
const double PENNY_RADII[COIN_RESONANCES] = { 0.999, 0.999, 0.999 };
const double PENNY_GAINS[COIN_RESONANCES] = { 1.0, 0.8, 0.5 };

const double NICKEL_FREQUENCIES[COIN_RESONANCES] = { 5583, 9255, 9805 };
const double NICKEL_RADII[COIN_RESONANCES] = { 0.9992, 0.9992, 0.9992 };
const double NICKEL_GAINS[COIN_RESONANCES] = { 1.0, 0.8, 0.5 };

const double DIME_FREQUENCIES[COIN_RESONANCES] = { 4450, 4974, 9945 };
const double DIME_RADII[COIN_RESONANCES] = { 0.9993, 0.9993, 0.9993 };
const double DIME_GAINS[COIN_RESONANCES] = { 1.0, 0.8, 0.5 };

const double QUARTER_FREQUENCIES[COIN_RESONANCES] = { 1708, 8863, 9045 };
const double QUARTER_RADII[COIN_RESONANCES] = { 0.9995, 0.9995, 0.9995 };
const double QUARTER_GAINS[COIN_RESONANCES] = { 1.0, 0.8, 0.5 };

const double FRANC_FREQUENCIES[COIN_RESONANCES] = { 5583, 11010, 1917 };
const double FRANC_RADII[COIN_RESONANCES] = { 0.9995, 0.9995, 0.9995 };
const double FRANC_GAINS[COIN_RESONANCES] = { 0.7, 0.4, 0.3 };

const double PESO_FREQUENCIES[COIN_RESONANCES] = { 7250, 8150, 10060 };
const double PESO_RADII[COIN_RESONANCES] = { 0.9996, 0.9996, 0.9996 };
const double PESO_GAINS[COIN_RESONANCES] = { 1.0, 1.2, 0.7 };

// Big Gravel
const double BIGROCKS_SOUND_DECAY = 0.98;
const double BIGROCKS_SYSTEM_DECAY = 0.9965;
const double BIGROCKS_GAIN = 4.0;
const double BIGROCKS_NUM_PARTS = 23;
const int BIGROCKS_RESONANCES = 1;
const double BIGROCKS_FREQUENCIES[BIGROCKS_RESONANCES] = { 6460 };
const double BIGROCKS_RADII[BIGROCKS_RESONANCES] = { 0.932 };
const double BIGROCKS_GAINS[BIGROCKS_RESONANCES] = { 1.0 };

// Little Gravel
const double LITTLEROCKS_SOUND_DECAY = 0.98;
const double LITTLEROCKS_SYSTEM_DECAY = 0.99586;
const double LITTLEROCKS_GAIN = 4.0;
const double LITTLEROCKS_NUM_PARTS = 1600;
const int LITTLEROCKS_RESONANCES = 1;
const double LITTLEROCKS_FREQUENCIES[LITTLEROCKS_RESONANCES] = { 9000 };
const double LITTLEROCKS_RADII[LITTLEROCKS_RESONANCES] = { 0.843 };
const double LITTLEROCKS_GAINS[LITTLEROCKS_RESONANCES] = { 1.0 };


template<typename T>
Shakers<T>::Shakers( int type )
{
  shakerType_ = -1;
  this->setType( type );
}

template<typename T>
void Shakers<T>::setType( int type )
{
  if ( shakerType_ == type ) return;
  varyFactor_ = 0.0;
  shakerType_ = type;
  if ( type == 1 ) { // Cabasa
    nResonances_ = CABASA_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = CABASA_NUM_BEADS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = CABASA_RADII[i];
      baseFrequencies_[i] = CABASA_FREQUENCIES[i];
      filters_[i].gain = CABASA_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = CABASA_SYSTEM_DECAY;
    baseGain_ = CABASA_GAIN;
    soundDecay_ = CABASA_SOUND_DECAY;
    decayScale_ = 0.97;
    setEqualization( 1.0, -1.0, 0.0 );
  }
  else if ( type == 2 ) { // Sekere
    nResonances_ = SEKERE_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = SEKERE_NUM_BEANS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = SEKERE_RADII[i];
      baseFrequencies_[i] = SEKERE_FREQUENCIES[i];
      filters_[i].gain = SEKERE_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = SEKERE_SYSTEM_DECAY;
    baseGain_ = SEKERE_GAIN;
    soundDecay_ = SEKERE_SOUND_DECAY;
    decayScale_ = 0.94;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 3 ) { // Tambourine
    nResonances_ = TAMBOURINE_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = TAMBOURINE_NUM_TIMBRELS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = TAMBOURINE_RADII[i];
      baseFrequencies_[i] = TAMBOURINE_FREQUENCIES[i];
      filters_[i].gain = TAMBOURINE_GAINS[i];
      doVaryFrequency_[i] = true;
    }
    doVaryFrequency_[0] = false;
    baseDecay_ = TAMBOURINE_SYSTEM_DECAY;
    baseGain_ = TAMBOURINE_GAIN;
    soundDecay_ = TAMBOURINE_SOUND_DECAY;
    decayScale_ = 0.95;
    varyFactor_ = 0.05;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 4 ) { // Sleighbells
    nResonances_ = SLEIGH_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = SLEIGH_NUM_BELLS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = SLEIGH_RADII[i];
      baseFrequencies_[i] = SLEIGH_FREQUENCIES[i];
      filters_[i].gain = SLEIGH_GAINS[i];
      doVaryFrequency_[i] = true;
    }
    baseDecay_ = SLEIGH_SYSTEM_DECAY;
    baseGain_ = SLEIGH_GAIN;
    soundDecay_ = SLEIGH_SOUND_DECAY;
    decayScale_ = 0.9;
    varyFactor_ = 0.03;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 5 ) { // Bamboo chimes
    nResonances_ = BAMBOO_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = BAMBOO_NUM_TUBES;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = BAMBOO_RADII[i];
      baseFrequencies_[i] = BAMBOO_FREQUENCIES[i];
      filters_[i].gain = BAMBOO_GAINS[i];
      doVaryFrequency_[i] = true;
    }
    baseDecay_ = BAMBOO_SYSTEM_DECAY;
    baseGain_ = BAMBOO_GAIN;
    soundDecay_ = BAMBOO_SOUND_DECAY;
    decayScale_ = 0.7;
    varyFactor_ = 0.2;
    setEqualization( 1.0, 0.0, 0.0 );
  }
  else if ( type == 6 ) { // Sandpaper
    nResonances_ = SANDPAPER_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = SANDPAPER_NUM_GRAINS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = SANDPAPER_RADII[i];
      baseFrequencies_[i] = SANDPAPER_FREQUENCIES[i];
      filters_[i].gain = SANDPAPER_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = SANDPAPER_SYSTEM_DECAY;
    baseGain_ = SANDPAPER_GAIN;
    soundDecay_ = SANDPAPER_SOUND_DECAY;
    decayScale_ = 0.97;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 7 ) { // Cokecan
    nResonances_ = COKECAN_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = COKECAN_NUM_PARTS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = COKECAN_RADII[i];
      baseFrequencies_[i] = COKECAN_FREQUENCIES[i];
      filters_[i].gain = COKECAN_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = COKECAN_SYSTEM_DECAY;
    baseGain_ = COKECAN_GAIN;
    soundDecay_ = COKECAN_SOUND_DECAY;
    decayScale_ = 0.95;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 8 ) { // Stix1
    nResonances_ = STIX1_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = STIX1_NUM_BEANS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = STIX1_RADII[i];
      baseFrequencies_[i] = STIX1_FREQUENCIES[i];
      filters_[i].gain = STIX1_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = STIX1_SYSTEM_DECAY;
    baseGain_ = STIX1_GAIN;
    soundDecay_ = STIX1_SOUND_DECAY;
    decayScale_ = 0.96;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 9 ) { // Crunch1
    nResonances_ = CRUNCH1_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = CRUNCH1_NUM_BEADS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = CRUNCH1_RADII[i];
      baseFrequencies_[i] = CRUNCH1_FREQUENCIES[i];
      filters_[i].gain = CRUNCH1_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = CRUNCH1_SYSTEM_DECAY;
    baseGain_ = CRUNCH1_GAIN;
    soundDecay_ = CRUNCH1_SOUND_DECAY;
    decayScale_ = 0.96;
    setEqualization( 1.0, -1.0, 0.0 );
  }
  else if ( type == 10 ) { // Big Rocks
    nResonances_ = BIGROCKS_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = BIGROCKS_NUM_PARTS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = BIGROCKS_RADII[i];
      baseFrequencies_[i] = BIGROCKS_FREQUENCIES[i];
      filters_[i].gain = BIGROCKS_GAINS[i];
      doVaryFrequency_[i] = true;
    }
    baseDecay_ = BIGROCKS_SYSTEM_DECAY;
    baseGain_ = BIGROCKS_GAIN;
    soundDecay_ = BIGROCKS_SOUND_DECAY;
    decayScale_ = 0.95;
    varyFactor_ = 0.11;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 11 ) { // Little Rocks
    nResonances_ = LITTLEROCKS_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = LITTLEROCKS_NUM_PARTS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = LITTLEROCKS_RADII[i];
      baseFrequencies_[i] = LITTLEROCKS_FREQUENCIES[i];
      filters_[i].gain = LITTLEROCKS_GAINS[i];
      doVaryFrequency_[i] = true;
    }
    baseDecay_ = LITTLEROCKS_SYSTEM_DECAY;
    baseGain_ = LITTLEROCKS_GAIN;
    soundDecay_ = LITTLEROCKS_SOUND_DECAY;
    decayScale_ = 0.95;
    varyFactor_ = 0.18;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type > 11 && type < 19 ) { // Nextmug
    nResonances_ = NEXTMUG_RESONANCES;
    if ( type > 12 )  // mug + coin
      nResonances_ += COIN_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = NEXTMUG_NUM_PARTS;
    for ( int i=0; i<NEXTMUG_RESONANCES; i++ ) {
      baseRadii_[i] = NEXTMUG_RADII[i];
      baseFrequencies_[i] = NEXTMUG_FREQUENCIES[i];
      filters_[i].gain = NEXTMUG_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = NEXTMUG_SYSTEM_DECAY;
    baseGain_ = NEXTMUG_GAIN;
    soundDecay_ = NEXTMUG_SOUND_DECAY;
    decayScale_ = 0.95;
    setEqualization( 1.0, 0.0, -1.0 );

    // Add coins
    if ( type == 13 ) { // Mug + Penny
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = PENNY_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = PENNY_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = PENNY_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
    else if ( type == 14 ) { // Mug + Nickel
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = NICKEL_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = NICKEL_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = NICKEL_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
    else if ( type == 15 ) { // Mug + Dime
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = DIME_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = DIME_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = DIME_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
    else if ( type == 16 ) { // Mug + Quarter
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = QUARTER_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = QUARTER_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = QUARTER_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
    else if ( type == 17 ) { // Mug + Franc
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = FRANC_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = FRANC_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = FRANC_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
    else if ( type == 18 ) { // Mug + Peso
      for ( int i=0; i<COIN_RESONANCES; i++ ) {
        baseRadii_[i+NEXTMUG_RESONANCES] = PESO_RADII[i];
        baseFrequencies_[i+NEXTMUG_RESONANCES] = PESO_FREQUENCIES[i];
        filters_[i+NEXTMUG_RESONANCES].gain = PESO_GAINS[i];
        doVaryFrequency_[i+NEXTMUG_RESONANCES] = false;
      }
    }
  }
  else if ( type == 19 ) { // Guiro
    nResonances_ = GUIRO_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = GUIRO_NUM_PARTS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = GUIRO_RADII[i];
      baseFrequencies_[i] = GUIRO_FREQUENCIES[i];
      filters_[i].gain = GUIRO_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseGain_ = GUIRO_GAIN;
    soundDecay_ = GUIRO_SOUND_DECAY;
    ratchetCount_ = 0;
    baseRatchetDelta_ = 0.0001;
    ratchetDelta_ = baseRatchetDelta_;
    lastRatchetValue_ = -1;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 20 ) { // Wrench
    nResonances_ = WRENCH_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = WRENCH_NUM_PARTS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = WRENCH_RADII[i];
      baseFrequencies_[i] = WRENCH_FREQUENCIES[i];
      filters_[i].gain = WRENCH_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseGain_ = WRENCH_GAIN;
    soundDecay_ = WRENCH_SOUND_DECAY;
    ratchetCount_ = 0;
    baseRatchetDelta_ = 0.00015;
    ratchetDelta_ = baseRatchetDelta_;
    lastRatchetValue_ = -1;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else if ( type == 21 ) { // Water Drops
    nResonances_ = WATER_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    tempFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = WATER_NUM_SOURCES;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = WATER_RADII[i];
      baseFrequencies_[i] = WATER_FREQUENCIES[i];
      tempFrequencies_[i] = WATER_FREQUENCIES[i];
      filters_[i].gain = WATER_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = WATER_SYSTEM_DECAY;
    baseGain_ = WATER_GAIN;
    soundDecay_ = WATER_SOUND_DECAY;
    decayScale_ = 0.8;
    setEqualization( -1.0, 0.0, 1.0 );
  }
  else if ( type == 22 ) { // Tuned Bamboo Chimes (Angklung)
    nResonances_ = ANGKLUNG_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = ANGKLUNG_NUM_TUBES;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = ANGKLUNG_RADII[i];
      baseFrequencies_[i] = ANGKLUNG_FREQUENCIES[i];
      filters_[i].gain = ANGKLUNG_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = ANGKLUNG_SYSTEM_DECAY;
    baseGain_ = ANGKLUNG_GAIN;
    soundDecay_ = ANGKLUNG_SOUND_DECAY;
    decayScale_ = 0.7;
    setEqualization( 1.0, 0.0, -1.0 );
  }
  else { // Maraca (default)
    shakerType_ = 0;
    nResonances_ = MARACA_RESONANCES;
    filters_.resize( nResonances_ );
    baseFrequencies_.resize( nResonances_ );
    baseRadii_.resize( nResonances_ );
    doVaryFrequency_.resize( nResonances_ );
    baseObjects_ = MARACA_NUM_BEANS;
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      baseRadii_[i] = MARACA_RADII[i];
      baseFrequencies_[i] = MARACA_FREQUENCIES[i];
      filters_[i].gain = MARACA_GAINS[i];
      doVaryFrequency_[i] = false;
    }
    baseDecay_ = MARACA_SYSTEM_DECAY;
    baseGain_ = MARACA_GAIN;
    soundDecay_ = MARACA_SOUND_DECAY;
    decayScale_ = 0.97;
    setEqualization( 1.0, -1.0, 0.0 );
  }

  // Set common algorithm variables.
  shakeEnergy_ = 0.0;
  sndLevel_ = 0.0;
  nObjects_ = baseObjects_;
  systemDecay_ = baseDecay_;
  currentGain_ = log( nObjects_ ) * baseGain_ / nObjects_;

  for ( unsigned int i=0; i<nResonances_; i++ )
    setResonance( filters_[i], baseFrequencies_[i], baseRadii_[i] );
}

const double MAX_SHAKE = 1.0;


template<typename T>
void Shakers<T>::noteOn( T frequency, T amplitude )
{
  // Yep ... pretty kludgey, but it works!
  int noteNumber = (int) ((12 * log(frequency/220.0)/log(2.0)) + 57.01) % 32;
  if ( shakerType_ != noteNumber ) this->setType( noteNumber );

  shakeEnergy_ += amplitude * MAX_SHAKE * 0.1;
  if ( shakeEnergy_ > MAX_SHAKE ) shakeEnergy_ = MAX_SHAKE;
  if ( shakerType_==19 || shakerType_==20 ) ratchetCount_ += 1;
}


template<typename T>
void Shakers<T>::noteOff( T amplitude )
{
  shakeEnergy_ = 0.0;
  if ( shakerType_==19 || shakerType_==20 ) ratchetCount_ = 0;
}


template<typename T>
void Shakers<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Shakers::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == __SK_Breath_ || number == __SK_AfterTouch_Cont_ ) { // 2 or 128 ... energy
    if ( shakerType_ == 19 || shakerType_ == 20 ) {
      if ( lastRatchetValue_ < 0.0 ) ratchetCount_++;
      else ratchetCount_ = (int) fabs(value - lastRatchetValue_);
      ratchetDelta_ = baseRatchetDelta_ * ratchetCount_;
      lastRatchetValue_ = (int) value;
    }
    else {
      shakeEnergy_ += normalizedValue * MAX_SHAKE * 0.1;
      if ( shakeEnergy_ > MAX_SHAKE ) shakeEnergy_ = MAX_SHAKE;
    }
  }
  else if ( number == __SK_ModFrequency_ ) { // 4 ... decay
    systemDecay_ = baseDecay_ + ( 2.0 * (normalizedValue - 0.5) * decayScale_ * (1.0 - baseDecay_) );
  }
  else if ( number == __SK_FootControl_ ) { // 11 ... number of objects
    nObjects_ = (T) ( 2.0 * normalizedValue * baseObjects_ ) + 1.1;
    currentGain_ = log( nObjects_ ) * baseGain_ / nObjects_;
  }
  else if ( number == __SK_ModWheel_ ) { // 1 ... resonance frequency
    for ( unsigned int i=0; i<nResonances_; i++ ) {
      T temp = baseFrequencies_[i] * pow( 4.0, normalizedValue-0.5 );
      setResonance( filters_[i], temp, baseRadii_[i] );
    }
  }
  else  if (number == __SK_ShakerInst_) { // 1071
    unsigned int type = (unsigned int) (value + 0.5);	//  Just to be safe
    this->setType( type );
  }
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Shakers::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace
