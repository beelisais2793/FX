#include "..\include\GPVarianceKernel.h"

using namespace GP;

GP::VarianceKernel::VarianceKernel(float sigma)
	: sigma(sigma)
{
}

float VarianceKernel::valueAt(float x, float y) const
{
	return std::exp(-(x - y) * (x - y) / (2.f * sigma *sigma ));
}
