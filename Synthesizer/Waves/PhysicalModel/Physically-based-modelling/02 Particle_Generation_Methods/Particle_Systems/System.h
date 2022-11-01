#pragma once
#include "Particle.h"
#include <math.h>
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <time.h>
#include <random>
#include "geometry.h"

#define PI 3.14159265

class System
{
public:
	static const int n_particles = 15000;
	Particle *particle;
	int inactivecount = n_particles;
	int inactive[n_particles];
	int n_generate = 1;
	float newPos[n_particles][3];
	float timestepmain[n_particles], f;
	float d = 0.5; //air resistance constant
	float wind[3] = { 0.0,0.0,0.0 };
	float mu = 0.1; //friction constant
	float boxxl = -100;
	float boxxh = 100;
	float boxyl = -100;
	float boxyh = 100;
	float boxzl = -100;
	float boxzh = 100;
	char Gravity = 'y';
	float distance_from_center;
	float vnorm[3];
	Point origin, newpos[n_particles], oldpos[n_particles], face[3];
	float P_velocity;

	System();
	void clear();
	void GenerateParticlesPoint(float px, float py, float pz, float vx=0, float vy=0, float vz=0);
	void GenerateParticlesDirected(float px, float py, float pz, float vx, float vy, float vz);
	void GenerateParticlesDisc(float x, float y, float z, float rad);
	void GenerateParticlesRectangle();
	void TestDeactivate();
	void ComputeAccLennard(float sigma);
	void ComputeAccLine();
	void ComputeAccPlane();
	void integrate(float h, Point *wall, Point * unit_normal, int n_faces, float e);
	void SurfaceSampling(Point *wall, int n_faces);
	~System();
};

