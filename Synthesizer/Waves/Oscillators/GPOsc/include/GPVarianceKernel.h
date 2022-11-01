#pragma once

#include <vector>
#include <functional>


// TODO this is hard coded for now, needs refactoring
// TODO extend with 1st and 2nd derivative?

namespace GP
{
	class VarianceKernel
	{
	public:
		VarianceKernel(float sigma);

		float valueAt(float x, float y) const;

	private: 
		float sigma;

	};

}