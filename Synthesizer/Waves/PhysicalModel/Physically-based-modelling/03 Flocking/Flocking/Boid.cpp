#include "Boid.h"
#include <iostream>
#include <time.h>

Boid::Boid()
{
}


void Boid::update(float h, float d, float * wind, char G) {

		if (G == 'y' || G == 'Y') {
			gravity[0] = 0.0;
			gravity[1] = 0.0;
			gravity[2] = 0.0;
		}

			position += h*velocity;
			velocity += h*acceleration;

		lifespan--;
}


void Boid::init() {
	lifespan = 100000;
}
Boid::~Boid()
{
}
