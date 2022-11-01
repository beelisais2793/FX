#include "..\include\GPModel.h"

#include <iostream>

using namespace GP;

Model::Model(std::vector<std::pair<float, float>> points, const VarianceKernel & varKernel, unsigned int size, Generation gen)
	: Size(size)
{
	unsigned int numPoints;

	Eigen::VectorXf x, y;
	// TODO, try/catch, assert all the stupid setups

	
	if (gen == Generation::Periodic)
	{
		numPoints = 3 * points.size();
		x.resize(numPoints);
		y.resize(numPoints);

		for (int i = 0; i < numPoints; i++)
		{
			float xOffset = (i < points.size()) ? -1 : ((i >= 2 * points.size()) ? +1 : 0);
			x[i] = points[i % points.size()].first + xOffset;
			y[i] = points[i % points.size()].second;
		}
	}
	else
	{
		numPoints = points.size();
		x.resize(numPoints);
		y.resize(numPoints);

		for (int i = 0; i < points.size(); i++)
		{
			x[i] = points[i].first;
			y[i] = points[i].second;
		}

	}



	// linear spaced x vector, discard x==1
	Eigen::VectorXf xn = Eigen::ArrayXf::LinSpaced(Size + 1, 0, 1);
	xn.conservativeResize(Size);

	Eigen::MatrixXf kk;
	kk.resize(x.size(), x.size());
	for (int ix = 0; ix <  x.size(); ix++)
	{
		for (int iy = 0; iy <  x.size(); iy++)
		{
			kk(ix, iy) = varKernel.valueAt(x[ix], x[iy]);
		}
	}

	Eigen::MatrixXf kkn;
	kkn.resize( x.size(), Size);
	for (int ix = 0; ix <  kkn.rows(); ix++)
	{
		for (int iy = 0; iy < kkn.cols(); iy++)
		{
			kkn(ix, iy) = varKernel.valueAt(x[ix], xn[iy]);
		}
	}

	Eigen::MatrixXf knkn;
	knkn.resize(Size, Size);
	for (int ix = 0; ix < Size; ix++)
	{
		for (int iy = 0; iy < Size; iy++)
		{
			knkn(ix, iy) = varKernel.valueAt(xn[ix], xn[iy]);
		}
	}


	// calculate mean
	auto yn = (kkn.transpose() * kk.inverse()) * y;

	// calculate variance matrix
	auto sn = knkn - ((kkn.transpose() * kk.inverse()) * kkn);

	// calculate svd for prepared variance matrix
	auto sn_svd = sn.jacobiSvd(Eigen::ComputeFullU);

	Eigen::MatrixXf S = sn_svd.singularValues().asDiagonal();
	Eigen::MatrixXf U = sn_svd.matrixU();

	Eigen::MatrixXf sn_inv = U * S.cwiseSqrt();
	
	this->mean = yn;
	this->var  = sn;

	this->var_inv = sn_inv;

}


unsigned int GP::Model::getSize() const
{
	return Size;
}

const Eigen::VectorXf & GP::Model::getMean() const
{
	return mean;
}

const GP::MatrixRowMaj & GP::Model::getVar() const
{
	return var;
}

const GP::MatrixRowMaj & GP::Model::getVarInv() const
{
	return var_inv;
}
