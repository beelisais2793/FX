#pragma once
#include <stdlib.h>
#include <glut.h>
#include <glm/glm.hpp>

class Boid
{

public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration = {0.0,0.0,0.0};

	int lifespan;
	float gravity[3] = { 0.0, 0.0, 0.0 };
	float radius=3;
	float mass = 10;
	bool active;
	GLfloat color[3] = { 0.7f,0.4f,0.3f };

	Boid();
	void init();
	void update(float h, float d, float * wind, char G);
	~Boid();
};

