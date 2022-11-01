
#include "GPRandom.h"


namespace GP
{
	/*
	Produces a smooth normal (unclipped) random sequence.


	Parameters:
	- velocity determines the maximum change between two samples
	- draw incoperates a spring back effect

	x[n+1] = x[n] + vel * (rand(-1,+1) - draw * x[n]);

	A standard normal distribution can be achieved with draw ~ 0.2 * vel;
	While (0.1 < vel < 1)

	*/
	class DrunkRNG
	{
	public:
		inline DrunkRNG() = default;
		inline DrunkRNG(uint64_t seed);
		inline DrunkRNG(std::string seed);

		inline void setSeed(uint64_t seed);
		inline void setSeed(std::string seed);


		/*
			Resets the seed as well as the state
		*/
		inline void reset(float state = 0);

		/**
			Produces a singe sample
		*/
		inline float getF();

		/**
			Sets the velocity which determines the maximum change per sample.
		*/
		inline void setVelocity(float vel);

		/**
			Sets the draw, which determines the change towards 0 for higher abs(x)
		*/
		inline void setDraw(float draw);

		inline void setParameters(float vel, float draw);


	private:
		SeedableRNG rng;

		float vel{ 0.5f };
		float draw{ 0.2f * vel };

		float state{ 0 };
	};
}

GP::DrunkRNG::DrunkRNG(uint64_t seed) : rng(seed)
{}

inline GP::DrunkRNG::DrunkRNG(std::string seed) : rng(seed)
{}

inline void GP::DrunkRNG::setSeed(uint64_t seed)
{
	rng.setSeed(seed);
}

inline void GP::DrunkRNG::setSeed(std::string seed)
{
	rng.setSeed(seed);
}

inline void GP::DrunkRNG::reset(float state)
{
	rng.resetSeed();
	this->state = state;
}

inline float GP::DrunkRNG::getF()
{
	float change = vel * (rng.getF(-1.f, +1.f) - state * draw);
	return state += change;
}

void GP::DrunkRNG::setVelocity(float vel)
{
	this->vel = vel;
}

void GP::DrunkRNG::setDraw(float draw)
{
	this->draw = draw;
}

void GP::DrunkRNG::setParameters(float vel, float draw)
{
	setVelocity(vel);
	setDraw(draw);
}
