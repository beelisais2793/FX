#include "ball.h"
#include <iostream>


Ball::Ball()
{
}
float * Ball::getPos() {
	return position;
}

void Ball::update(float h, float d, float * wind, char G) {
	if (G == 'y' || G == 'Y') {
		gravity[0] = 0.0;
		gravity[1] = -10.0; 
		gravity[2] = 0.0; 
	}
	for (int i = 0; i < 3; i++) {
		acceleration[i] = gravity[i]-d/mass*velocity[i]+d*wind[i];
	}
	for (int i = 0; i < 3; i++) {
		position[i] += h*velocity[i];
	}
	for (int i = 0; i < 3; i++) {
		velocity[i] += h*acceleration[i];
	}

	
}
Ball::~Ball()
{
}
