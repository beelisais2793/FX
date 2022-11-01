#pragma once
#include "Particle.h"
class Edge
{
public:
	float length, restLength, actualDistance, k=100, d=10;
	Particle *p1, *p2;

	Edge(Particle *p1, Particle *p2) : p1(p1), p2(p2) {
		restLength = glm::length(p1->X - p2->X);
	};

	void applyConstraintForce() {
		glm::vec3 f;

		actualDistance = glm::length(p1->X - p2->X);
		f = k*(1 - restLength/ actualDistance)*(p1->X-p2->X);
		f += d*glm::dot((p1->V - p2->V), (p1->X - p2->X)) * (p1->X - p2->X) / (actualDistance*actualDistance);
		//printf("%f\n", actualDistance);
		p2->f += f; p1->f -= f;
	}
	~Edge();
};

