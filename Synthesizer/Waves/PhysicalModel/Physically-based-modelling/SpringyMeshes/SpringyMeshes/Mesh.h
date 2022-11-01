#pragma once
#include "Particle.h"
#include "Edge.h"
#include <vector>
#include <glut.h>
#include <omp.h>
#include "geometry.h"

class Mesh
{
	
public:
	std::vector<Particle> particles;
	std::vector<Edge> edges;

	void insertEdge(Particle *p1, Particle *p2) { edges.push_back(Edge(p1,p2)); }

	void MakeMesh() {
		glm::vec3 p1 = { -100.0f,-150.0f,0.0f }, p2 = { 0.0f,0.0f,0.0f }, p3 = { 50,-150,0 };
		particles.push_back(Particle(p1)); particles.push_back(Particle(p2)); particles.push_back(Particle(p3));
		particles[0].V = { 0,0.5,0 };
		
		insertEdge(&particles[0], &particles[1]);
		insertEdge(&particles[1], &particles[2]);
		insertEdge(&particles[0], &particles[2]);
	
	}

	void makeRectanglulargrid(float width,float height, int no_width, int no_height) {

		particles.resize(no_width*no_height);
		for (int x = 0; x<no_width; x++)
		{
			for (int y = 0; y<no_height; y++)
			{
				glm::vec3 pos = { width * (x / (float)no_width)-100,-20,-height * (y / (float)no_height)+100 };
				particles[y*no_width + x] = Particle(pos);
			}
		}
		for (int x = 0; x < no_width; x++)
		{
for (int y = 0; y < no_height; y++)
{
	//One neighbour
	if (x < no_width - 1) insertEdge(&particles[y*no_width + x], &particles[y*no_width + x + 1]);
	if (y < no_height - 1) insertEdge(&particles[y*no_width + x], &particles[(y + 1)*no_width + x]);
	if ((x < no_width - 1) && (y < no_height - 1)) {
		insertEdge(&particles[y*no_width + x], &particles[(y + 1)*no_width + x + 1]);
		insertEdge(&particles[(y + 1)*no_width + x], &particles[y*no_width + x + 1]);
	}

	//Two neighbour
	if (x < no_width - 2) insertEdge(&particles[y*no_width + x], &particles[y*no_width + x + 2]);
	if (y < no_height - 2) insertEdge(&particles[y*no_width + x], &particles[(y + 2)*no_width + x]);
	if ((x < no_width - 2) && (y < no_height - 2)) {
		insertEdge(&particles[y*no_width + x], &particles[(y + 2)*no_width + x + 2]);
		insertEdge(&particles[(y + 2)*no_width + x], &particles[y*no_width + x + 2]);
	}
}
		}
		//particles[1].V = { 0,0,10 };
	}

	void WindForce(glm::vec3 wind) {
		for (int i = 0; i < particles.size(); i++) {
			particles[i].f = particles[i].f + wind;
		}
	}

	void nextTimeStep(float h) {

		for (int i = 0; i < edges.size(); i++) {
			if (i != 4)  edges[i].applyConstraintForce();
			// if (i == 2) printf("%f %f %f %f  %f \n", edges[i].p1->X[0], edges[i].p1->X[1], edges[i].p2->X[0], edges[i].p2->X[1], (edges[i].p2->X - edges[i].p1->X).length());
		}

#pragma omp parallel for
		for (int i = 0; i < particles.size(); i++) {
			particles[i].RK4step(h);
		}

		//particles[0].X = { -100,100,0 };
		//particles[49].X = { 100,100,0 };
	}

	void drawEdges() {
		glLineWidth(3.0);
		GLfloat lineColor[3] = { 1.0,1.0,1.0 };
		glBegin(GL_LINES);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);

		for (int i = 0; i < edges.size(); i++) {
			glVertex3f(edges[i].p1->X[0], edges[i].p1->X[1], edges[i].p1->X[2]);
			glVertex3f(edges[i].p2->X[0], edges[i].p2->X[1], edges[i].p2->X[2]);
		}
		glEnd();
	}

	void drawParticles() {
		GLfloat particleColor[3] = { 0.0,1.0,1.0 };
		for (int i = 0; i < particles.size(); i++) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, particleColor);
			glPushMatrix();
			glTranslatef(particles[i].X[0], particles[i].X[1], particles[i].X[2]);
			glutSolidSphere(2, 20, 20);
			glPopMatrix();
		}
	}

	void collisionResponseBall(glm::vec3 center, float radius) {
		for (int i = 0; i < particles.size(); i++) {
			glm::vec3 pos = particles[i].X - center;
			float length = glm::length(pos);
			if (length < radius) {
				particles[i].X += pos / length*(radius - length);
			}
		}
	}

	void FaceCollision() {
		for (int i = 0; i < particles.size(); i++) {

			Point p1, p[3];
			p[0].x = 0; p[0].y = 0; p[0].z = -50;
			p[1].x = 0; p[1].y = 0; p[1].z = 50;
			p[2].x = 50; p[2].y = 0; p[2].z = 0;
			p1.x = particles[i].X[0]; p1.y = particles[i].X[1]; p1.z = particles[i].X[2];
			if (isInside_yz(p, 3, p1))
				if (particles[i].X[1] < -25)
					particles[i].X[1] = -25;
		}
	}

	void edgeCollision(glm::vec3 e1, glm::vec3 e2) {
		
		glm::vec3 b = (e1 - e2) / glm::length(e1 - e2);
		for (int i = 0; i < edges.size(); i++) {
			glm::vec3 normal = glm::cross((e1 - e2), (edges[i].p1->X - edges[i].p2->X)) / (glm::length(e1 - e2)*glm::length(edges[i].p1->X - edges[i].p2->X));
			glm::vec3 r = e1 - edges[i].p1->X;
			glm::vec3 a = (edges[i].p1->X - edges[i].p2->X) / glm::length((edges[i].p1->X - edges[i].p2->X));
			float s = glm::dot(r, glm::cross(b, normal)) / glm::dot(a, glm::cross(b, normal));
			float t = -glm::dot(r, glm::cross(a, normal)) / glm::dot(b, glm::cross(a, normal));
			glm::vec3 pa, qa, m;
			pa = edges[i].p2->X + (edges[i].p1->X - edges[i].p2->X)*t;
			qa = e2 + (e1 - e2)*s;
			m = pa - qa;
			//printf("%f\n", glm::length(m));
			if (glm::length(m) < 700) {
				glm::vec3 impact = { 0, -0.5, 0 };
				edges[i].p1->V += impact;
				edges[i].p2->V += impact;
			}
		}
	}
	Mesh();
	~Mesh();
};

