#include "VanDerPol.h"

inline double VanDerPol::getNextPostion()
{
	return m_x + m_v * m_timeStep;
}
inline double VanDerPol::getNextVelocity()
{
	return m_v + (m_epsilon * (1 - m_x * m_x) * m_v - m_x) * m_timeStep;
}

VanDerPol::VanDerPol(double epsilon, double position, double velocity, double timeStep)
	:m_epsilon{ epsilon }, m_x{ position }, m_v{ velocity }, m_timeStep{ timeStep }
{
}

void VanDerPol::stepForward(int n)
{
	double nextPosition{};
	double nextVelocity{};

	for (int count{ 0 }; count < n; ++count)
	{
		nextPosition = getNextPostion();
		nextVelocity = getNextVelocity();
		// calculation of the next value needs the original value, so we'll assign it later.
		m_x = nextPosition;
		m_v = nextVelocity;
	}
}

double VanDerPol::getPosition() const
{
	return m_x;
}
double VanDerPol::getVelocity() const
{
	return m_v;
}