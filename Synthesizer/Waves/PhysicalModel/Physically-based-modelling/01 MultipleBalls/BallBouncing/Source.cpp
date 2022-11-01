#include <stdlib.h>

#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <ball.h>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <vector>
#include <Windows.h>

static const int NBALLS = 10;
Ball ball[NBALLS];

float normal[3];
float VNorm[3];
float Distance_btw_balls_old, Distance_btw_balls;
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

float h = 0.001; // Step size 
float d = 0.5; //air resistance constant
float wind[3] = { 0.0,0.0,0.0 };
float mu = 0.1; //friction constant
char Gravity;

clock_t initialTime = clock(), finalTime;
float timeTaken, timestepmain[NBALLS], f;
float newPos[NBALLS][3];
static const int FPS = 30;
const int tMAX = 10;

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

	/*
	8 vertices:
	glVertex3f(boxxl, boxyl, boxzl);
	glVertex3f(boxxh, boxyl, boxzl);
	glVertex3f(boxxh, boxyh, boxzl);
	glVertex3f(boxxl, boxyh, boxzl);
	glVertex3f(boxxl, boxyl, boxzh);
	glVertex3f(boxxh, boxyl, boxzh);
	glVertex3f(boxxh, boxyh, boxzh);
	glVertex3f(boxxl, boxyh, boxzh);
	*/

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
	glBegin(GL_LINE_STRIP);
	GLfloat lineColor[3] = { 0.0,0.3,0.0 };
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



	//draw the ball
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ball_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ball_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ball_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ball_shininess);

	GLfloat ballColor[] = {0.5, 0.2, 0.2};
	glMaterialfv(GL_FRONT, GL_AMBIENT, ballColor);

	for (int i = 0; i < NBALLS; i++) {
		glPushMatrix();
		glTranslatef(ball[i].getPos()[0], ball[i].getPos()[1], ball[i].getPos()[2]);
		glutSolidSphere(ball[i].radius, 20, 20);
		glPopMatrix();
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

	//initialize position and velocity of N particles
	for (int i = 0; i < NBALLS; i++) {
		ball[i].position[0] = (i-5)*15.0;
		//ball[i].position[1] = (i - 5)*15.0;
		ball[i].radius = 5;
	}
	
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

void IntegrateNextStep()
{
	
	
	//Find what would be the new position
	for (int j = 0; j < NBALLS; j++) {
		for (int i = 0; i < 3; i++) {
			newPos[j][i] = ball[j].getPos()[i] + h*ball[j].velocity[i];
		}
		timestepmain[j] = h;
	}
	
	float tangent_vector;
	//Checking whether the ball would collide in the next time step
	

		
	for (int k = 0; k < NBALLS; k++) {
		
		
		while (timestepmain[k] > 0) {

			if (newPos[k][0] > (boxxh - 10)) {

				f = ((boxxh - 10) - ball[k].getPos()[0]) / (newPos[k][0] - ball[k].getPos()[0]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[0] = -ball[k].velocity[0];
				tangent_vector = sqrt((ball[k].velocity[1] * ball[k].velocity[1]) + (ball[k].velocity[2] * ball[k].velocity[2]));
				if (tangent_vector != 0) {
					ball[k].velocity[1] = ball[k].velocity[1] - min(mu*abs(ball[k].velocity[0]), tangent_vector)*ball[k].velocity[1] / tangent_vector;
					ball[k].velocity[2] = ball[k].velocity[2] - min(mu*abs(ball[k].velocity[0]), tangent_vector)*ball[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][0] < (boxxl + 10)) {

				f = ((boxxl + 10) - ball[k].getPos()[0]) / (newPos[k][0] - ball[k].getPos()[0]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[0] = -ball[k].velocity[0];
				tangent_vector = sqrt((ball[k].velocity[1] * ball[k].velocity[1]) + (ball[k].velocity[2] * ball[k].velocity[2]));
				if (tangent_vector != 0) {
					ball[k].velocity[1] = ball[k].velocity[1] - min(mu*abs(ball[k].velocity[0]), tangent_vector)*ball[k].velocity[1] / tangent_vector;
					ball[k].velocity[2] = ball[k].velocity[2] - min(mu*abs(ball[k].velocity[0]), tangent_vector)*ball[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
				printf("%f ", timestepmain[1]);
			}
			else if (newPos[k][1] > (boxyh - 10)) {

				f = ((boxyh - 10) - ball[k].getPos()[1]) / (newPos[k][1] - ball[k].getPos()[1]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[1] = -ball[k].velocity[1];
				tangent_vector = sqrt((ball[k].velocity[0] * ball[k].velocity[0]) + (ball[k].velocity[2] * ball[k].velocity[2]));
				if (tangent_vector != 0) {
					ball[k].velocity[0] = ball[k].velocity[0] - min(mu*abs(ball[k].velocity[1]), tangent_vector)*ball[k].velocity[0] / tangent_vector;
					ball[k].velocity[2] = ball[k].velocity[2] - min(mu*abs(ball[k].velocity[1]), tangent_vector)*ball[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
				printf("%f ", timestepmain[1]);
			}
			else if (newPos[k][1] < (boxyl + 10)) {
				f = ((boxyl + 10) - ball[k].getPos()[1]) / (newPos[k][1] - ball[k].getPos()[1]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[1] = -ball[k].velocity[1];
				tangent_vector = sqrt((ball[k].velocity[0] * ball[k].velocity[0]) + (ball[k].velocity[2] * ball[k].velocity[2]));
				if (tangent_vector != 0) {
					ball[k].velocity[0] = ball[k].velocity[0] - min(mu*abs(ball[k].velocity[1]), tangent_vector)*ball[k].velocity[0] / tangent_vector;
					ball[k].velocity[2] = ball[k].velocity[2] - min(mu*abs(ball[k].velocity[1]), tangent_vector)*ball[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
				printf("%f ", timestepmain[1]);
			}
			else if (newPos[k][2] > (boxzh - 10)) {
				f = ((boxzh - 10) - ball[k].getPos()[2]) / (newPos[k][2] - ball[k].getPos()[2]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[2] = -ball[k].velocity[2];
				tangent_vector = sqrt((ball[k].velocity[0] * ball[k].velocity[0]) + (ball[k].velocity[1] * ball[k].velocity[1]));
				if (tangent_vector != 0) {
					ball[k].velocity[0] = ball[k].velocity[0] - min(mu*abs(ball[k].velocity[2]), tangent_vector)*ball[k].velocity[0] / tangent_vector;
					ball[k].velocity[1] = ball[k].velocity[1] - min(mu*abs(ball[k].velocity[2]), tangent_vector)*ball[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
				printf("%f ", timestepmain[1]);
			}
			else if (newPos[k][2] < (boxzl + 10)) {
				f = ((boxzl + 10) - ball[k].getPos()[2]) / (newPos[k][2] - ball[k].getPos()[2]);
				ball[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = ball[k].getPos()[i];
				}
				ball[k].velocity[2] = -ball[k].velocity[2];
				tangent_vector = sqrt((ball[k].velocity[0] * ball[k].velocity[0]) + (ball[k].velocity[1] * ball[k].velocity[1]));
				if (tangent_vector != 0) {
					ball[k].velocity[0] = ball[k].velocity[0] - min(mu*abs(ball[k].velocity[2]), tangent_vector)*ball[k].velocity[0] / tangent_vector;
					ball[k].velocity[1] = ball[k].velocity[1] - min(mu*abs(ball[k].velocity[2]), tangent_vector)*ball[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];

			}

			for (int l = k + 1; l < NBALLS; l++) {

				Distance_btw_balls = sqrt(pow((newPos[k][0] - newPos[l][0]), 2.0) + pow((newPos[k][1] - newPos[l][1]), 2.0) + pow((newPos[k][2] - newPos[l][2]), 2.0));

				if (Distance_btw_balls < (ball[k].radius + ball[l].radius)) {

					Distance_btw_balls_old = sqrt(pow((ball[k].position[0] - ball[l].position[0]), 2.0) + pow((ball[k].position[1] - ball[l].position[1]), 2.0) + pow((ball[k].position[2] - ball[l].position[2]), 2.0));
					f = (Distance_btw_balls_old - (ball[k].radius + ball[l].radius)) / (Distance_btw_balls_old - Distance_btw_balls);
					normal[0] = (ball[k].position[0] - ball[l].position[0]) / Distance_btw_balls_old;
					normal[1] = (ball[k].position[1] - ball[l].position[1]) / Distance_btw_balls_old;
					normal[2] = (ball[k].position[2] - ball[l].position[2]) / Distance_btw_balls_old;

					VNorm[0] = (ball[k].velocity[0] - ball[l].velocity[0])*normal[0] * normal[0];
					VNorm[1] = (ball[k].velocity[1] - ball[l].velocity[1])*normal[1] * normal[1];
					VNorm[2] = (ball[k].velocity[2] - ball[l].velocity[2])*normal[2] * normal[2];


					ball[k].update(f*h, d, wind, Gravity);
					ball[l].update(f*h, d, wind, Gravity);

					ball[k].velocity[0] = ball[k].velocity[0] - VNorm[0];
					ball[k].velocity[1] = ball[k].velocity[1] - VNorm[1];
					ball[k].velocity[2] = ball[k].velocity[2] - VNorm[2];

					ball[l].velocity[0] = ball[l].velocity[0] + VNorm[0];
					ball[l].velocity[1] = ball[l].velocity[1] + VNorm[1];
					ball[l].velocity[2] = ball[l].velocity[2] + VNorm[2];

					timestepmain[k] = (1 - f)*timestepmain[k];
				}
			}

			if (timestepmain[k]!=0) //if not just update the state to the next time step
			{

				ball[k].update(timestepmain[k], d, wind, Gravity);

				timestepmain[k] = 0;
			}

		}
	}//End of for loop

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


	for (float t = 0; t < 1.0/FPS; t += h) {
		IntegrateNextStep();
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
void VelocityControl(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN:
		ball[0].velocity[1] -= 100;
		break;
	case GLUT_KEY_UP:
		ball[0].velocity[1] += 100;
		break;
	case GLUT_KEY_LEFT:
		ball[0].velocity[0] -= 100;
		break;
	case GLUT_KEY_RIGHT:
		ball[0].velocity[0] += 100;
		break;
	case GLUT_KEY_PAGE_UP:
		ball[0].velocity[2] += 100;
		break;
	case GLUT_KEY_PAGE_DOWN:
		ball[0].velocity[2] -= 100;
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
	case 'w':
		wind[1] = 100.0;
		break;
	case 's':
		wind[1] = -100.0;
		break;
	case 'a':
		wind[0] = -100.0;
		break;
	case 'd': 
		wind[0] = 100.0;
	case 'i':
		wind[2] -= 1.0;
		break;
	case 'k':
		wind[2] += 1.0;
		break;
	case 'c':
		ball[0].velocity[0] += -ball[0].position[0];
		ball[0].velocity[1] += -ball[0].position[1];
		ball[0].velocity[2] += -ball[0].position[2];
		break;
	case 'o':
		wind[0] = -ball[0].position[0];
		wind[1] = -ball[0].position[1];
		wind[2] = -ball[0].position[2];
		break;
	case 32:
		ball[0].velocity[0] = 0;
		ball[0].velocity[1] = 0;
		ball[0].velocity[2] = 0;
		break;
	case 'g':
		ball[1].velocity[1] -= 100;
		break;
	case 't':
		ball[1].velocity[1] += 100;
		break;
	case 'f':
		ball[1].velocity[0] -= 100;
		break;
	case 'h':
		ball[1].velocity[0] += 100;
		break;
	}
}

void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		wind[1] = 0.0;
		break;
	case 's':
		wind[1] = 0.0;
		break;
	case 'a':
		wind[0] = 0.0;
		break;
	case 'd':
		wind[0] = 0.0;
	case 'i':
		wind[2] = 0.0;
		break;
	case 'k':
		wind[2] = 0.0;
		break;
	case 'o':
		wind[0] = 0;
		wind[1] = 0;
		wind[2] = 0;
		break;
	}
}

void GetInput()
{
	printf("Enter the value of Euler time step : ");
	scanf_s("%f", &h);
	printf("Do you want gravity? (y/n) : ");
	scanf_s(" %c", &Gravity);
	printf("Initial Position? (d for default) ");
	scanf_s("%f %f %f", &ball[0].position[0], &ball[0].position[1], &ball[0].position[2]);
	printf("Initial Velocity? (d for default) ");
	scanf_s("%f %f %f", &ball[0].velocity[0], &ball[0].velocity[1], &ball[0].velocity[2]);
	printf("Air resistance constant:");
	scanf_s(" %f", &d);
	printf("Friction constant :");
	scanf_s(" %f", &mu);
	system("CLS");
	printf("------------------Controls---------------------\n");
	printf("Press 'c' to accelerate towards the center\n");
}

int main(int argc, char** argv)
{
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
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(VelocityControl);
	glutKeyboardUpFunc(KeyboardUp);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);
	//GetInput();

	glutMainLoop();
	
	return 0;
}