#include "Particle.h"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

Particle::Particle(): m_x(0),m_y(0)
{
	m_direction = (2* M_PI * rand()) / RAND_MAX;
	m_speed = 0.001 * rand() / RAND_MAX;

}
void Particle::update() {
	double x_speed = m_speed * cos(m_direction);
	double y_speed = m_speed * sin(m_direction);

	m_x += x_speed;
	m_y += y_speed;
}
Particle::~Particle()
{
}
