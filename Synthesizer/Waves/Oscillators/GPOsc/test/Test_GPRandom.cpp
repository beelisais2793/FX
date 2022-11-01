#include "catch/catch.hpp"

#include "GPRandom.h"

#include <array>
#include <algorithm>
#include <numeric>
using namespace GP;


std::vector<float> getDistribution(std::vector<float> buffer, unsigned int numBins)
{

	auto min = *std::min_element(buffer.begin(), buffer.end());
	auto max = *std::max_element(buffer.begin(), buffer.end());

	std::vector<float> distribution;
	distribution.resize(numBins);
	std::fill(distribution.begin(),distribution.end(), 0);


	for (auto sample : buffer)
	{
		float norm = (sample - min) / (max - min);
		float idx = std::round(norm * (numBins-1));

		distribution[idx]++;
	}

	for (auto &sample : distribution) sample /= buffer.size();

	return distribution;
}

TEST_CASE("SeedableRNG::getF", "[RNG]")
{
	static const int N = 100;
	
	SeedableRNG rng;

	for (int i= 0; i < N; i++)
	{
		float x = rng.getF();
		REQUIRE(0.f <= x);
		REQUIRE(x  <= 1.f);
	}

	float min = -1;
	float max =  0;
	for (int i = 0; i < N; i++)
	{
		float x = rng.getF(min, max);
		REQUIRE(min <= x);
		REQUIRE(x <= max);
	}
}

TEST_CASE("SeedableRNG", "[RNG]")
{
	static const int N = 100;
	std::array<float, N> sequence;

	SeedableRNG rng1;

	for (auto &x : sequence)
	{
		x = rng1.getF();
		REQUIRE(0.f <= x);
		REQUIRE(x <= 1.f);
	}

	SeedableRNG rng2;

	for (auto &x : sequence)
	{
		auto y = rng2.getF();
		REQUIRE(x == y);
	}
}


TEST_CASE("SeedableRNG::setSeed(std::string)", "[RNG]")
{
	static const int N = 10;
	std::array<float, N> sequence;

	SeedableRNG rng1("Hello");

	for (auto &x : sequence)
	{
		x = rng1.getF();
		REQUIRE(0.f <= x);
		REQUIRE(x <= 1.f);
	}

	rng1.setSeed("Hello");

	for (auto &x : sequence)
	{
		auto y = rng1.getF();
		REQUIRE(x == y);
	}
}


TEST_CASE("SeedableRNG::setSeed(uint64_t)", "[RNG]")
{
	static const int N = 10;
	std::array<float, N> sequence;

	uint64_t seed = static_cast<uint32_t> (std::rand());

	SeedableRNG rng1(seed);

	for (auto &x : sequence)
	{
		x = rng1.getF();
		REQUIRE(0.f <= x);
		REQUIRE(x <= 1.f);
	}

	rng1.setSeed(seed);

	for (auto &x : sequence)
	{
		auto y = rng1.getF();
		REQUIRE(x == y);
	}
}


TEST_CASE("SeedableRNG::resetSeed", "[RNG]")
{
	static const int N = 10;
	std::array<float, N> sequence;

	SeedableRNG rng1("TestSeed");

	for (auto &x : sequence)
	{
		x = rng1.getF();
		REQUIRE(0.f <= x);
		REQUIRE(x <= 1.f);
	}

	rng1.resetSeed();

	for (auto &x : sequence)
	{
		auto y = rng1.getF();
		REQUIRE(x == y);
	}
}


TEST_CASE("SeedableRNG distribution)", "[RNG]")
{
	static const int N = 2000;

	SeedableRNG rng("TestSeed");

	std::vector<float> samples;
	samples.reserve(N);
	for (int i = 0; i < N; i++) samples.push_back(rng.getF());

	
	auto distribution = getDistribution(samples, N/200);

	auto mean = std::accumulate(distribution.begin(), distribution.end(), 0.f) / static_cast<float>(distribution.size());
	
	// expecting all other bins to be at variance
	for (int i = 1; i < (distribution.size() -1); i++)
	{
		REQUIRE(distribution[i] == Approx(mean).margin(0.02));
	}
}