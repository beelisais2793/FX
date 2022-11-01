#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.hpp"
#include "Boid.h"
#include "Flock.h"
#include "geometry.h"

Flock flock;

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
Point wall[N_faces*3], unit_normal[N_faces]; 
Plane P;
int n_faces;
bool trigger = false;


int obstacleRadius = 20, noVertices = 6;
glm::vec3 unit_velocity;
std::vector<glm::vec3> obstacleVertices;

void SpiralObstacle() {
	
	glm::vec3 newVertex = {0,5,0}, addV;
	obstacleVertices.push_back(newVertex);
	newVertex = { 0,150,0 };
	obstacleVertices.push_back(newVertex);
	newVertex = { -150,150,0 };
	obstacleVertices.push_back(newVertex);
	newVertex = { -150,-150,0 };
	obstacleVertices.push_back(newVertex);
	newVertex = { 0,-150,0 };
	obstacleVertices.push_back(newVertex);
	newVertex = { 0,-5,0 };
	obstacleVertices.push_back(newVertex);
	/*
	float length = 20;
	for (int i = 0; i < noVertices; i++) {
		obstacleVertices.push_back(newVertex);
		if (i % 4 == 0)
			addV = { length,0,0 };
		else if ((i+1)%4 ==0)
			addV = { 0,length,0 };
		else if ((i + 2) % 4 == 0)
			addV = { -length,0,0 };
		else if ((i + 3) % 4 == 0)
			addV = { 0,-length,0 };
		newVertex += (float)(i+1)*addV;
	}
	*/
}

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

void DrawStripObstacle() {
	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.0,0.5,0.0 };
	for (int i = 0; i < noVertices-1; i++) {
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
		glVertex3f(obstacleVertices[i].x, obstacleVertices[i].y, obstacleVertices[i].z);
		glVertex3f(obstacleVertices[i+1].x, obstacleVertices[i+1].y, obstacleVertices[i+1].z);
		glEnd();
	}
}
void DrawBoundingBox() {

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

	glDisable(GL_BLEND);

	boxxl = -100;
	boxxh = 100;
	boxyl = -100;
	boxyh = 100;
	boxzl = -100;
	boxzh = 100;

	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.0,0.3,0.0 };
	
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
}

void DrawBall() {
	//draw the ball
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ball_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ball_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ball_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ball_shininess);

	for (int i = 0; i < flock.n_Boids; i++) {

		if (flock.boid[i].active == true) {
			
			glMaterialfv(GL_FRONT, GL_AMBIENT, flock.boid[i].color);
			glPushMatrix();
			glTranslatef(flock.boid[i].position[0], flock.boid[i].position[1], flock.boid[i].position[2]);
			glutSolidSphere(flock.boid[i].radius, 20, 20);
			glPopMatrix();
		}
	}

	
	//Display Lead
	for (int k = 0; k < flock.no_leads; k++) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, flock.leadBoid[0].color);
		glPushMatrix();
		glTranslatef(flock.leadBoid[k].position.x, flock.leadBoid[k].position.y, flock.leadBoid[k].position.z);
		glutSolidSphere(flock.leadBoid[0].radius, 20, 20);
		glPopMatrix();
	}
	/*
	//Display Repel Boid
	glMaterialfv(GL_FRONT, GL_AMBIENT, flock.leadBoid.color);
	glPushMatrix();
	glTranslatef(flock.repelBoid.position.x, flock.repelBoid.position.y, flock.repelBoid.position.z);
	glutSolidSphere(flock.leadBoid.radius, 20, 20);
	glPopMatrix();
	*/
}

void DrawOBJ() {

	glMaterialfv(GL_FRONT, GL_AMBIENT, box_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, box_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, box_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, box_shininess);

	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.0,0.3,0.0 };
	for (int i = 0; i < n_faces; i++) {
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
		glVertex3f(wall[3 * i].x, wall[3 * i].y, wall[3 * i].z);
		glVertex3f(wall[3 * i + 1].x, wall[3 * i + 1].y, wall[3 * i + 1].z);
		glVertex3f(wall[3 * i + 2].x, wall[3 * i + 2].y, wall[3 * i + 2].z);
		glVertex3f(wall[3 * i].x, wall[3 * i].y, wall[3 * i].z);
		glEnd();

	}
}

void DrawWall() {

	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.0,0.3,0.0 };
	glBegin(GL_LINE_STRIP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	glVertex3f(wall[0].x, wall[0].y, wall[0].z);
	glVertex3f(wall[1].x, wall[1].y, wall[1].z);
	glVertex3f(wall[2].x, wall[2].y, wall[2].z);
	glVertex3f(wall[0].x, wall[0].y, wall[0].z);
	glEnd();
	
}

void DrawVelocity(float arrow_size) {

	GLfloat lineColor[3] = { 0.5,0.2,0.2 };
	float width = 2;
	for (int i = 0; i < flock.n_Boids; i++) {

		if (flock.boid[i].active == true) {
			glBegin(GL_LINE_STRIP);
			glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
			unit_velocity = flock.boid[i].velocity / glm::length(flock.boid[i].velocity);
			glVertex3f(flock.boid[i].position.x, flock.boid[i].position.y, flock.boid[i].position.z);
			glVertex3f(flock.boid[i].position.x + unit_velocity.x*arrow_size, flock.boid[i].position.y + unit_velocity.y*arrow_size, flock.boid[i].position.z + unit_velocity.z*arrow_size);
			//glVertex3f(flock.boid[i].position.x + width, flock.boid[i].position.y-unit_velocity.y/unit_velocity.x*width, flock.boid[i].position.z);
			//glVertex3f(flock.boid[i].position.x, flock.boid[i].position.y, flock.boid[i].position.z);
			glEnd();

		}
	}
}

void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glLineWidth(3.0);
	GLfloat lineColor[3] = { 0.4,0.3,0.4 };
	glBegin(GL_LINE_LOOP);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex3f(x + cx, y + cy,0);//output vertex

	}
	glEnd();
}

void readOBJ() {
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec3 > normals;
	bool res = loadOBJfacenormal("dodecahedron.obj", vertices, normals);
	Point point;
	for (int i = 0; i < vertices.size(); i++)
	{
		point = Point(vertices[i].x * 100, vertices[i].y * 100, vertices[i].z * 100);
		wall[i] = point;
	}
	for (int i = 0; i < normals.size(); i++) {
		point = Point(normals[i].x, normals[i].y, normals[i].z);
		unit_normal[i] = point;
	}
	n_faces = normals.size();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	//rotate the view
	glRotatef(spinup, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 1.0, 0.0);

	//DrawBoundingBox();

	DrawBall();
	DrawVelocity(flock.leadBoid[0].radius*2);
	DrawStripObstacle();
	//DrawOBJ();
	//DrawWall();
	/*
	for (int i = 0; i<3; i++)
		for (int j = 0; j < 3; j++)
		{
			DrawCircle(-50 + i * 50, -50 + j * 50, obstacleRadius, 30);
		}
		*/
	glPopMatrix();
	glutSwapBuffers();
}

void initializeWall() {

	//Point p1(-100.0, -40.0, -50.0), p2(100.0, -40.0, -50.0), p3(0.0, -50.0, 5.0);
	Point p1(100, -80.0, -50.0), p3(-100.0, -80.0, -50.0), p2(0.0, -80.0, 60.0);
	wall[0] = p1; wall[1] = p2; wall[2] = p3;
	P = Plane(p1, p2, p3);
	n_faces = 1;
	Point normal(P.a, P.b, P.c);
	unit_normal[0] = normal;
}

void init(void)
{
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
		gluLookAt(0, 0, 300, 0, 0, 0, 0, 1, 0);

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
	}

	SpiralObstacle();
	//readOBJ();
	//initializeWall();
	
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

glm::vec3 p1 = { -100,0,0 }, p2 = { 0,0,0 };
int count1 = 0;
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

		if(count1==0)
			flock.GenerateBoids(10);
		count1=1;

		flock.TestDeactivate();
		//flock.LineStop(p1,p2);
		flock.Obstacles(obstacleVertices, noVertices);
		flock.ComputeAccSeparation();
		flock.ComputeAccAlign();
		flock.ComputeAccCohesion();

		if (trigger == true) {
			//flock.FollowLeadParticle();
			//trigger = false;
		}


		flock.FollowLeadParticleForcely();
		//flock.RepelBoid();
		//for(int i=0;i<3;i++)
			//for (int j = 0; j < 3; j++)
			//{
				//glm::vec3 center = {-50+i*50,-50+j*50,0};
				//flock.SphericalObstacle(obstacleRadius, 5 , 10 , center);
			//}
		flock.integrate(h, wall, unit_normal, n_faces);
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

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             // ESCAPE key
		exit(0);
		break;

	case 32:
		trigger = true;
		break;

	case 'w':
		flock.leadBoid[0].position.y+=2;
		break;
	case 's':
		flock.leadBoid[0].position.y-=2;
		break;
	case 'a':
		flock.leadBoid[0].position.x-=2;
		break;
	case 'd':
		flock.leadBoid[0].position.x+=2;
		break;
	case 'i':
		flock.repelBoid.position.y += 2;
		break;
	case 'k':
		flock.repelBoid.position.y -= 2;
		break;
	case 'j':
		flock.repelBoid.position.x -= 2;
		break;
	case 'l':
		flock.repelBoid.position.x += 2;
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
	glutCreateWindow("Flocking demo");
	init();
	rotateon = 0;
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}