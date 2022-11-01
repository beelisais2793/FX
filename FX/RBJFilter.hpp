#pragma once

// http://shepazu.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

#include "KfrBiquads.hpp"
namespace Filters
{
	////////////////////////////////////////////////////////////////////////////////////////////
	// Rbj
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RbjFilter
	{

		FilterType filter_type;

		DspFloatType freq,sr,Q,dbGain;
		bool bandwidth;
		// filter coeffs
		DspFloatType b0a0,b1a0,b2a0,a1a0,a2a0;

		// in/out history
		DspFloatType ou1,ou2,in1,in2;

		RbjFilter(FilterType type, DspFloatType sample_rate, DspFloatType frequency, DspFloatType q=0.707, DspFloatType db_gain = 1.0f, bool q_is_bandwidth=false)
		{		
			b0a0=b1a0=b2a0=a1a0=a2a0=0.0;			
			ou1=ou2=in1=in2=0.0f;	
			filter_type = type;
			calc_filter_coeffs(frequency,sample_rate,q,db_gain,q_is_bandwidth);
		};

		DspFloatType filter(DspFloatType in0)
		{
			// filter
			Undenormal denormal;
			DspFloatType const yn = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;

			// push in/out buffers
			in2=in1;
			in1=in0;
			ou2=ou1;
			ou1=yn;

			// return output
			return yn;
		};

		DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0) {
			DspFloatType cut = freq;
			DspFloatType r   = Q;
			Undenormal denormal;
			calc_filter_coeffs(freq + 0.5*X*22050.0/12, sr, Q+0.5*Y, dbGain,bandwidth);
			DspFloatType x   = std::tanh(A*filter(A*I));
			calc_filter_coeffs(cut, sr, r, dbGain,bandwidth);
			return x;
		}


		void calc_filter_coeffs(DspFloatType frequency,DspFloatType sample_rate,DspFloatType q,DspFloatType  db_gain,bool q_is_bandwidth)
		{
			// temp pi
			DspFloatType const temp_pi=3.1415926535897932384626433832795;
			
			if(frequency < 0) frequency = 0;
			if(frequency > (sample_rate/2-1)) frequency = sample_rate/2-1;
			if(q < 0) q = 0;
			if(db_gain < 0) db_gain = 0;

			// temp coef vars
			DspFloatType alpha,a0,a1,a2,b0,b1,b2;
			freq = frequency;
			sr   = sample_rate;
			Q    = q;
			dbGain = db_gain;
			bandwidth = q_is_bandwidth;
			
				
			// peaking, lowshelf and hishelf
			if(filter_type == Peak || filter_type == Lowshelf || filter_type == Highshelf)
			{
				DspFloatType const A		=	pow(10.0,(db_gain/40.0));
				DspFloatType const omega	=	2.0*temp_pi*frequency/sample_rate;
				DspFloatType const tsin	=	sin(omega);
				DspFloatType const tcos	=	cos(omega);
				
				if(q_is_bandwidth)
				alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
				else
				alpha=tsin/(2.0*q);

				DspFloatType const beta	=	sqrt(A)/q;
				
				// peaking
				if(filter_type==Peak)
				{
					b0=DspFloatType(1.0+alpha*A);
					b1=DspFloatType(-2.0*tcos);
					b2=DspFloatType(1.0-alpha*A);
					a0=DspFloatType(1.0+alpha/A);
					a1=DspFloatType(-2.0*tcos);
					a2=DspFloatType(1.0-alpha/A);
				}
				
				// lowshelf
				if(filter_type== Lowshelf)
				{
					b0=DspFloatType(A*((A+1.0)-(A-1.0)*tcos+beta*tsin));
					b1=DspFloatType(2.0*A*((A-1.0)-(A+1.0)*tcos));
					b2=DspFloatType(A*((A+1.0)-(A-1.0)*tcos-beta*tsin));
					a0=DspFloatType((A+1.0)+(A-1.0)*tcos+beta*tsin);
					a1=DspFloatType(-2.0*((A-1.0)+(A+1.0)*tcos));
					a2=DspFloatType((A+1.0)+(A-1.0)*tcos-beta*tsin);
				}

				// hishelf
				if(filter_type==Highshelf)
				{
					b0=DspFloatType(A*((A+1.0)+(A-1.0)*tcos+beta*tsin));
					b1=DspFloatType(-2.0*A*((A-1.0)+(A+1.0)*tcos));
					b2=DspFloatType(A*((A+1.0)+(A-1.0)*tcos-beta*tsin));
					a0=DspFloatType((A+1.0)-(A-1.0)*tcos+beta*tsin);
					a1=DspFloatType(2.0*((A-1.0)-(A+1.0)*tcos));
					a2=DspFloatType((A+1.0)-(A-1.0)*tcos-beta*tsin);
				}
			}
			else
			{
				// other filters
				DspFloatType const omega	=	2.0*temp_pi*frequency/sample_rate;
				DspFloatType const tsin	=	sin(omega);
				DspFloatType const tcos	=	cos(omega);

				if(q_is_bandwidth)
				alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
				else
				alpha=tsin/(2.0*q);

				
				// lowpass
				if(filter_type==Lowpass)
				{
					b0=(1.0-tcos)/2.0;
					b1=1.0-tcos;
					b2=(1.0-tcos)/2.0;
					a0=1.0+alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}

				// hipass
				if(filter_type==Highpass)
				{
					b0=(1.0+tcos)/2.0;
					b1=-(1.0+tcos);
					b2=(1.0+tcos)/2.0;
					a0=1.0+ alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}

				// bandpass csg
				if(filter_type==Bandpass)
				{
					b0=tsin/2.0;
					b1=0.0;
					b2=-tsin/2;
					a0=1.0+alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}

				// bandpass czpg
				if(filter_type==Bandpass2)
				{
					b0=alpha;
					b1=0.0;
					b2=-alpha;
					a0=1.0+alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}

				// notch
				if(filter_type==Notch || filter_type == Bandstop)
				{
					b0=1.0;
					b1=-2.0*tcos;
					b2=1.0;
					a0=1.0+alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}

				// allpass
				if(filter_type==Allpass)
				{
					b0=1.0-alpha;
					b1=-2.0*tcos;
					b2=1.0+alpha;
					a0=1.0+alpha;
					a1=-2.0*tcos;
					a2=1.0-alpha;
				}
			}

			// set filter coeffs
			b0a0=DspFloatType(b0/a0);
			b1a0=DspFloatType(b1/a0);
			b2a0=DspFloatType(b2/a0);
			a1a0=DspFloatType(a1/a0);
			a2a0=DspFloatType(a2/a0);
		};

		
	};
}
