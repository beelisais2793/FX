
#include "GPContinuousOsc.h"

using namespace GP;

ContinuousOsc::ContinuousOsc(unsigned int wtSize)
	:WTSize(wtSize)
{
	// check that size is power of two
	auto wtSizeLog = std::log2(wtSize);
	assert((wtSizeLog - std::floor(wtSizeLog)) == 0.f);

	this->wt1.resize(wtSize);
	this->wt2.resize(wtSize);

	rngs.resize(wtSize);

	for (int i = 0; i < rngs.size(); i++)
	{
		rngs[i].setSeed(std::rand());
		float rngStep = 0.5;
		rngs[i].setParameters(rngStep, 0.2 * rngStep);
	}

	rndVector.resize(wtSize);


	// TODO remove temporary Model
	auto varianceKernel = VarianceKernel(0.15);

	std::vector<std::pair<float, float>> points;

	points.push_back({0, 0 });
	points.push_back({ 0.2, +1.f });
	points.push_back({ 0.5, +0.5f });
	points.push_back({ 0.7, -1.f });

	this->model = new Model(points, varianceKernel, WTSize, Model::Generation::Periodic);
}



void GP::ContinuousOsc::generateTable(std::vector<float>* table)
{
	// safety to ensure that Eigen isn't doing anything crazy.
	Eigen::internal::set_is_malloc_allowed(false);

	// update drunk rng's
	for (int i = 0; i < WTSize; i++)
	{
		rndVector[i] = rngs[i].getF();
	}
		
	auto &varMat  = model->getVarInv();
	auto &meanVec = model->getMean();

	Eigen::Map<const Eigen::VectorXf> rndVecEigen(rndVector.data(), WTSize);
	
	for (int pIdx = 0; pIdx < WTSize; pIdx++)
	{
		// calculate variance window
		float phase = pIdx / static_cast<float>(WTSize);

		float winPhase = (phase < 0.5) ? 10 * phase : 5 - 10 * (phase - 0.5);

		winPhase = std::min(winPhase, 1.f);
		float window = 0.5 - 0.5 * std::cos(M_PI * winPhase);
		
		auto varVec = varMat.data() + pIdx * WTSize;
		
		Eigen::Map<const Eigen::VectorXf> v2(varVec, WTSize);
		float var = (rndVecEigen.transpose() * v2)(0);


		// combine mean and variance
		float sample = var * window + meanVec(pIdx);
		(*table)[pIdx] = sample;
	}
	Eigen::internal::set_is_malloc_allowed(true);
}