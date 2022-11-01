#pragma once
#include "StkInstrmnt.hpp"
#include "StkOnePole.hpp"
#include "SKINImsg.h"

namespace stk {

/***************************************************/
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000--2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/

const unsigned short NXMAX = 12;
const unsigned short NYMAX = 12;
const double VSCALE = 0.5;

template<typename T>
class Mesh2D : public Instrmnt<T>
{
 public:
  //! Class constructor, taking the x and y dimensions in samples.
  Mesh2D( unsigned short nX, unsigned short nY );

  //! Class destructor.
  ~Mesh2D( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the x dimension size in samples.
  void setNX( unsigned short lenX );

  //! Set the y dimension size in samples.
  void setNY( unsigned short lenY );

  //! Set the x, y input position on a 0.0 - 1.0 scale.
  void setInputPosition( T xFactor, T yFactor );

  //! Set the loss filters gains (0.0 - 1.0).
  void setDecay( T decayFactor );

  //! Impulse the mesh with the given amplitude (frequency ignored).
  void noteOn( T frequency, T amplitude );

  //! Stop a note with the given amplitude (speed of decay) ... currently ignored.
  void noteOff( T amplitude );

  //! Calculate and return the signal energy stored in the mesh.
  T energy( void );

  //! Input a sample to the mesh and compute one output sample.
  T inputTick( T input );

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

 protected:

  T tick0();
  T tick1();
  void clearMesh();

  unsigned short NX_, NY_;
  unsigned short xInput_, yInput_;
  OnePole<T>  filterX_[NXMAX];
  OnePole<T>  filterY_[NYMAX];
  T v_[NXMAX-1][NYMAX-1]; // junction velocities
  T vxp_[NXMAX][NYMAX];   // positive-x velocity wave
  T vxm_[NXMAX][NYMAX];   // negative-x velocity wave
  T vyp_[NXMAX][NYMAX];   // positive-y velocity wave
  T vym_[NXMAX][NYMAX];   // negative-y velocity wave

  // Alternate buffers
  T vxp1_[NXMAX][NYMAX];  // positive-x velocity wave
  T vxm1_[NXMAX][NYMAX];  // negative-x velocity wave
  T vyp1_[NXMAX][NYMAX];  // positive-y velocity wave
  T vym1_[NXMAX][NYMAX];  // negative-y velocity wave

  int counter_; // time in samples
};

template<typename T>
inline StkFrames<T>& Mesh2D<T>::tick( StkFrames<T>& frames, unsigned int channel )
{
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "Mesh2D::tick(): channel and StkFrames<T> arguments are incompatible!";
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
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000--2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/


template<typename T>
Mesh2D<T>::Mesh2D( unsigned short nX, unsigned short nY )
{
  if ( nX == 0.0 || nY == 0.0 ) {
    oStream_ << "Mesh2D::Mesh2D: one or more argument is equal to zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  this->setNX( nX );
  this->setNY( nY );

  T pole = 0.05;
  unsigned short i;
  for ( i=0; i<NYMAX; i++ ) {
    filterY_[i].setPole( pole );
    filterY_[i].setGain( 0.99 );
  }

  for ( i=0; i<NXMAX; i++ ) {
    filterX_[i].setPole( pole );
    filterX_[i].setGain( 0.99 );
  }

  this->clearMesh();

  counter_ = 0;
  xInput_ = 0;
  yInput_ = 0;
}

template<typename T>
Mesh2D<T>::~Mesh2D( void )
{
}

template<typename T>
void Mesh2D<T>::clear( void )
{
  this->clearMesh();

  unsigned short i;
  for ( i=0; i<NY_; i++ )
    filterY_[i].clear();

  for ( i=0; i<NX_; i++ )
    filterX_[i].clear();

  counter_ = 0;
}

template<typename T>
void Mesh2D<T>::clearMesh( void )
{
  int x, y;
  for ( x=0; x<NXMAX-1; x++ ) {
    for ( y=0; y<NYMAX-1; y++ ) {
      v_[x][y] = 0;
    }
  }
  for ( x=0; x<NXMAX; x++ ) {
    for ( y=0; y<NYMAX; y++ ) {

      vxp_[x][y] = 0;
      vxm_[x][y] = 0;
      vyp_[x][y] = 0;
      vym_[x][y] = 0;

      vxp1_[x][y] = 0;
      vxm1_[x][y] = 0;
      vyp1_[x][y] = 0;
      vym1_[x][y] = 0;
    }
  }
}

template<typename T>
T Mesh2D<T>::energy( void )
{
  // Return total energy contained in wave variables Note that some
  // energy is also contained in any filter delay elements.

  int x, y;
  T t;
  T e = 0;
  if ( counter_ & 1 ) { // Ready for Mesh2D::tick1() to be called.
    for ( x=0; x<NX_; x++ ) {
      for ( y=0; y<NY_; y++ ) {
        t = vxp1_[x][y];
        e += t*t;
        t = vxm1_[x][y];
        e += t*t;
        t = vyp1_[x][y];
        e += t*t;
        t = vym1_[x][y];
        e += t*t;
      }
    }
  }
  else { // Ready for Mesh2D::tick0() to be called.
    for ( x=0; x<NX_; x++ ) {
      for ( y=0; y<NY_; y++ ) {
        t = vxp_[x][y];
        e += t*t;
        t = vxm_[x][y];
        e += t*t;
        t = vyp_[x][y];
        e += t*t;
        t = vym_[x][y];
        e += t*t;
      }
    }
  }

  return e;
}

template<typename T>
void Mesh2D<T>::setNX( unsigned short lenX )
{
  if ( lenX < 2 ) {
    oStream_ << "Mesh2D::setNX(" << lenX << "): Minimum length is 2!";
    handleError( StkError::WARNING ); return;
  }
  else if ( lenX > NXMAX ) {
    oStream_ << "Mesh2D::setNX(" << lenX << "): Maximum length is " << NXMAX << '!';
    handleError( StkError::WARNING ); return;
  }

  NX_ = lenX;
}

template<typename T>
void Mesh2D<T>::setNY( unsigned short lenY )
{
  if ( lenY < 2 ) {
    oStream_ << "Mesh2D::setNY(" << lenY << "): Minimum length is 2!";
    handleError( StkError::WARNING ); return;
  }
  else if ( lenY > NYMAX ) {
    oStream_ << "Mesh2D::setNY(" << lenY << "): Maximum length is " << NXMAX << '!';
    handleError( StkError::WARNING ); return;
  }

  NY_ = lenY;
}

template<typename T>
void Mesh2D<T>::setDecay( T decayFactor )
{
  if ( decayFactor < 0.0 || decayFactor > 1.0 ) {
    oStream_ << "Mesh2D::setDecay: decayFactor is out of range!";
    handleError( StkError::WARNING ); return;
  }

  int i;
  for ( i=0; i<NYMAX; i++ )
    filterY_[i].setGain( decayFactor );

  for (i=0; i<NXMAX; i++)
    filterX_[i].setGain( decayFactor );
}

template<typename T>
void Mesh2D<T>::setInputPosition( T xFactor, T yFactor )
{
  if ( xFactor < 0.0 || xFactor > 1.0 ) {
    oStream_ << "Mesh2D::setInputPosition xFactor value is out of range!";
    handleError( StkError::WARNING ); return;
  }

  if ( yFactor < 0.0 || yFactor > 1.0 ) {
    oStream_ << "Mesh2D::setInputPosition yFactor value is out of range!";
    handleError( StkError::WARNING ); return;
  }

  xInput_ = (unsigned short) (xFactor * (NX_ - 1));
  yInput_ = (unsigned short) (yFactor * (NY_ - 1));
}

template<typename T>
void Mesh2D<T>::noteOn( T frequency, T amplitude )
{
  // Input at corner.
  if ( counter_ & 1 ) {
    vxp1_[xInput_][yInput_] += amplitude;
    vyp1_[xInput_][yInput_] += amplitude;
  }
  else {
    vxp_[xInput_][yInput_] += amplitude;
    vyp_[xInput_][yInput_] += amplitude;
  }
}

template<typename T>
void Mesh2D<T>::noteOff( T amplitude )
{
  return;
}

template<typename T>
T Mesh2D<T>::inputTick( T input )
{
  if ( counter_ & 1 ) {
    vxp1_[xInput_][yInput_] += input;
    vyp1_[xInput_][yInput_] += input;
    this->lastFrame_[0] = tick1();
  }
  else {
    vxp_[xInput_][yInput_] += input;
    vyp_[xInput_][yInput_] += input;
    this->lastFrame_[0] = tick0();
  }

  counter_++;
  return this->lastFrame_[0];
}

template<typename T>
T Mesh2D<T>::tick( unsigned int )
{
  this->lastFrame_[0] = ((counter_ & 1) ? this->tick1() : this->tick0());
  counter_++;
  return this->lastFrame_[0];
}



template<typename T>
T Mesh2D<T>::tick0( void )
{
  int x, y;
  T outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      v_[x][y] = ( vxp_[x][y] + vxm_[x+1][y] + 
		  vyp_[x][y] + vym_[x][y+1] ) * VSCALE;
    }
  }    

  // Update junction outgoing waves, using alternate wave-variable buffers.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      T vxy = v_[x][y];
      // Update positive-going waves.
      vxp1_[x+1][y] = vxy - vxm_[x+1][y];
      vyp1_[x][y+1] = vxy - vym_[x][y+1];
      // Update minus-going waves.
      vxm1_[x][y] = vxy - vxp_[x][y];
      vym1_[x][y] = vxy - vyp_[x][y];
    }
  }    

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY_-1; y++) {
    vxp1_[0][y] = filterY_[y].tick(vxm_[0][y]);
    vxm1_[NX_-1][y] = vxp_[NX_-1][y];
  }
  for (x=0; x<NX_-1; x++) {
    vyp1_[x][0] = filterX_[x].tick(vym_[x][0]);
    vym1_[x][NY_-1] = vyp_[x][NY_-1];
  }

  // Output = sum of outgoing waves at far corner.  Note that the last
  // index in each coordinate direction is used only with the other
  // coordinate indices at their next-to-last values.  This is because
  // the "unit strings" attached to each velocity node to terminate
  // the mesh are not themselves connected together.
  outsamp = vxp_[NX_-1][NY_-2] + vyp_[NX_-2][NY_-1];

  return outsamp;
}

template<typename T>
T Mesh2D<T>::tick1( void )
{
  int x, y;
  T outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      v_[x][y] = ( vxp1_[x][y] + vxm1_[x+1][y] + 
		  vyp1_[x][y] + vym1_[x][y+1] ) * VSCALE;
    }
  }

  // Update junction outgoing waves, 
  // using alternate wave-variable buffers.
  for (x=0; x<NX_-1; x++) {
    for (y=0; y<NY_-1; y++) {
      T vxy = v_[x][y];

      // Update positive-going waves.
      vxp_[x+1][y] = vxy - vxm1_[x+1][y];
      vyp_[x][y+1] = vxy - vym1_[x][y+1];

      // Update minus-going waves.
      vxm_[x][y] = vxy - vxp1_[x][y];
      vym_[x][y] = vxy - vyp1_[x][y];
    }
  }

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY_-1; y++) {
    vxp_[0][y] = filterY_[y].tick(vxm1_[0][y]);
    vxm_[NX_-1][y] = vxp1_[NX_-1][y];
  }
  for (x=0; x<NX_-1; x++) {
    vyp_[x][0] = filterX_[x].tick(vym1_[x][0]);
    vym_[x][NY_-1] = vyp1_[x][NY_-1];
  }

  // Output = sum of outgoing waves at far corner.
  outsamp = vxp1_[NX_-1][NY_-2] + vyp1_[NX_-2][NY_-1];

  return outsamp;
}

template<typename T>
void Mesh2D<T>::controlChange( int number, T value )
{
#if defined(_STK_DEBUG_)
  if ( Stk::inRange( value, 0.0, 128.0 ) == false ) {
    oStream_ << "Mesh2D::controlChange: value (" << value << ") is out of range!";
    handleError( StkError::WARNING ); return;
  }
#endif

  T normalizedValue = value * ONE_OVER_128;
  if ( number == 2 ) // 2
    this->setNX( (unsigned short) (normalizedValue * (NXMAX-2) + 2) );
  else if ( number == 4 ) // 4
    this->setNY( (unsigned short) (normalizedValue * (NYMAX-2) + 2) );
  else if ( number == 11 ) // 11
    this->setDecay( 0.9 + (normalizedValue * 0.1) );
  else if ( number == __SK_ModWheel_ ) // 1
    this->setInputPosition( normalizedValue, normalizedValue );
#if defined(_STK_DEBUG_)
  else {
    oStream_ << "Mesh2D::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }
#endif
}

} // stk namespace

