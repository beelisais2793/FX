#pragma once
#include "Boid.h"
#include <math.h>
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <time.h>
#include <random>
#include "geometry.h"

#define PI 3.14159265

class Flock
{
public:
	static const int n_Boids = 10000;
	Boid *boid;
	Boid leadBoid[2];
	int no_leads = 1;
	Boid repelBoid;
	int inactivecount = n_Boids;
	int inactive[n_Boids];
	int n_generate = 1;
	float newPos[n_Boids][3];
	float timestepmain[n_Boids], f;
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
	Point origin, newpos[n_Boids], oldpos[n_Boids], face[3];
	float P_velocity;

	Flock();
	void clear();
	void GenerateBoidsPoint();
	void GenerateBoidsDisc(float x, float y, float z, float rad);
	void GenerateBoidsRectangle();
	void TestDeactivate();
	void ComputeAccAlign();
	void ComputeAccCohesion();
	void ComputeAccSeparation();
	void integrate(float h, Point *wall, Point * unit_normal, int n_faces);
	void GenerateBoids(int nos);
	void FollowLeadParticle();
	void FollowLeadParticleForcely();
	void RepelBoid();
	void SphericalObstacle(float radius = 50.0, float safe_radius = 5.0, float threashold_time = 10.0, glm::vec3 center = { 30.0,0.0,0.0 });
	void LineStop(glm::vec3 p1,glm::vec3 p2);
	void Obstacles(std::vector<glm::vec3> vertices, int noVertices);
	//void SphericalObstacle(float radius, float safe_radius, float threashold_time, glm::vec3 center);

	~Flock();
};

