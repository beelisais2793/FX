#pragma once
#include <glm/glm.hpp>
#include <stdio.h>
class Particle
{

public:
	glm::vec3 X;
	glm::vec3 V;
	glm::vec3 f;
	float mass=1;
	Particle() {
		V = { 0,0,0 };
		f = { 0,0,0 };
	};
	Particle(glm::vec3 X) : X(X) {
		V = { 0,0,0 };
		f = { 0,0,0 };
		
	};

	glm::vec3 acc() {
		return (f / mass);
	}

	/*Particle operator + (Particle const &obj) {
		Particle ans;
		ans.X = X + obj.X;
		ans.V = V + obj.V;
		return ans;
	}

	 Particle operator * (float const &obj) const {
		Particle ans;
		ans.X = X*obj;
		ans.V = V*obj;
		return ans;
	}*/

	 Particle add(Particle *p1, Particle *p2) {
		 Particle ans;
		 ans.X = p1->X + p2->X;
		 ans.V = p1->V + p2->V;
		 ans.f = p1->f + p2->f;
		 return ans;
	}

	 Particle multiply(Particle *p1, float h) {
		 Particle ans;
		 ans.X = p1->X * h;
		 ans.V = p1->V * h;
		 ans.f = p1->f * h;
		 return ans;
	 }
	Particle derivative(Particle particle) {
		Particle deriv;
		deriv.X = particle.V;
		deriv.V = particle.acc();
		deriv.f = { 0,0,0 };
		return deriv;
	}

	void eulerStep(float h) {

		*this = add((this),&multiply(&derivative(*(this)),h));

	}

	void RK2step(float h) {
		Particle K1, K2;
		K1 = derivative((*this));
		K2 = derivative(add(this, &multiply(&K1, 0.5*h)));
		*this = add(this, &multiply(&K2, h));
	}

	void RK4step(float h) {
		Particle K1, K2, K3, K4;
		K1 = derivative((*this));
		K2 = derivative(add(this, &multiply(&K1, 0.5*h)));
		K3 = derivative(add(this, &multiply(&K2, 0.5*h)));
		K4 = derivative(add(this, &multiply(&K3, h)));
		*this = add(this, &multiply(&K1, h/6));
		*this = add(this, &multiply(&K2, h / 3));
		*this = add(this, &multiply(&K3, h / 3));
		*this = add(this, &multiply(&K4, h / 6));
		f = { 0,0,0 };
	}
	~Particle();
	
};

