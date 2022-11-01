// https://github.com/JTriggerFish/TriggerFish-VCV/blob/master/src/models/VanDerPoleODE.hpp
// https://github.com/JTriggerFish/TriggerFish-VCV/blob/master/src/models/VdpOscillator.hpp
// https://github.com/JTriggerFish/TriggerFish-VCV/blob/master/src/models/ode.hpp
#pragma once
#include "../Eigen/Dense"
#include "../tfdsp/filters.hpp"
using namespace Eigen;

#pragma once
#include "../Eigen/Dense"

using namespace Eigen;

namespace ode
{
	/**
	 * \brief Representation of current and past states for a linear multistep ODE methods
	 * like BDF.
	 * Note that the number of past states is fixed at 8 even though the max useable order of BDF is 6 which would require storing 7 elements,
	 * this is done for alignment reasons and fixed to 8 for simplicity.
	 * TODO : review if this should be reduced for low order methods performance
	 * \tparam Scalar float or double
	 * \tparam Dim state space dimension, should be <= 4
	 */
	template<typename Scalar, int Dim>
	using StateMatrix = Eigen::Matrix<Scalar, Dim, 8>;

	template<typename Scalar, int Dim>
	using StateVector = Eigen::Matrix<Scalar, Dim, 1>;

	template<typename Function, typename Scalar, int Dim, int Order >
	class SubStep
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x);
	};
	template<typename Scalar>
	struct BDFNewtonConstants
	{
		static constexpr Scalar incrementEps{1.0e-12};
	};
	template<typename Function, typename Scalar, int Dim>
	void BDFNewtonSolve(Ref<StateMatrix<Scalar, Dim>> state, const Function& f, Scalar x, const StateVector<Scalar,Dim>& midTerm, Scalar rightCoeff, Scalar T, Scalar tolerance = 1.0e-7,
		int maxIterations = 10);

	/**
	 * Backward differentiation linear multistep ODE solver.
	 * Important note : in the current version this should only be used for small Dimensions
	 * i.e Dim <= 4, a dynamic allocation version should be written for larger Dimensions.
	 */
	template<typename Function, typename Scalar, int Dim, int Order >
	class BDF
	{
	private:
		Scalar _T;
		StateMatrix<Scalar, Dim> _state;
		int _numStepsKnown{};

	public:
	  BDF(const BDF &) = delete;
	  const BDF &operator=(const BDF &) = delete;
	  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	  BDF()
	  {
		  _state = StateMatrix<Scalar, Dim>::Zero();
		}
		void SetSampleRate(Scalar sampleRate)
		{
			_T = Scalar(1.0) / sampleRate;
		}
		void SetInitConditions(const StateVector<Scalar,Dim>& init)
		{
			_state = StateMatrix<Scalar,Dim>::Zero();
			_state.col(1) = init;
			//Note setting the first column has no effect unless we take the current state before solving a step
			_state.col(0) = init;
			_numStepsKnown = 1;
		}
		void Step(const Function& f, Scalar x)
		{
			SubStep<Function, Scalar, Dim, Order>::StepSolve(_state, _numStepsKnown, f, _T, x);
		}
		StateVector<Scalar,Dim> CurrentState() const
		{
			return _state.col(0);
		}
		Ref<StateMatrix<Scalar,Dim>> FullState()
		{
			return _state;
		}
	};

	//Order 1 BDF ie implicit Euler method
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,1>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			//Solve  y[n] - y[n-1] = T * ( f(y[n] , x[n]) )
			BDFNewtonSolve<Function,Scalar,Dim>(state, f,x,
				-state.col(1),
				Scalar(1),
				T);
 
			//Note : always keep one more column in case the method is used as an initialization for a 
			//higher order method
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,2>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			if(numPastSteps < 2)
			{
				SubStep<Function, Scalar, Dim, 1>::StepSolve(state, numPastSteps, f, T, x);
				numPastSteps = 2;
			}
			//Solve  y[n] - 4/3*y[n-1] + 1/3*y[n-2] = 2/3 * T * ( f(y[n] , x[n]) )

			BDFNewtonSolve<Function, Scalar, Dim>(state, f, x,
				-Scalar(4) / 3  * state.col(1)
				+ Scalar(1) / 3 * state.col(2),
				Scalar(2) / 3,
				T);

			//Note : always keep one more column in case the method is used as an initialization for a 
			//higher order method
			state.col(3) = state.col(2);
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,3>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			if(numPastSteps < 3)
			{
				SubStep<Function, Scalar, Dim, 2>::StepSolve(state, numPastSteps, f, T, x);
				numPastSteps = 3;
			}

			BDFNewtonSolve<Function, Scalar, Dim>(state, f, x,
				-Scalar(18) / 11 * state.col(1)
				+ Scalar(9) / 11 * state.col(2)
				- Scalar(2) / 11 * state.col(3),
				Scalar(6) / 11,
				T);

			//Note : always keep one more column in case the method is used as an initialization for a 
			//higher order method
			state.col(4) = state.col(3);
			state.col(3) = state.col(2);
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,4>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			if(numPastSteps < 4)
			{
				SubStep<Function, Scalar, Dim, 3>::StepSolve(state, numPastSteps, f, T, x);
				numPastSteps = 4;
			}

			BDFNewtonSolve<Function, Scalar, Dim>(state, f, x,
				-Scalar(48) / 25 * state.col(1)
				+ Scalar(36) / 25 * state.col(2)
				- Scalar(16) / 25 * state.col(3)
				+ Scalar(3) / 25 * state.col(4),
				Scalar(12) / 25,
				T);

			//Note : always keep one more column in case the method is used as an initialization for a 
			//higher order method
			state.col(5) = state.col(4);
			state.col(4) = state.col(3);
			state.col(3) = state.col(2);
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,5>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			if(numPastSteps < 5)
			{
				SubStep<Function, Scalar, Dim, 4>::StepSolve(state, numPastSteps, f, T, x);
				numPastSteps = 5;
			}

			BDFNewtonSolve<Function, Scalar, Dim>(state, f, x,
				-Scalar(300) / 137 * state.col(1)
				+ Scalar(300) / 137 * state.col(2)
				- Scalar(200) / 137 * state.col(3)
				+ Scalar(75) / 137 * state.col(4)
				- Scalar(12) / 137 * state.col(5),
				Scalar(60) / 137,
				T);

			//Note : always keep one more column in case the method is used as an initialization for a 
			//higher order method
			state.col(6) = state.col(5);
			state.col(5) = state.col(4);
			state.col(4) = state.col(3);
			state.col(3) = state.col(2);
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	//Note this is the maximum stable order for BDF
	template<typename Function, typename Scalar, int Dim>
	class SubStep<Function,Scalar,Dim,6>
	{
	public:
		static void StepSolve(Ref<StateMatrix<Scalar, Dim>> state, int& numPastSteps, const Function& f, Scalar T, Scalar x)
		{
			if(numPastSteps < 6)
			{
				SubStep<Function, Scalar, Dim, 5>::StepSolve(state, numPastSteps, f, T, x);
				numPastSteps = 6;
			}

			BDFNewtonSolve<Function, Scalar, Dim>(state, f, x,
				-Scalar(360) / 147 * state.col(1)
				+ Scalar(450) / 147 * state.col(2)
				- Scalar(400) / 147 * state.col(3)
				+ Scalar(225) / 147 * state.col(4)
				- Scalar(72) / 147 * state.col(5)
				+ Scalar(10) / 147 * state.col(6),
				Scalar(60) / 147,
				T);

			state.col(7) = state.col(6);
			state.col(6) = state.col(5);
			state.col(5) = state.col(4);
			state.col(4) = state.col(3);
			state.col(3) = state.col(2);
			state.col(2) = state.col(1);
			state.col(1) = state.col(0);
		}
	};
	template<>
	struct BDFNewtonConstants<float>
	{
		static constexpr float incrementEps{ 1.0e-10f };
	};
	template<>
	struct BDFNewtonConstants<double>
	{
		static constexpr double incrementEps{ 1.0e-12 };
	};

	template<typename Function, typename Scalar, int Dim>
	void BDFNewtonSolve(Ref<StateMatrix<Scalar, Dim>> state,
		const Function& f,
		Scalar x,
		const StateVector<Scalar,Dim>& midTerm,
		Scalar rightCoeff,
		Scalar T,
		Scalar tolerance,
		int maxIterations)
	{
		//Solve Phi(Y) = Y + midTerm - T* rightCoeff * f(Y) = 0
		//where Y = the first column of state
		//and midterm is a function of the other columns

		//1 step euler for the initial guess
		StateVector<Scalar, Dim> y = state.col(1) + T * f.DyDt(state.col(1), x);
		StateVector<Scalar, Dim> phi = y + midTerm - T * rightCoeff * f.DyDt(y, x);

		int i = 0;

		while( i++ < maxIterations && phi.norm() > tolerance)
		{
			Matrix<Scalar, Dim, Dim> J = Matrix<Scalar, Dim, Dim>::Identity() - T * rightCoeff * f.Jacobian(y, x);
			//Solve J * (Y[n+1] - Y[n]) = - Phi(Y[n])
			StateVector<Scalar, Dim> increment = J.fullPivLu().solve(-phi);
			if (increment.norm() <= BDFNewtonConstants<Scalar>::incrementEps)
				break;
			//if(increment(0) != increment(0))
			//{
			//	int breakHere = 0;
			//}
			y = y + increment;
			phi = y + midTerm - T * rightCoeff * f.DyDt(y, x);
		}
		//Store solution
		state.col(0) = y;
	}
}
namespace ode
{
	/**
	 * \brief The classic Van der Pole 2d ODE
	 */
	template<typename Scalar>
	struct VanDerPoleODE
	{
		//Damping parameter
		Scalar _mu{ Scalar(0.1) };
		//Radian frequency ( 2* pi * f)
		Scalar _w{ Scalar(200 * 2 * PI) };

		explicit VanDerPoleODE(Scalar mu, Scalar w)
			: _mu(mu), _w(w)
		{
		}
		VanDerPoleODE() {};

		/**
		 * \brief 
		 * \param y current state
		 * \param x forcing input
		 * \return Derivative
		 */
        Matrix<Scalar, 2, 1> DyDt(const Matrix<Scalar, 2, 1> &y, Scalar x) const
        {
			Matrix<Scalar, 2, 1> deriv;
			deriv << y(1),
				_mu * (Scalar(1.0) - y(0)*y(0)) * y(1) * _w + _w * _w * (x - y(0));
			return deriv;
		}
        Matrix<Scalar, 2, 2> Jacobian(const Matrix<Scalar, 2, 1> &y, Scalar x) const
        {
			Matrix<Scalar, 2, 2> J;
			J << Scalar(0.0), Scalar(1.0),
				-Scalar(2.0)*_mu*y(0)*y(1)*_w - _w * _w, _mu * (Scalar(1.0) - y(0)*y(0)) *_w;
			return J;
		}
	};
}


#pragma once
#include "VanDerPoleODE.hpp"
#include "ode.hpp"
#include <algorithm>
#include "../tfdsp/sampleRate.hpp"

using namespace ode;

/**
 * \brief Van der Pole Oscillator
 * Note using double as float is too unstable for the way the ode is parameterised.
 * \tparam IntegrationOrder less than 5 is not recommended ( which leaves 5 or 6 as choices )
 */
template<typename Oversampler, int IntegrationOrder>
class VdpOscillator
{
private:
	VanDerPoleODE<double> _vdp{};
	BDF<VanDerPoleODE<double>, double, 2, IntegrationOrder> _integrator{};
	double _sampleRate{};
	StateVector<double,2> _initConditions;

	//More or less C8 on a piano, no point going higher and things start to blow up anyway
	static constexpr double maxAngularFreq = double(2 * PI * 4200);
	//Clamp the output to avoid blow ups
	static constexpr double maxOutput = 12.0;

	static constexpr double _initY0 = double(0.0);
	static constexpr double _initY1 = double(1.0);

	std::unique_ptr<Oversampler> _resamplerX;
	std::unique_ptr<Oversampler> _resamplerMu;
	std::unique_ptr<Oversampler> _resamplerW;

	static constexpr int ResamplingFactor{ Oversampler::ResamplingFactor };

	//tfdsp::PinkNoiseSource _noise{};

	float ModelStep(double x, double mu, double w)
	{
		_vdp._mu = std::max<double>(1.0e-8, mu);
		_vdp._w = std::max<double>(1.0e-4, std::min<double>(maxAngularFreq, w));
		_integrator.Step(_vdp, x);// +_noise.Step());

		//Clamp the state to avoid exploding if the ODE becomes unstable, typically when both mu and w are high.
		auto state = _integrator.FullState();
		for (int i = 0; i < 2; ++i)
		{
			state(0, i) = std::max<double>(std::min<double>(state(0, i), maxOutput), -maxOutput);
			auto derivMax = 2 * maxOutput * _sampleRate;
			state(1, i) = std::max<double>(std::min<double>(state(1, i), derivMax), -derivMax);
		}

		return _integrator.CurrentState()(0);
	}
public:

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	explicit VdpOscillator(std::function<Oversampler*()> resamplerCreator) : _resamplerX(resamplerCreator()),
		_resamplerMu(resamplerCreator()),
		_resamplerW(resamplerCreator())
	{
		_initConditions << _initY0, _initY1;
		_integrator.SetInitConditions(_initConditions);
	}

	void SetSampleRate(const double f0)
	{
		_sampleRate = f0 * ResamplingFactor;
		StateVector<double, 2> currentState = _integrator.CurrentState();
		_initConditions << currentState(0), currentState(1);
		_integrator.SetInitConditions(_initConditions);
		_integrator.SetSampleRate(_sampleRate);
	}

	float Step(double x, double mu, double w)
	{
		if (_sampleRate <= 0.)
		{
			throw std::runtime_error("Sample rate invalid or not initialized");
		}
		auto xA = _resamplerX->Upsample(x);
		auto muA = _resamplerMu->Upsample(mu);
		auto wA = _resamplerW->Upsample(w);

		Eigen::Matrix<double, ResamplingFactor, 1> output;

		for(int i=0; i < ResamplingFactor; ++i)
		{
			output(i) =  ModelStep(xA(i), muA(i), wA(i));
		}
		return _resamplerX->Downsample(output);

	}
};

//Fix linking errors for old versions of gcc
template<typename Oversampler, int IntegrationOrder>
constexpr double VdpOscillator<Oversampler, IntegrationOrder>::maxAngularFreq;
template<typename Oversampler, int IntegrationOrder>
constexpr double VdpOscillator<Oversampler, IntegrationOrder>::maxOutput;
template<typename Oversampler, int IntegrationOrder>
constexpr double VdpOscillator<Oversampler, IntegrationOrder>::_initY0;
template<typename Oversampler, int IntegrationOrder>
constexpr double VdpOscillator<Oversampler, IntegrationOrder>::_initY1;
template<typename Oversampler, int IntegrationOrder>
constexpr int VdpOscillator<Oversampler, IntegrationOrder>::ResamplingFactor;