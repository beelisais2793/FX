#pragma once

#include <cstring>

#define	MAXBUFFERSIZE 8192		// must be about 1/5 of a second at given sample rate
#define ROUND(n)		((int)((DspFloatType)(n)+0.5))
#define	MODF(n,i,f) ((i) = (int)(n), (f) = (n) - (DspFloatType)(i))
//	modf - vaguely related to the library routine modf(), this macro breaks a DspFloatType into
//	integer and fractional components i and f respectively.
//	n - input number, a DspFloatType
//	i - integer portion, an integer (the input number integer portion should fit)
//	f - fractional portion, a DspFloatType


#define	NUM_MIX_MODES	7
#define	NUM_DELAYS	11

class Chorus2
{
public:
    enum 
    {
        // Parameters Tags
        kRate = 0,
        kWidth,
        kDelay,
        kWetLevel,

        kNumParams
    };

	Chorus2();
	~Chorus2();
	
	void resetBuffer();
	void resetCoeffs();
	
	void setParameter(int index, DspFloatType value);
	void setSampleRate (DspFloatType sampleRate);
	
	void setRate ();
	void setWidth ();
	void setDelay ();
	void setWetLevel ();
	void setSweep();
	
	void process(DspFloatType &in);
	void process(DspFloatType &inL, DspFloatType &inR);
	void process(DspFloatType &in);
	void process(DspFloatType &inL, DspFloatType &inR);

private :
	DspFloatType *parameter_;
	DspFloatType paramSweepRate;
	DspFloatType paramWidth;
	DspFloatType paramDelay;
	DspFloatType paramWetLevel;	
	
	DspFloatType _minSweepSamples;	// lower bound of calculated sweep range, calc'd by setSweep from rate, width, delay
	DspFloatType _maxSweepSamples;	// upper bound, ditto
	
	DspFloatType _sweepRate;			// actual calculated sweep rate
	int		_sweepSamples;			// sweep width in # of samples
	int	   _delaySamples;		// number of samples to run behind filling pointer

	DspFloatType *_bufferL;		// stored sound
	DspFloatType *_bufferR;
	int	   writeIndex;					// fill/write pointer
	DspFloatType _step;				// amount to step the sweep each sample
	DspFloatType _sweep;
    DspFloatType sampleRate_;
    std::string name_;
};

inline
Chorus::Chorus()
{
	name_ = "Chorus";

	parameter_ = new DspFloatType[kNumParams];

	for(int i = 0; i < kNumParams; ++i)
	{
		parameter_[i] = 0;
	}

	//reset();
	//constructor
	paramSweepRate = 0.2f;
	paramWidth = 0.3f;
	paramDelay = 0.2f;
	paramWetLevel = 0.5f;
	
	_sweepRate = 0.2;
	_sweepSamples = 0;
	_delaySamples = 10;
	writeIndex = 0;
	_sweep = 0.0;

	// allocate the buffer
	_bufferL = new DspFloatType[MAXBUFFERSIZE];
	_bufferR = new DspFloatType[MAXBUFFERSIZE];
}


inline
Chorus::~Chorus ()
{
	//destructor
	if(parameter_)
		delete[] parameter_;
	parameter_ = 0;
}


inline
void Chorus::resetBuffer()
{

}

inline
void Chorus::resetCoeffs()
{

}


inline
void Chorus::setSampleRate(DspFloatType sampleRate)
{
	sampleRate_ = sampleRate;
}


inline
void Chorus::setParameter(int index, DspFloatType value)
{
	switch(index)
	{
	case kRate :
		parameter_[kRate] = value;
		paramSweepRate = parameter_[kRate];
		break;
	
	case kWidth :
		parameter_[kWidth] = value;
		paramWidth = parameter_[kWidth];
		break;

	case kDelay :
		parameter_[kDelay] = value;
		paramDelay = parameter_[kDelay];
		break;

	case kWetLevel :
		parameter_[kWetLevel] = value;
		paramWetLevel = parameter_[kWetLevel];
		break;
	}
}


inline
void Chorus::setRate ()
{
	// map into param onto desired sweep range with log curve
	_sweepRate = pow(10.0,(DspFloatType)paramSweepRate);
	_sweepRate  -= 0.1f;
	_sweepRate  *= 1.1f;
	_sweepRate  += 0.1f;

	// finish setup
	setSweep();
}

inline
void Chorus::setWidth ()
{
	// map so that we can spec between 0ms and 50ms
	_sweepSamples = ROUND(paramWidth * 0.05 * sampleRate_);

	// finish setup
	setSweep();
}


inline
void Chorus::setDelay ()
{	
	DspFloatType delay = pow(10.0, (DspFloatType)paramDelay * 2.0)/1000.0;		// map logarithmically and convert to seconds
	_delaySamples = ROUND(delay * sampleRate_);

	// finish setup
	setSweep();
}


inline
void Chorus::setWetLevel()
{
	
}

inline
void Chorus::setSweep()
{
	// calc # of samples per second we'll need to move to achieve spec'd sweep rate
	_step = (DspFloatType)(_sweepSamples * 2.0 * _sweepRate) / sampleRate_;
	
	if( _step <= 1.0 ) // NEED CALC
		_step = 1.0;	// NEED CALC
		
	// calc min and max sweep now
	_minSweepSamples = _delaySamples;
	_maxSweepSamples = _delaySamples + _sweepSamples;

	// set intial sweep pointer to midrange
	_sweep = (_minSweepSamples + _maxSweepSamples) / 2;
}

inline
void Chorus::process(DspFloatType &inL, DspFloatType &inR)
{
		// assemble input value and store it in circle queue
		_bufferL[writeIndex] = inL;
		_bufferR[writeIndex] = inR;
		
		writeIndex = (writeIndex + 1) & (MAXBUFFERSIZE-1);

		// build the two emptying pointers and do linear interpolation
		int ep1, ep2;
		DspFloatType w1, w2;
		DspFloatType ep = writeIndex - _sweep;
		
		MODF(ep, ep1, w2);
		
		ep1 &= (MAXBUFFERSIZE-1);
		ep2 = ep1 + 1;
		ep2 &= (MAXBUFFERSIZE-1);
		
		w1 = 1.0 - w2;
		
		DspFloatType outL = _bufferL[ep1] * w1 + _bufferL[ep2] * w2;
		DspFloatType outR = _bufferL[ep1] *w1 + _bufferR[ep2] * w2;

		// develop output wet
		inL = (DspFloatType)(paramWetLevel * outL + (1 - paramWetLevel) * inL);
		inR = (DspFloatType)(paramWetLevel * outR + (1 - paramWetLevel) * inR);

		// increment the sweep
		_sweep += _step;
		if( _sweep >= _maxSweepSamples || _sweep <= _minSweepSamples )
		{
			_step = -_step;
		}

}

inline
void Chorus::process(DspFloatType &in)
{
		// assemble input value and store it in circle queue
		_bufferL[writeIndex] = in;
		
		writeIndex = (writeIndex + 1) & (MAXBUFFERSIZE-1);

		// build the two emptying pointers and do linear interpolation
		int ep1, ep2;
		DspFloatType w1, w2;
		DspFloatType ep = writeIndex - _sweep;
		
		MODF(ep, ep1, w2);
		
		ep1 &= (MAXBUFFERSIZE-1);
		ep2 = ep1 + 1;
		ep2 &= (MAXBUFFERSIZE-1);
		
		w1 = 1.0 - w2;
		
		DspFloatType outL = _bufferL[ep1] * w1 + _bufferL[ep2] * w2;
		DspFloatType outR = _bufferL[ep1] *w1 + _bufferR[ep2] * w2;

		// develop output wet
		in = (DspFloatType)(paramWetLevel * outL + (1 - paramWetLevel) * in);

		// increment the sweep
		_sweep += _step;
		if( _sweep >= _maxSweepSamples || _sweep <= _minSweepSamples )
		{
			_step = -_step;
		}
}

inline
void Chorus::process(DspFloatType &in)
{
		// assemble input value and store it in circle queue
		_bufferL[writeIndex] = in;
		
		writeIndex = (writeIndex + 1) & (MAXBUFFERSIZE-1);

		// build the two emptying pointers and do linear interpolation
		int ep1, ep2;
		DspFloatType w1, w2;
		DspFloatType ep = writeIndex - _sweep;
		
		MODF(ep, ep1, w2);
		
		ep1 &= (MAXBUFFERSIZE-1);
		ep2 = ep1 + 1;
		ep2 &= (MAXBUFFERSIZE-1);
		
		w1 = 1.0 - w2;
		
		DspFloatType outL = _bufferL[ep1] * w1 + _bufferL[ep2] * w2;
		DspFloatType outR = _bufferL[ep1] *w1 + _bufferR[ep2] * w2;

		// develop output wet
		in = paramWetLevel * outL + (1 - paramWetLevel) * in;

		// increment the sweep
		_sweep += _step;
		if( _sweep >= _maxSweepSamples || _sweep <= _minSweepSamples )
		{
			_step = -_step;
		}
}


inline
void Chorus::process(DspFloatType &inL, DspFloatType &inR)
{
		// assemble input value and store it in circle queue
		_bufferL[writeIndex] = inL;
		_bufferR[writeIndex] = inR;
		
		writeIndex = (writeIndex + 1) & (MAXBUFFERSIZE-1);

		// build the two emptying pointers and do linear interpolation
		int ep1, ep2;
		DspFloatType w1, w2;
		DspFloatType ep = writeIndex - _sweep;
		
		MODF(ep, ep1, w2);
		
		ep1 &= (MAXBUFFERSIZE-1);
		ep2 = ep1 + 1;
		ep2 &= (MAXBUFFERSIZE-1);
		
		w1 = 1.0 - w2;
		
		DspFloatType outL = _bufferL[ep1] * w1 + _bufferL[ep2] * w2;
		DspFloatType outR = _bufferL[ep1] *w1 + _bufferR[ep2] * w2;

		// develop output wet
		inL = paramWetLevel * outL + (1 - paramWetLevel) * inL;
		inR = paramWetLevel * outR + (1 - paramWetLevel) * inR;

		// increment the sweep
		_sweep += _step;
		if( _sweep >= _maxSweepSamples || _sweep <= _minSweepSamples )
		{
			_step = -_step;
		}
}
