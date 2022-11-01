#include "catch/catch.hpp"

#include "GPModel.h"

#include <chrono>
#include <iostream>

using namespace GP;

//#define GPMODEL_RUN_PERFORMANCE_TEST

struct ModelData
{	
	Eigen::VectorXf mean;
	Eigen::MatrixXf var;
	Eigen::MatrixXf var_inv;
};

ModelData generateModelReference(std::vector<std::pair<float, float>> points, const VarianceKernel &varKernel, unsigned int size)
{
	ModelData data;

	auto numPoints = points.size();

	Eigen::VectorXf x, y;
	x.resize(points.size());
	y.resize(points.size());

	for (int i = 0; i < points.size(); i++)
	{
		x[i] = points[i].first;
		y[i] = points[i].second;
	}
	
	// linear spaced x vector, discard x==1
	Eigen::VectorXf xn = Eigen::ArrayXf::LinSpaced(size + 1, 0, 1);
	xn.conservativeResize(size);

	Eigen::MatrixXf kk;
	kk.resize(x.size(), x.size());
	for (int ix = 0; ix < x.size(); ix++)
	{
		for (int iy = 0; iy < x.size(); iy++)
		{
			kk(ix, iy) = varKernel.valueAt(x[ix], x[iy]);
		}
	}

	Eigen::MatrixXf kkn;
	kkn.resize(x.size(), size);
	for (int ix = 0; ix < kkn.rows(); ix++)
	{
		for (int iy = 0; iy < kkn.cols(); iy++)
		{
			kkn(ix, iy) = varKernel.valueAt(x[ix], xn[iy]);
		}
	}

	Eigen::MatrixXf knkn;
	knkn.resize(size, size);
	for (int ix = 0; ix < size; ix++)
	{
		for (int iy = 0; iy < size; iy++)
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

	data.mean = yn;
	data.var = sn;

	data.var_inv = sn_inv;
	return data;

}


void compareModel(Model model, ModelData reference)
{

	// 1st compare mean
	float meanEpsilon(0.0001);
	auto mean = model.getMean();
	REQUIRE(mean.size() == reference.mean.size());

	for (int i = 0; i < mean.size(); i++)
	{
		REQUIRE(mean[i] == Approx(reference.mean[i]).epsilon(meanEpsilon));
	}

	// compare variance matrix
	float varEpsilon(0.0001);
	auto var = model.getVar();

	REQUIRE(var.rows() == var.cols());
	REQUIRE(var.rows() == reference.var.rows());
	REQUIRE(var.cols() == reference.var.cols());

	for (int r = 0; r < var.rows(); r++)
	{
		for (int c = 0; c < var.cols(); c++)
		{
			REQUIRE(var(r,c) == Approx(reference.var(r,c)).epsilon(varEpsilon));
		}
	}

	// TODO: compare decomposed variance matrix
	

}

#ifdef  GPMODEL_RUN_PERFORMANCE_TEST

TEST_CASE("Model Generaton", "[Model]")
{

	std::vector<std::pair<float, float>> values;

	int tests = 10;

	auto updateValues = [&]()
	{
		values =
		{
			{ rand() / static_cast<float>(RAND_MAX), 0 },
			{ 1, 1 }
		};
	};

	GP::VarianceKernel varKernel(0.5);

	std::cout << "And ..." << std::endl;

	auto start = std::chrono::steady_clock().now();

	for (int i = 0; i < tests; i++)
	{
		updateValues();
		auto model = GP::Model(values, varKernel, 2048, GP::Model::Generation::Single);
	}

	auto stop = std::chrono::steady_clock().now();
	
	auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

	std::cout << "Took " << dt/static_cast<float>(tests) << "ms" << std::endl;
}
#endif //  GPMODEL_RUN_PERFORMANCE_TEST


TEST_CASE("Model Regression", "[Model]")
{

	int order   = rand() % 100 + 100;
	int samples = rand() % 10 + 2;


	std::vector<std::pair<float, float>> values;
	values.resize(samples);
	for (int i = 0; i < samples; i++)
	{
		values[i].first = i / static_cast<float>(samples);
		values[i].second = rand() / static_cast<float>(RAND_MAX);
	}
	
	GP::VarianceKernel varKernel(0.5);

	auto refModel = generateModelReference(values, varKernel, order);

	auto testModel = Model(values, varKernel, order, GP::Model::Generation::Single);

	compareModel(testModel, refModel);


}