#pragma once
#include <stdlib.h>
#include <glut.h>
class Particle
{

public:
	float position[3] = {0.0, 0.0, 0.0};
	float velocity[3] = {0.0, 0.0, 0.0 };
	float acceleration[3] = {0.0, 0.0, 0.0 };
	int lifespan;
	float gravity[3] = { 0.0, 0.0, 0.0 };
	float radius=1;
	float mass = 10;
	bool active;
	GLfloat color[3];

	Particle();
	float * getPos();
	void init();
	void update(float h, float d, float * wind, char G);
	~Particle();
};

