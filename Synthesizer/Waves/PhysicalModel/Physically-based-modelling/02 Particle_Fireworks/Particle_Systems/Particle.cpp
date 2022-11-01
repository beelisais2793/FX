#include "Particle.h"
#include <iostream>
#include <time.h>

Particle::Particle()
{
}

float * Particle::getPos() {
	return position;
}

void Particle::update(float h, float d, float * wind, char G) {

		if (G == 'y' || G == 'Y') {
			gravity[0] = 0.0;
			gravity[1] = 0.0;
			gravity[2] = 0.0;
		}
		for (int i = 0; i < 3; i++) {
			position[i] += h*velocity[i];
		}
		for (int i = 0; i < 3; i++) {
			velocity[i] += h*acceleration[i];
		}
		lifespan--;
}

void Particle::init_two()
{
	repeat = 1;
	lifespan = 500;
}

void Particle::init() {
	repeat = 2;
	lifespan = 500;
}
Particle::~Particle()
{
}
