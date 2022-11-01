class VanDerPol
{
private:
	// position and velocity
	double m_x{};
	double m_v{};
    double t = 0;
    double inc = 440/sampleRate;
	// constants
	// parameter of the non-linear term
	double m_epsilon{};
	// time step
	double m_timeStep{};

	inline double getNextPostion();
	inline double getNextVelocity();

    double Tick() {
        double p = getNextPosition();
        double v = getNextVelocity();
        return sin(2*M_PI*(t+p+v));
    }
public:
	VanDerPol(double epsilon, double position = 1.0, double velocity = 1.0, double timeStep = 0.1);

	void stepForward(int n = 1);

	double getPosition() const;
	double getVelocity() const;
};

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
