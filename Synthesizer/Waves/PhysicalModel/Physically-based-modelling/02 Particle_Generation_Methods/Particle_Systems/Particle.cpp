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

void Particle::init()
{
	//color[0] = (float)rand() / RAND_MAX; color[1] = (float)rand() / RAND_MAX; color[2] = (float)rand() / RAND_MAX;
	color[0] = 0.5; color[1] = 0.5; color[2] = 0.8;
	lifespan = 2000;
}

Particle::~Particle()
{
}
