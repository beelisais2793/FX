#pragma once

#include <vector>
#define EIGEN_RUNTIME_NO_MALLOC
#include "../libs/Eigen/Dense"

#include "GPVarianceKernel.h"

namespace GP
{
	using MatrixRowMaj = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
	

	class Model
	{
	public:

		enum class Generation
		{
			Single,
			Periodic
		};

	public:
		Model(std::vector<std::pair<float, float>> points, const VarianceKernel &varKernel, unsigned int size, Generation gen);
		

	public:

		unsigned int getSize() const;

		const Eigen::VectorXf & getMean()   const;
		const MatrixRowMaj & getVar()    const;
		const MatrixRowMaj & getVarInv() const;


	private:
		const unsigned int Size;

		Eigen::VectorXf mean;
		MatrixRowMaj var;
		MatrixRowMaj var_inv;
	};

}