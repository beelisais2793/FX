/*
 *	Simple Compressor (header)
 *
 *  File		: SimpleComp.h
 *	Library		: SimpleSource
 *  Version		: 1.12
 *  Class		: SimpleComp, SimpleCompRms
 *
 *	© 2006, ChunkWare Music Software, OPEN-SOURCE
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *	DEALINGS IN THE SOFTWARE.
 */


#pragma once


#if _MSC_VER > 1000			// MS Visual Studio
#define INLINE __forceinline	// forces inline
#define NOMINMAX				// for standard library min(), max()
#define _USE_MATH_DEFINES		// for math constants
#else						// other IDE's
#define INLINE inline
#endif

#include <algorithm>	// for min(), max()
#include <cassert>		// for assert()
#include <cmath>
#include <vector>

namespace Dynamics
{
	//-------------------------------------------------------------
	// DC offset (to prevent denormal)
	//-------------------------------------------------------------

	// USE:
	// 1. init envelope state to DC_OFFSET before processing
	// 2. add to input before envelope runtime function
	static const DspFloatType DC_OFFSET = 1.0E-25;

	//-------------------------------------------------------------
	// envelope detector
	//-------------------------------------------------------------
	class EnvelopeDetector
	{
	public:
		EnvelopeDetector(
			  DspFloatType ms = 1.0
			, DspFloatType sampleRate = 44100.0
			);
		virtual ~EnvelopeDetector() {}

		// time constant
		virtual void   setTc( DspFloatType ms );
		virtual DspFloatType getTc( void ) const { return ms_; }

		// sample rate
		virtual void   setSampleRate( DspFloatType sampleRate );
		virtual DspFloatType getSampleRate( void ) const { return sampleRate_; }

		// runtime function
		INLINE void run( DspFloatType in, DspFloatType &state ) {
			state = in + coef_ * ( state - in );
		}

	protected:
			
		DspFloatType sampleRate_;		// sample rate
		DspFloatType ms_;				// time constant in ms
		DspFloatType coef_;			// runtime coefficient
		virtual void setCoef( void );	// coef calculation

	};	// end SimpleComp class

	//-------------------------------------------------------------
	// attack/release envelope
	//-------------------------------------------------------------
	class AttRelEnvelope
	{
	public:
		AttRelEnvelope(
			  DspFloatType att_ms = 10.0
			, DspFloatType rel_ms = 100.0
			, DspFloatType sampleRate = 44100.0
			);
		virtual ~AttRelEnvelope() {}

		// attack time constant
		virtual void   setAttack( DspFloatType ms );
		virtual DspFloatType getAttack( void ) const { return att_.getTc(); }

		// release time constant
		virtual void   setRelease( DspFloatType ms );
		virtual DspFloatType getRelease( void ) const { return rel_.getTc(); }

		// sample rate dependencies
		virtual void   setSampleRate( DspFloatType sampleRate );
		virtual DspFloatType getSampleRate( void ) const { return att_.getSampleRate(); }

		// runtime function
		INLINE void run( DspFloatType in, DspFloatType &state ) {

			/* assumes that:
			* positive delta = attack
			* negative delta = release
			* good for linear & log values
			*/

			if ( in > state )
				att_.run( in, state );	// attack
			else
				rel_.run( in, state );	// release
		}

	private:
			
		EnvelopeDetector att_;
		EnvelopeDetector rel_;
		
	};	// end AttRelEnvelope class


	//-------------------------------------------------------------
	// gain functions
	//-------------------------------------------------------------

	// linear -> dB conversion
	static INLINE DspFloatType lin2dB( DspFloatType lin ) {
		static const DspFloatType LOG_2_DB = 8.6858896380650365530225783783321;	// 20 / ln( 10 )
		return log( lin ) * LOG_2_DB;
	}

	// dB -> linear conversion
	static INLINE DspFloatType dB2lin( DspFloatType dB ) {
		static const DspFloatType DB_2_LOG = 0.11512925464970228420089957273422;	// ln( 10 ) / 20
		return exp( dB * DB_2_LOG );
	}

//-------------------------------------------------------------
	// envelope detector
	//-------------------------------------------------------------
	EnvelopeDetector::EnvelopeDetector( DspFloatType ms, DspFloatType sampleRate )
	{
		assert( sampleRate > 0.0 );
		assert( ms > 0.0 );
		sampleRate_ = sampleRate;
		ms_ = ms;
		setCoef();
	}

	//-------------------------------------------------------------
	void EnvelopeDetector::setTc( DspFloatType ms )
	{
		assert( ms > 0.0 );
		ms_ = ms;
		setCoef();
	}

	//-------------------------------------------------------------
	void EnvelopeDetector::setSampleRate( DspFloatType sampleRate )
	{
		assert( sampleRate > 0.0 );
		sampleRate_ = sampleRate;
		setCoef();
	}

	//-------------------------------------------------------------
	void EnvelopeDetector::setCoef( void )
	{
		coef_ = exp( -1000.0 / ( ms_ * sampleRate_ ) );
	}

	//-------------------------------------------------------------
	// attack/release envelope
	//-------------------------------------------------------------
	AttRelEnvelope::AttRelEnvelope( DspFloatType att_ms, DspFloatType rel_ms, DspFloatType sampleRate )
		: att_( att_ms, sampleRate )
		, rel_( rel_ms, sampleRate )
	{
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setAttack( DspFloatType ms )
	{
		att_.setTc( ms );
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setRelease( DspFloatType ms )
	{
		rel_.setTc( ms );
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setSampleRate( DspFloatType sampleRate )
	{
		att_.setSampleRate( sampleRate );
		rel_.setSampleRate( sampleRate );
	}
	//-------------------------------------------------------------
	// simple compressor
	//-------------------------------------------------------------
	class SimpleComp : public AttRelEnvelope
	{
	public:
		SimpleComp();
		virtual ~SimpleComp() {}

		// parameters
		virtual void setThresh( DspFloatType dB );
		virtual void setRatio( DspFloatType dB );

		virtual DspFloatType getThresh( void ) const { return threshdB_; }
		virtual DspFloatType getRatio( void )  const { return ratio_; }

		// runtime
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( DspFloatType &in1, DspFloatType &in2 );	// compressor runtime process
		void process( DspFloatType &in1, DspFloatType &in2, DspFloatType keyLinked );	// with stereo-linked key in

	private:

		// transfer function
		DspFloatType threshdB_;		// threshold (dB)
		DspFloatType ratio_;			// ratio (compression: < 1 ; expansion: > 1)

		// runtime variables
		DspFloatType envdB_;			// over-threshold envelope (dB)

	};	// end SimpleComp class

	//-------------------------------------------------------------
	// simple compressor with RMS detection
	//-------------------------------------------------------------
	class SimpleCompRms : public SimpleComp
	{
	public:
		SimpleCompRms();
		virtual ~SimpleCompRms() {}

		// sample rate
		virtual void setSampleRate( DspFloatType sampleRate );

		// RMS window
		virtual void setWindow( DspFloatType ms );
		virtual DspFloatType getWindow( void ) const { return ave_.getTc(); }

		// runtime process
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( DspFloatType &in1, DspFloatType &in2 );	// compressor runtime process

	private:

		EnvelopeDetector ave_;	// averager
		DspFloatType aveOfSqrs_;		// average of squares

	};	// end SimpleCompRms class

    //-------------------------------------------------------------
	INLINE void SimpleComp::process( DspFloatType &in1, DspFloatType &in2 )
	{
		// create sidechain

		DspFloatType rect1 = fabs( in1 );	// rectify input
		DspFloatType rect2 = fabs( in2 );

		/* if desired, one could use another EnvelopeDetector to smooth
		 * the rectified signal.
		 */

		DspFloatType link = std::max( rect1, rect2 );	// link channels with greater of 2

		process( in1, in2, link );	// rest of process
	}

	//-------------------------------------------------------------
	INLINE void SimpleComp::process( DspFloatType &in1, DspFloatType &in2, DspFloatType keyLinked )
	{
		keyLinked = fabs( keyLinked );		// rectify (just in case)

		// convert key to dB
		keyLinked += DC_OFFSET;				// add DC offset to avoid log( 0 )
		DspFloatType keydB = lin2dB( keyLinked );	// convert linear -> dB

		// threshold
		DspFloatType overdB = keydB - threshdB_;	// delta over threshold
		if ( overdB < 0.0 )
			overdB = 0.0;

		// attack/release

		overdB += DC_OFFSET;					// add DC offset to avoid denormal
		AttRelEnvelope::run( overdB, envdB_ );	// run attack/release envelope
		overdB = envdB_ - DC_OFFSET;			// subtract DC offset

		/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
		 * the attack/release processes, the envelope will never fall below the offset,
		 * thereby avoiding denormals. However, to prevent the offset from causing
		 * constant gain reduction, we must subtract it from the envelope, yielding
		 * a minimum value of 0dB.
		 */

		// transfer function
		DspFloatType gr = overdB * ( ratio_ - 1.0 );	// gain reduction (dB)
		gr = dB2lin( gr );						// convert dB -> linear

		// output gain
		in1 *= gr;	// apply gain reduction to input
		in2 *= gr;
	}

	//-------------------------------------------------------------
	INLINE void SimpleCompRms::process( DspFloatType &in1, DspFloatType &in2 )
	{
		// create sidechain

		DspFloatType inSq1 = in1 * in1;	// square input
		DspFloatType inSq2 = in2 * in2;

		DspFloatType sum = inSq1 + inSq2;			// power summing
		sum += DC_OFFSET;					// DC offset, to prevent denormal
		ave_.run( sum, aveOfSqrs_ );		// average of squares
		DspFloatType rms = sqrt( aveOfSqrs_ );	// rms (sort of ...)

		/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
		 * calculation. A true RMS is an FIR moving average. This
		 * approximation is a 1-pole IIR. Nonetheless, in practice,
		 * and in the interest of simplicity, this method will suffice,
		 * giving comparable results.
		 */

		SimpleComp::process( in1, in2, rms );	// rest of process
	}

    //-------------------------------------------------------------
	// simple compressor
	//-------------------------------------------------------------
	SimpleComp::SimpleComp()
		: AttRelEnvelope( 10.0, 100.0 )
		, threshdB_( 0.0 )
		, ratio_( 1.0 )
		, envdB_( DC_OFFSET )
	{
	}

	//-------------------------------------------------------------
	void SimpleComp::setThresh( DspFloatType dB )
	{
		threshdB_ = dB;
	}

	//-------------------------------------------------------------
	void SimpleComp::setRatio( DspFloatType ratio )
	{
		assert( ratio > 0.0 );
		ratio_ = ratio;
	}

	//-------------------------------------------------------------
	void SimpleComp::initRuntime( void )
	{
		envdB_ = DC_OFFSET;
	}

	//-------------------------------------------------------------
	// simple compressor with RMS detection
	//-------------------------------------------------------------
	SimpleCompRms::SimpleCompRms()
		: ave_( 5.0 )
		, aveOfSqrs_( DC_OFFSET )
	{
	}

	//-------------------------------------------------------------
	void SimpleCompRms::setSampleRate( DspFloatType sampleRate )
	{
		SimpleComp::setSampleRate( sampleRate );
		ave_.setSampleRate( sampleRate );
	}

	//-------------------------------------------------------------
	void SimpleCompRms::setWindow( DspFloatType ms )
	{
		ave_.setTc( ms );
	}

	//-------------------------------------------------------------
	void SimpleCompRms::initRuntime( void )
	{
		SimpleComp::initRuntime();
		aveOfSqrs_ = DC_OFFSET;
	}


//-------------------------------------------------------------
	// simple gate
	//-------------------------------------------------------------
	class SimpleGate : public AttRelEnvelope
	{
	public:
		SimpleGate();
		virtual ~SimpleGate() {}

		// parameters
		virtual void   setThresh( DspFloatType dB );
		virtual DspFloatType getThresh( void ) const { return threshdB_; }
		
		// runtime
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( DspFloatType &in1, DspFloatType &in2 );	// gate runtime process
		void process( DspFloatType &in1, DspFloatType &in2, DspFloatType keyLinked );	// with stereo-linked key in
		
	private:
		
		// transfer function
		DspFloatType threshdB_;	// threshold (dB)
		DspFloatType thresh_;		// threshold (linear)
		
		// runtime variables
		DspFloatType env_;		// over-threshold envelope (linear)
		
	};	// end SimpleGate class

	//-------------------------------------------------------------
	// simple gate with RMS detection
	//-------------------------------------------------------------
	class SimpleGateRms : public SimpleGate
	{
	public:
		SimpleGateRms();
		virtual ~SimpleGateRms() {}

		// sample rate
		virtual void setSampleRate( DspFloatType sampleRate );

		// RMS window
		virtual void setWindow( DspFloatType ms );
		virtual DspFloatType getWindow( void ) const { return ave_.getTc(); }

		// runtime process
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( DspFloatType &in1, DspFloatType &in2 );	// gate runtime process

	private:

		EnvelopeDetector ave_;	// averager
		DspFloatType aveOfSqrs_;		// average of squares

	};	// end SimpleGateRms class

//-------------------------------------------------------------
	INLINE void SimpleGate::process( DspFloatType &in1, DspFloatType &in2 )
	{
		// create sidechain

		DspFloatType rect1 = fabs( in1 );	// rectify input
		DspFloatType rect2 = fabs( in2 );

		/* if desired, one could use another EnvelopeDetector to smooth
		 * the rectified signal.
		 */

		DspFloatType link = std::max( rect1, rect2 );	// link channels with greater of 2

		process( in1, in2, link );	// rest of process
	}

	//-------------------------------------------------------------
	INLINE void SimpleGate::process( DspFloatType &in1, DspFloatType &in2, DspFloatType keyLinked )
	{
		keyLinked = fabs( keyLinked );	// rectify (just in case)

		// threshold
		// key over threshold ( 0.0 or 1.0 )
		DspFloatType over = DspFloatType( keyLinked > thresh_ );

		// attack/release
		over += DC_OFFSET;					// add DC offset to avoid denormal
		AttRelEnvelope::run( over, env_ );	// run attack/release
		over = env_ - DC_OFFSET;			// subtract DC offset

		/* REGARDING THE DC OFFSET: In this case, since the offset is added before 
		 * the attack/release processes, the envelope will never fall below the offset,
		 * thereby avoiding denormals. However, to prevent the offset from causing
		 * constant gain reduction, we must subtract it from the envelope, yielding
		 * a minimum value of 0dB.
		 */

		// output gain
		in1 *= over;	// apply gain reduction to input
		in2 *= over;
	}

	//-------------------------------------------------------------
	INLINE void SimpleGateRms::process( DspFloatType &in1, DspFloatType &in2 )
	{
		// create sidechain

		DspFloatType inSq1 = in1 * in1;	// square input
		DspFloatType inSq2 = in2 * in2;

		DspFloatType sum = inSq1 + inSq2;			// power summing
		sum += DC_OFFSET;					// DC offset, to prevent denormal
		ave_.run( sum, aveOfSqrs_ );		// average of squares
		DspFloatType rms = sqrt( aveOfSqrs_ );	// rms (sort of ...)

		/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
		 * calculation. A true RMS is an FIR moving average. This
		 * approximation is a 1-pole IIR. Nonetheless, in practice,
		 * and in the interest of simplicity, this method will suffice,
		 * giving comparable results.
		 */

		SimpleGate::process( in1, in2, rms );	// rest of process
	}

    //-------------------------------------------------------------
	SimpleGate::SimpleGate()
		: AttRelEnvelope( 1.0, 100.0 )
		, threshdB_( 0.0 )
		, thresh_( 1.0 )
		, env_( DC_OFFSET )
	{
	}

	//-------------------------------------------------------------
	void SimpleGate::setThresh( DspFloatType dB )
	{
		threshdB_ = dB;
		thresh_ = dB2lin( dB );
	}

	//-------------------------------------------------------------
	void SimpleGate::initRuntime( void )
	{
		env_ = DC_OFFSET;
	}

	//-------------------------------------------------------------
	// simple gate with RMS detection
	//-------------------------------------------------------------
	SimpleGateRms::SimpleGateRms()
		: ave_( 5.0 )
		, aveOfSqrs_( DC_OFFSET )
	{
	}

	//-------------------------------------------------------------
	void SimpleGateRms::setSampleRate( DspFloatType sampleRate )
	{
		SimpleGate::setSampleRate( sampleRate );
		ave_.setSampleRate( sampleRate );
	}

	//-------------------------------------------------------------
	void SimpleGateRms::setWindow( DspFloatType ms )
	{
		ave_.setTc( ms );
	}

	//-------------------------------------------------------------
	void SimpleGateRms::initRuntime( void )
	{
		SimpleGate::initRuntime();
		aveOfSqrs_ = DC_OFFSET;
	}

    //-------------------------------------------------------------
	// simple limiter
	//-------------------------------------------------------------
	class SimpleLimit
	{
	public:
		SimpleLimit();
		virtual ~SimpleLimit() {}

		// parameters
		virtual void setThresh( DspFloatType dB );
		virtual void setAttack( DspFloatType ms );
		virtual void setRelease( DspFloatType ms );

		virtual DspFloatType getThresh( void )  const { return threshdB_; }
		virtual DspFloatType getAttack( void )  const { return att_.getTc(); }
		virtual DspFloatType getRelease( void ) const { return rel_.getTc(); }

		// latency
		virtual const unsigned int getLatency( void ) const { return peakHold_; }

		// sample rate dependencies
		virtual void   setSampleRate( DspFloatType sampleRate );
		virtual DspFloatType getSampleRate( void ) { return att_.getSampleRate(); }
		
		// runtime
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( DspFloatType &in1, DspFloatType &in2 );	// limiter runtime process

	protected:

		// class for faster attack/release
		class FastEnvelope : public EnvelopeDetector
		{
		public:
			FastEnvelope( DspFloatType ms = 1.0, DspFloatType sampleRate = 44100.0 )
				: EnvelopeDetector( ms, sampleRate )
			{}
			virtual ~FastEnvelope() {}

		protected:
			// override setCoef() - coefficient calculation
			virtual void setCoef( void );
		};
		
	private:
		
		// transfer function
		DspFloatType threshdB_;	// threshold (dB)
		DspFloatType thresh_;		// threshold (linear)

		// max peak
		unsigned int peakHold_;		// peak hold (samples)
		unsigned int peakTimer_;	// peak hold timer
		DspFloatType maxPeak_;			// max peak

		// attack/release envelope
		FastEnvelope att_;			// attack
		FastEnvelope rel_;			// release
		DspFloatType env_;				// over-threshold envelope (linear)

		// buffer
		// BUFFER_SIZE default can handle up to ~10ms at 96kHz
		// change this if you require more
		static const int BUFFER_SIZE = 1024;	// buffer size (always a power of 2!)
		unsigned int mask_;						// buffer mask
		unsigned int cur_;						// cursor
		std::vector< DspFloatType > outBuffer_[ 2 ];	// output buffer
		
	};	// end SimpleLimit class

    //-------------------------------------------------------------
	SimpleLimit::SimpleLimit()
		: threshdB_( 0.0 )
		, thresh_( 1.0 )
		, peakHold_( 0 )
		, peakTimer_( 0 )
		, maxPeak_( 1.0 )
		, att_( 1.0 )
		, rel_( 10.0 )
		, env_( 1.0 )
		, mask_( BUFFER_SIZE - 1 )
		, cur_( 0 )
	{
		setAttack( 1.0 );
		outBuffer_[ 0 ].resize( BUFFER_SIZE, 0.0 );
		outBuffer_[ 1 ].resize( BUFFER_SIZE, 0.0 );
	}

	//-------------------------------------------------------------
	void SimpleLimit::setThresh( DspFloatType dB )
	{
		threshdB_ = dB;
		thresh_ = dB2lin( dB );
	}

	//-------------------------------------------------------------
	void SimpleLimit::setAttack( DspFloatType ms )
	{
		unsigned int samp = int( 0.001 * ms * att_.getSampleRate() );

		assert( samp < BUFFER_SIZE );

		peakHold_ = samp;
		att_.setTc( ms );
	}

	//-------------------------------------------------------------
	void SimpleLimit::setRelease( DspFloatType ms )
	{
		rel_.setTc( ms );
	}

	//-------------------------------------------------------------
	void SimpleLimit::setSampleRate( DspFloatType sampleRate )
	{
		att_.setSampleRate( sampleRate );
		rel_.setSampleRate( sampleRate );
	}

	//-------------------------------------------------------------
	void SimpleLimit::initRuntime( void )
	{
		peakTimer_ = 0;
		maxPeak_ = thresh_;
		env_ = thresh_;
		cur_ = 0;
		outBuffer_[ 0 ].assign( BUFFER_SIZE, 0.0 );
		outBuffer_[ 1 ].assign( BUFFER_SIZE, 0.0 );
	}

	//-------------------------------------------------------------
	void SimpleLimit::FastEnvelope::setCoef( void )
	{
		// rises to 99% of in value over duration of time constant
		coef_ = pow( 0.01, (1000.0 / (ms_ * sampleRate_) ) );
	}

    //-------------------------------------------------------------
	INLINE void SimpleLimit::process( DspFloatType &in1, DspFloatType &in2 )
	{
		// create sidechain

		DspFloatType rect1 = fabs( in1 );	// rectify input
		DspFloatType rect2 = fabs( in2 );

		DspFloatType keyLink = std::max( rect1, rect2 );	// link channels with greater of 2

		// threshold
		// we always want to feed the sidechain AT LEATS the threshold value
		if ( keyLink < thresh_ )
			keyLink = thresh_;

		// test:
		// a) whether peak timer has "expired"
		// b) whether new peak is greater than previous max peak
		if ( (++peakTimer_ >= peakHold_) || (keyLink > maxPeak_) ) {
			// if either condition is met:
			peakTimer_ = 0;		// reset peak timer
			maxPeak_ = keyLink;	// assign new peak to max peak
		}

		/* REGARDING THE MAX PEAK: This method assumes that the only important
		 * sample in a look-ahead buffer would be the highest peak. As such,
		 * instead of storing all samples in a look-ahead buffer, it only stores
		 * the max peak, and compares all incoming samples to that one.
		 * The max peak has a hold time equal to what the look-ahead buffer
		 * would have been, which is tracked by a timer (counter). When this
		 * timer expires, the sample would have exited from the buffer. Therefore,
		 * a new sample must be assigned to the max peak. We assume that the next
		 * highest sample in our theoretical buffer is the current input sample.
		 * In reality, we know this is probably NOT the case, and that there has
		 * been another sample, slightly lower than the one before it, that has
		 * passed the input. If we do not account for this possibility, our gain
		 * reduction could be insufficient, resulting in an "over" at the output.
		 * To remedy this, we simply apply a suitably long release stage in the
		 * envelope follower.
		 */

		// attack/release
		if ( maxPeak_ > env_ )
			att_.run( maxPeak_, env_ );		// run attack phase
		else
			rel_.run( maxPeak_, env_ );		// run release phase

		/* REGARDING THE ATTACK: This limiter achieves "look-ahead" detection
		 * by allowing the envelope follower to attack the max peak, which is
		 * held for the duration of the attack phase -- unless a new, higher
		 * peak is detected. The output signal is buffered so that the gain
		 * reduction is applied in advance of the "offending" sample.
		 */

		/* NOTE: a DC offset is not necessary for the envelope follower,
		 * as neither the max peak nor envelope should fall below the
		 * threshold (which is assumed to be around 1.0 linear).
		 */

		// gain reduction
		DspFloatType gR = thresh_ / env_;

		// unload current buffer index
		// ( cur_ - delay ) & mask_ gets sample from [delay] samples ago
		// mask_ variable wraps index
		unsigned int delayIndex = ( cur_ - peakHold_ ) & mask_;
		DspFloatType delay1 = outBuffer_[ 0 ][ delayIndex ];
		DspFloatType delay2 = outBuffer_[ 1 ][ delayIndex ];

		// load current buffer index and advance current index
		// mask_ wraps cur_ index
		outBuffer_[ 0 ][ cur_ ] = in1;
		outBuffer_[ 1 ][ cur_ ] = in2;
		++cur_ &= mask_;

		// output gain
		in1 = delay1 * gR;	// apply gain reduction to input
		in2 = delay2 * gR;

		/* REGARDING THE GAIN REDUCTION: Due to the logarithmic nature
		 * of the attack phase, the sidechain will never achieve "full"
		 * attack. (Actually, it is only guaranteed to achieve 99% of
		 * the input value over the given time constant.) As such, the
		 * limiter cannot achieve "brick-wall" limiting. There are 2
		 * workarounds:
		 *
		 * 1) Set the threshold slightly lower than the desired threshold.
		 *		i.e. 0.0dB -> -0.1dB or even -0.5dB
		 *
		 * 2) Clip the output at the threshold, as such:
		 *
		 *		if ( in1 > thresh_ )		in1 = thresh_;
		 *		else if ( in1 < -thresh_ )	in1 = -thresh_;
		 *
		 *		if ( in2 > thresh_ )		in2 = thresh_;
		 *		else if ( in2 < -thresh_ )	in2 = -thresh_;
		 *
		 *		(... or replace with your favorite branchless clipper ...)
		 */
	}

}	// end namespace chunkware_simple


