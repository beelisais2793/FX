#pragma once 

/**
 * "gammaenv" produces smoothed-out S-curve envelope signal with the specified
 * attack and release characteristics. The attack and release times can be
 * further adjusted in real-time. Delay parameter is also specified as the
 * percentage of the total time.
 *
 * The S-curve produced by this envelope algorithm closely resembles a
 * sine-wave signal slightly augmented via the tanh() function. Such
 * augmentation makes the shape slightly steeper and in the end allows the
 * algorithm to follow it closer. The name "gammaenv" relates to this
 * algorithm's version.
 *
 * The algorithm's topology is based on 5 sets of "leaky integrators" (the
 * simplest form of 1st order low-pass filters). Each set (except the 5th) use
 * 4 low-pass filters in series. Outputs of all sets are then simply
 * summed/subtracted to produce the final result. The topology is numerically
 * stable for any valid input signal, but may produce envelope overshoots
 * depending on the input signal.
 *
 * Up to 25% of total attack (or release) time can be allocated (via Delay
 * parameters) to the delay stage. The delay is implemented by the same
 * topology: this has the benefit of not requiring additional memory
 * buffering. For example, it is possible to get up to 250 ms delay on
 * a 1-second envelope release stage without buffering.
 *
 * The processSymm() function provides the "perfect" implementation of the
 * algorithm, but it is limited to the same attack and release times. A more
 * universal process() function can work with any attack and release times,
 * but it is about 2 times less efficient and the actual attack stage's
 * envelope can range from the "designed" U to the undesired sharp V shape.
 * Unfortunately, the author was unable to find an approach that could be
 * similar to the processSymm() function while providing differing attack and
 * release times (the best approach found so far lengthens the delay stage
 * unpredictably).
 */

#include <cmath>
#include "SoundAlchemy.hpp"
namespace SoundAlchemy::Envelopes
{
	template<typename T>
	class TGammaEnvelope : public Object<T>
	{
	public:
		T Attack; ///< Attack time, in seconds.
		T Release; ///< Release time, in seconds.
		T AttackDelay; ///< Attack's delay stage percentage [0; 0.25].
		T ReleaseDelay; ///< Release's delay stage percentage [0; 0.25].
		bool IsInverse; ///< "True" if the input signal is inversed (compression
			/// signal).

	protected:
		static const int envcount = 4; ///< The number of envelopes in use.
		static const int envcount4 = envcount * 4; ///< =envcount * 4 (fixed).
		T env[ envcount4 ]; ///< Signal envelope stages 1-4.
		T enva[ 4 ]; ///< Attack stage envelope multipliers 1-4.
		T envb[ 4 ]; ///< Release stage envelope multipliers 1-4.
		T envr[ envcount4 ]; ///< Signal envelope (release) stages 1-4.
		T env5; ///< Signal envelope stage 5.
		T enva5; ///< Attack stage envelope multiplier 5.
		T envb5; ///< Release stage envelope multiplier 5.
		T envr5; ///< Signal envelope (release) stage 5.
		T prevr; ///< Previous output (release).

		/**
		 * Function calculates low-pass filter coefficients (multipliers) for the
		 * specified SampleRate, Time and o values. This function's implementation
		 * is based on a set of tabulated values transformed into formulas. Hence
		 * it may not be useful to explore this function, because the original
		 * tabulated values were auto-generated via non-linear optimization: while
		 * these values are useful (they just work), they are not descriptive of
		 * the underlying regularity.
		 *
		 * @param SampleRate Sample rate.
		 * @param Time Envelope's time in seconds.
		 * @param o Envelope's delay in percent [0; 0.25].
		 * @param[out] envs Resulting envelope multipliers 1-4.
		 * @param[out] envs5 Resulting envelope multiplier 5.
		 */

		static void calcMults( const T SampleRate, const T Time,
			const T o, T* const envs, T& envs5 )
		{
			const T o2 = o * o;

			if( o <= 0.074 )
			{
				envs[ 3 ] = 0.44548 + 0.00920770 * std::cos( 90.2666 * o ) -
					3.18551 * o - 0.132021 * std::cos( 377.561 * o2 ) -
					90.2666 * o * o2 * std::cos( 90.2666 * o );
			}
			else
			if( o <= 0.139 )
			{
				envs[ 3 ] = 0.00814353 + 3.07059 * o + 0.00356226 *
					std::cos( 879.555 * o2 );
			}
			else
			if( o <= 0.180 )
			{
				envs[ 3 ] = 0.701590 + o2 * ( 824.473 * o * o2 - 11.8404 );
			}
			else
			{
				envs[ 3 ] = 1.86814 + o * ( 84.0061 * o2 - 10.8637 ) -
					0.0122863 / o2;
			}

			const T e3 = envs[ 3 ];

			envs[ 0 ] = 0.901351 + o * ( 12.2872 * e3 + o * ( 78.0614 -
				213.130 * o ) - 9.82962 ) + e3 * ( 0.024808 *
				std::exp( 7.29048 * e3 ) - 5.4571 * e3 );
			const T e0 = envs[ 0 ];

			const T e3exp = std::exp( 1.31354 * e3 + 0.181498 * o );
			envs[ 1 ] = e3 * ( e0 * ( 2.75054 * o - 1.0 ) - 0.611813 * e3 *
				e3exp ) + 0.821369 * e3exp - 0.845698;
			const T e1 = envs[ 1 ];

			envs[ 2 ] = 0.860352 + e3 * ( 1.17208 - 0.579576 * e0 ) + o * ( e0 *
				( 1.94324 - 1.95438 * o ) + 1.20652 * e3 ) - 1.08482 * e0 -
				2.14670 * e1;

			if( o >= 0.0750 )
			{
				envs5 = 0.00118;
			}
			else
			{
				envs5 = e0 * ( 2.68318 - 2.08720 * o ) + 0.485294 * std::log( e3 ) +
					3.5805e-10 * std::exp( 27.0504 * e0 ) - 0.851199 - 1.24658 * e3 -
					0.885938 * std::log( e0 );
			}

			const T c = (2*M_PI) / SampleRate;
			envs[ 0 ] = calcLP1CoeffLim( c / ( Time * envs[ 0 ]));
			envs[ 1 ] = calcLP1CoeffLim( c / ( Time * envs[ 1 ]));
			envs[ 2 ] = calcLP1CoeffLim( c / ( Time * envs[ 2 ]));
			envs[ 3 ] = calcLP1CoeffLim( c / ( Time * envs[ 3 ]));
			envs5 = calcLP1CoeffLim( c / ( Time * envs5 ));
		}

		/**
		 * Function calculates first-order low-pass filter coefficient for the
		 * given Theta frequency (0 to pi, inclusive). Returned coefficient in the
		 * form ( 1.0 - coeff ) can be used as a coefficient for a high-pass
		 * filter. This ( 1.0 - coeff ) can be also used as a gain factor for the
		 * high-pass filter so that when high-passed signal is summed with
		 * low-passed signal at the same Theta frequency the resuling sum signal
		 * is unity.
		 *
		 * @param theta Low-pass filter's circular frequency, >= 0.
		 */

		static T calcLP1Coeff( const T theta )
		{
			const T costheta2 = 2.0 - std::cos( theta );
			return( 1.0 - ( costheta2 - std::sqrt( costheta2 * costheta2 - 1.0 )));
		}

		/**
		 * Function checks the supplied parameter, limits it to "pi" and calls the
		 * calcLP1Coeff() function.
		 *
		 * @param theta Low-pass filter's circular frequency, >= 0.
		 */

		static T calcLP1CoeffLim( const T theta )
		{
			return( calcLP1Coeff( theta < M_PI ? theta : M_PI ));
		}

	public:

		TGammaEnv() = default;
		
		/**
		 * Function initializes or updates the internal variables. All public
		 * variables have to be defined before calling this function. The clear()
		 * function is needed to be called after the first init() function call.
		 *
		 * @param SampleRate Sample rate.
		 */

		void init( const T SampleRate )
		{
			T a;
			T adly;
			T b;
			T bdly;

			if( Attack < Release )
			{
				a = Attack;
				b = Release;
				adly = AttackDelay;
				bdly = ReleaseDelay;
			}
			else
			{
				b = Attack;
				a = Release;
				bdly = AttackDelay;
				adly = ReleaseDelay;
			}

			calcMults( SampleRate, a, adly, enva, enva5 );
			calcMults( SampleRate, b, bdly, envb, envb5 );
		}

		/**
		 * Function clears state of *this object.
		 *
		 * @param initv Initial state value.
		 */

		void clear( const T initv )
		{
			int i;

			for( i = 0; i < envcount4; i += 4 )
			{
				env[ i + 0 ] = initv;
				env[ i + 1 ] = initv;
				env[ i + 2 ] = initv;
				env[ i + 3 ] = initv;
				envr[ i + 0 ] = initv;
				envr[ i + 1 ] = initv;
				envr[ i + 2 ] = initv;
				envr[ i + 3 ] = initv;
			}

			env5 = initv;
			envr5 = initv;
			prevr = initv;
		}

		/**
		 * Function performs 1 sample processing and produces output sample.
		 *
		 * @param v Input sample.
		 */

		T process( T v )
		{
			const T resa = processSymm( v );
			const T cres = ( IsInverse ? resa <= prevr : resa >= prevr );
			int i;

			if( cres )
			{
				for( i = 0; i < envcount4; i += 4 )
				{
					envr[ i + 0 ] = resa;
					envr[ i + 1 ] = resa;
					envr[ i + 2 ] = resa;
					envr[ i + 3 ] = resa;
				}

				envr5 = resa;
				prevr = resa;

				return( resa );
			}

			envr[ 0 ] += ( v - envr[ 0 ]) * envb[ 0 ];
			envr[ 1 ] += ( envr5 - envr[ 1 ]) * envb[ 1 ];
			envr[ 2 ] += ( envr[ 4 * 3 + 1 ] - envr[ 2 ]) * envb[ 2 ];
			envr[ 3 ] += ( envr[ 4 * 3 + 0 ] - envr[ 3 ]) * envb[ 3 ];
			envr5 += ( envr[ 4 * 3 + 0 ] - envr5 ) * envb5;

			for( i = 4; i < envcount4; i += 4 )
			{
				envr[ i + 0 ] += ( envr[ i - 4 ] - envr[ i + 0 ]) * envb[ 0 ];
				envr[ i + 1 ] += ( envr[ i - 3 ] - envr[ i + 1 ]) * envb[ 1 ];
				envr[ i + 2 ] += ( envr[ i - 2 ] - envr[ i + 2 ]) * envb[ 2 ];
				envr[ i + 3 ] += ( envr[ i - 1 ] - envr[ i + 3 ]) * envb[ 3 ];
			}

			prevr = envr[ i - 4 ] + envr[ i - 3 ] + envr[ i - 2 ] -
				envr[ i - 1 ] - envr5;

			return( prevr );
		}

		T Tick(T input, T A=1, T X=1, T Y=1) {
			return A*X*Y*process(input);
		}
		
		/**
		 * Function performs 1 sample processing and produces output sample
		 * (symmetric mode, attack and release should be equal).
		 *
		 * @param v Input sample.
		 */

		T processSymm( T v )
		{
	//		_mm_empty(); // May be needed depending on the compiler output.
			env[ 0 ] += ( v - env[ 0 ]) * enva[ 0 ];
			env[ 1 ] += ( env5 - env[ 1 ]) * enva[ 1 ];
			env[ 2 ] += ( env[ 4 * 3 + 1 ] - env[ 2 ]) * enva[ 2 ];
			env[ 3 ] += ( env[ 4 * 3 + 0 ] - env[ 3 ]) * enva[ 3 ];
			env5 += ( env[ 4 * 3 + 0 ] - env5 ) * enva5;
			int i;

			for( i = 4; i < envcount4; i += 4 )
			{
				env[ i + 0 ] += ( env[ i - 4 ] - env[ i + 0 ]) * enva[ 0 ];
				env[ i + 1 ] += ( env[ i - 3 ] - env[ i + 1 ]) * enva[ 1 ];
				env[ i + 2 ] += ( env[ i - 2 ] - env[ i + 2 ]) * enva[ 2 ];
				env[ i + 3 ] += ( env[ i - 1 ] - env[ i + 3 ]) * enva[ 3 ];
			}

			return( env[ i - 4 ] + env[ i - 3 ] + env[ i - 2 ] -
				env[ i - 1 ] - env5 );
		}

		T TickS(T input) {
			return processSymm(input);
		}
		void ProcessS(size_t n, T * input, T * output) {
			for(size_t i = 0; i < n; i++) output[i] = processSymm(input[i]);
		}
		void ProcessS(T * samples,size_t n) {
			for(size_t i = 0; i < n; i++) samples[i] = processSymm(samples[i]);
		}

	};

} // namespace vox

