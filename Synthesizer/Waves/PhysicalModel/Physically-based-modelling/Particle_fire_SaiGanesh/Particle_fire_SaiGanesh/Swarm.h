#pragma once

#include "Particle.h"

class Swarm
{
public:
	const static int NPARTICLES = 500;
private:
	Particle * m_pParticles;
public:
	Swarm();
	~Swarm();
	void update();
	const Particle * const getParticles() { return m_pParticles; };
};

