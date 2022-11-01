#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include<ctime>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.hpp"
#include "Particle.h"
#include "System.h"
#include "geometry.h"


System ball_system;

using namespace std;

double boxxl, boxxh, boxyl, boxyh, boxzl, boxzh;  // The low and high x, y, z values for the box sides

int rotateon;

double xmin, xmax, ymin, ymax, zmin, zmax;
double maxdiff;

int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

float timeSinceLast = 0;
int frameTime = 0;

float h = 0.01; // Step size 

char Gravity;
float timeTaken;

clock_t initialTime = clock(), finalTime;

static const int FPS = 30;
const int tMAX = 10;

static const int N_faces = 50;
Point wall[N_faces*3], unit_normal[N_faces*3]; 
Plane P;
int n_faces;

void display(void)
{

	GLfloat box_ambient[] = { 0.1, 0.1, 0.1 };
	GLfloat smallr00[] = { 0.0, 0.0, 0.0 };
	GLfloat small0g0[] = { 0.0, 0.075, 0.0 };
	GLfloat small00b[] = { 0.0, 0.0, 0.1 };
	GLfloat smallrg0[] = { 0.1, 0.1, 0.0 };
	GLfloat smallr0b[] = { 0.1, 0.0, 0.1 };
	GLfloat small0gb[] = { 0.0, 0.1, 0.1 };
	GLfloat smallrgb[] = { 0.1, 0.1, 0.1 };

	GLfloat box_diffuse[] = { 0.7, 0.7, 0.7 };
	GLfloat box_specular[] = { 0.1, 0.1, 0.1 };
	GLfloat box_shininess[] = { 0.0 };
	GLfloat ball_ambient[] = { 0.2, 0.1, 0.0 };
	GLfloat ball_diffuse[] = { 0.1, 0.05, 0.0 };
	GLfloat ball_specular[] = { 0.3, 0.3, 0.3 };
	GLfloat ball_shininess[] = { 10.0 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	//rotate the view
	glRotatef(spinup, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 1.0, 0.0);

	//Draw the box
	//set material parameters
	glMaterialfv(GL_FRONT, GL_AMBIENT, box_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, box_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, box_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, box_shininess);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	//glColor4f(0.0, 0.0, 1.0, 1.0);

	boxxl = -100.5;
	boxxh = 100.5;
	boxyl = -100.5;
	boxyh = 100.5;
	boxzl = -100.5;
	boxzh = 100.5;

	/*
	glBegin(GL_QUADS);
	//back face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxl, boxyh, boxzl);

	//left face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxl, boxyh, boxzh);

	//right face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxh, boxyl, boxzl);

	//bottom face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxl, boxyl, boxzh);

	//top face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxh, boxyh, boxzl);

	//front face
	glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	glVertex3f(boxxh, boxyh, boxzh);

	glEnd();
	*/
	glDisable(GL_BLEND);

	//wall
	/*glBegin(GL_POLYGON);
	glColor3f(0.0,0.0,0.5);
	glVertex3f(wall[0].x, wall[0].y, wall[0].z);
	glVertex3f(wall[1].x, wall[1].y, wall[1].z);
	glVertex3f(wall[2].x, wall[2].y, wall[2].z);
	glVertex3f(wall[2].x, wall[2].y, wall[2].z);
	glEnd();
	*/
	boxxl = -100;
	boxxh = 100;
	boxyl = -100;
	boxyh = 100;
	boxzl = -100;
	boxzh = 100;

	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.0,0.3,0.0 };
	/*
	glBegin(GL_LINE_STRIP);

	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxl, boxyl, boxzl);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxl, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxh, boxyl, boxzh);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxh, boxyl, boxzh);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxh, boxyh, boxzh);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxh, boxyl, boxzh);
	glEnd();
	*/

	for (int i = 0; i < n_faces; i++) {
	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(wall[3*i].x, wall[3*i].y, wall[3*i].z);
	glVertex3f(wall[3*i + 1].x, wall[3*i + 1].y, wall[3*i+1].z);
	glVertex3f(wall[3*i + 2].x, wall[3*i + 2].y, wall[3*i + 2].z);
	glVertex3f(wall[3*i].x, wall[3*i].y, wall[3*i].z);
	glEnd();

	}
	//draw the ball
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ball_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ball_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ball_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ball_shininess);

	GLfloat ballColor[] = { 0.5, 0.2, 0.2 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ballColor);

	for (int i = 0; i < ball_system.n_particles; i++) {

		if (ball_system.particle[i].active == true) {
			glPushMatrix();
			glTranslatef(ball_system.particle[i].position[0], ball_system.particle[i].position[1], ball_system.particle[i].position[2]);
			glutSolidSphere(ball_system.particle[i].radius, 20, 20);
			glPopMatrix();
		}
	}

	glPopMatrix();
	glutSwapBuffers();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// Enable Z-buffering, backface culling, and lighting
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_BLEND);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1, 600);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set eye point and lookat point
	gluLookAt(0, 225, 300, 0, 0, 0, 0, 1, 0);

	// Set up lights
	GLfloat light0color[] = { 0.5, 0.5, 0.5 };
	GLfloat light0pos[] = { 500, 500, 300 };
	GLfloat light1color[] = { 0.5, 0.5, 0.5 };
	GLfloat light1pos[] = { 1000, 1000, 1000 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0color);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0color);
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1color);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1color);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1color);

	//Initialize box boundaries
	boxxl = -100;
	boxxh = 100;
	boxyl = -100;
	boxyh = 100;
	boxzl = -100;
	boxzh = 100;

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec3 > normals;
	bool res = loadOBJfacenormal("dodecahedron.obj", vertices, normals);
	Point point;
	for (int i = 0; i < vertices.size(); i++)
	{	//printf("%f %f %f \n ", vertices[i].x, vertices[i].y, vertices[i].z);
		//printf("%d  ", vertices.size());
		point= Point (vertices[i].x * 100, vertices[i].y * 100, vertices[i].z * 100);
		wall[i] = point;
	}
	for (int i = 0; i < normals.size(); i++) {
		point = Point(normals[i].x, normals[i].y, normals[i].z);
		unit_normal[i] = point;
	}
	n_faces = normals.size();
		//Point p1(0, 30.0, 0.0), p2(-30.0, -30.0, -30.0), p3(30.0, -30.0, -30.0);
		//Point p1(0, 100.0, 100.0), p2(-100.0, -100.0, 100.0), p3(100.0, 0.0, 100.0);
		//wall[0] = p1; wall[1] = p2; wall[2] = p3;
	//P = Plane(p1, p2, p3);
}

void reshapeFunc(GLint newWidth, GLint newHeight)
{
	if (newWidth > newHeight) // Keep a square viewport
		glViewport((newWidth - newHeight) / 2, 0, newHeight, newHeight);
	else
		glViewport(0, (newHeight - newWidth) / 2, newWidth, newWidth);
	init();
	glutPostRedisplay();
}


void idle(void)
{

	if (rotateon) {
		spin += xchange / 250.0;
		if (spin >= 360.0) spin -= 360.0;
		if (spin < 0.0) spin += 360.0;
		spinup -= ychange / 250.0;
		if (spinup > 89.0) spinup = 89.0;
		if (spinup < -89.0) spinup = -89.0;
	}


	for (float t = 0; t < 1.0 / FPS; t += h) {
		ball_system.GenerateParticlesPoint();
		ball_system.TestDeactivate();
		//ball_system.ComputeAcc();
		ball_system.integrate(h, wall, unit_normal, n_faces);
	}

	finalTime = clock();
	timeTaken = (float)((finalTime - initialTime)) / CLOCKS_PER_SEC;
	initialTime = finalTime;
	timeSinceLast += timeTaken;
	while ((timeSinceLast + (float)(clock() - initialTime) / CLOCKS_PER_SEC)  < 1.0 / FPS);

	timeSinceLast = 0;
	glutPostRedisplay();
}

void timer(int v) {
	glutTimerFunc(1000 / FPS, timer, v);
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			lastx = x;
			lasty = y;
			xchange = 0;
			ychange = 0;
			rotateon = 1;
		}
		else if (state == GLUT_UP) {
			xchange = 0;
			ychange = 0;
			rotateon = 0;
		}
		break;

	default:
		break;
	}
}

void motion(int x, int y)
{
	xchange = x - lastx;
	ychange = y - lasty;
}


int main(int argc, char** argv)
{
	srand(time(NULL));
	
	GLint SubMenu1, SubMenu2, SubMenu3, SubMenu4;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Ball in Cube Demo");
	init();
	rotateon = 0;

	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}