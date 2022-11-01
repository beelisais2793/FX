#pragma once

#include <cstdint>
#include <string>
#include <cassert>


namespace GP
{
	/**
		Implements a seedable random number generator using a xor-bitshift algorithm.

	*/
	class SeedableRNG
	{
	public:
		inline SeedableRNG(uint64_t seed = 88172645463325252);
		inline SeedableRNG(std::string seed);



		/**
			Directly sets the satet with given seed
		*/
		inline void setSeed(uint64_t seed);

		/**
			Initializes sate with a constant.
			Randomizes state by xor-ing 8-bit blocks with chars from string
		*/
		inline void setSeed(std::string seed);

		/**
			Resets state to last used setSeed()
		*/
		inline void resetSeed();

		/**
			Returns a signel random number between [0 .. 1]
		*/
		inline float getF();

		/**
			Returns a signel random number between [min .. max]
		*/
		inline float getF(float min, float max);

	private:
		inline void update();

	private:
		uint64_t state;
		
		const float MaxRand32Inv{ static_cast<float>(1. / 4294967295.) };

		uint64_t seed;

	};

}

inline GP::SeedableRNG::SeedableRNG(uint64_t seed)
{
	setSeed(seed);
}

inline GP::SeedableRNG::SeedableRNG(std::string seed)
{
	setSeed(seed);
}

inline void GP::SeedableRNG::setSeed(uint64_t seed)
{
	this->state = seed;
	this->seed = state;
}

inline void GP::SeedableRNG::setSeed(std::string seed)
{
	state = 88172645463325252;

	int pos = 0;
	for (uint64_t c : seed)
	{
		state ^= c << (pos * 8);
		pos = (pos + 1) % 8;
	}

	this->seed = state;
}

inline void GP::SeedableRNG::resetSeed()
{
	this->state = seed;
}

inline float GP::SeedableRNG::getF()
{
	update();
	return (state >> 32) * MaxRand32Inv;
}


inline float GP::SeedableRNG::getF(float min, float max)
{
	assert(max > min);
	return getF() * (max - min) + min;
}

inline void GP::SeedableRNG::update()
{
	state ^= state << 13;
	state ^= state >>  7;
	state ^= state << 17;
}