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
//		for (int i = 0; i < 3; i++) {
	//		acceleration[i] = gravity[i] - d / mass*velocity[i] + d*wind[i];
		//}
		for (int i = 0; i < 3; i++) {
			position[i] += h*velocity[i];
		}
		for (int i = 0; i < 3; i++) {
			velocity[i] += h*acceleration[i];
			//printf("%f\n", acceleration[0]);
		}
		lifespan--;
	
}
void Particle::init()
{
	lifespan = 1000;
	position[0] = 0; position[1] = 0; position[2] = 0;
	velocity[0] = 50.0*((float)rand()/RAND_MAX-0.5);
	velocity[1] = 50.0*((float)rand()/RAND_MAX-0.5);
	velocity[2] = 50.0*((float)rand()/RAND_MAX-0.5);
}
Particle::~Particle()
{
}
