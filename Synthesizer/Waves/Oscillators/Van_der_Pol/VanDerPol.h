#pragma once
 
class VanDerPol
{
private:
	// position and velocity
	double m_x{};
	double m_v{};

	// constants
	// parameter of the non-linear term
	double m_epsilon{};
	// time step
	double m_timeStep{};

	inline double getNextPostion();
	inline double getNextVelocity();

public:
	VanDerPol(double epsilon, double position = 1.0, double velocity = 1.0, double timeStep = 0.1);

	void stepForward(int n = 1);

	double getPosition() const;
	double getVelocity() const;
};