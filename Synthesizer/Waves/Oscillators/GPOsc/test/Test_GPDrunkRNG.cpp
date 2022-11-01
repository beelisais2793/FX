#include "catch/catch.hpp"

#include "GPDrunkRNG.h"

#include <array>
#include <algorithm>
#include <numeric>

using namespace GP;


TEST_CASE("DrunkRNG::reset", "[RNG]")
{
	static const int N = 10;
	std::array<float, N> sequence;

	DrunkRNG drunk;

	for (int i = 0; i < N; i++)
	{
		sequence [i] = drunk.getF();
	}

	drunk.reset(0);

	for (int i = 0; i < N; i++)
	{
		REQUIRE(sequence[i] == drunk.getF());
	}
}

TEST_CASE("DrunkRNG::setSeed", "[RNG]")
{
	DrunkRNG drunk1("Seed");
	DrunkRNG drunk2;
	drunk2.setSeed("Seed");

	DrunkRNG drunk3("A different Seed");

	static const int N = 10;
	for (int i = 0; i < N; i++)
	{
		float d1 = drunk1.getF();
		float d2 = drunk2.getF();
		float d3 = drunk3.getF();

		REQUIRE(d1 == d2);
		REQUIRE(d1 != d3);
	}
}

TEST_CASE("DrunkRNG::setVelocity", "[RNG]")
{
	DrunkRNG drunk1("Seed");

	float vel = 0.2;
	float draw = 0;
	
	drunk1.setVelocity(vel);
	drunk1.setDraw(0);
	
	float x = 0;

	static const int N = 10;
	for (int i = 0; i < N; i++)
	{
		float xn = drunk1.getF();
		REQUIRE(std::abs(xn - x) <= vel);
		x = xn;
	}
}
