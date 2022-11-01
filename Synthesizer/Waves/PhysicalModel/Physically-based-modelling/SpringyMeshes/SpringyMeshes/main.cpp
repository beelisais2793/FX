#include <iostream>
#include "Mesh.h"
#include <stdlib.h>

Mesh mesh;

float h = 0.01; //step size
static const int FPS = 30;

glm::vec3 center_ball = { 50,50,-55 };
glm::vec3 e1 = { -50,50,-10 }, e2{ -40,40,10 };
float ball_radius = 50;

void drawEdge() {
	glLineWidth(3.0);
	GLfloat lineColor[3] = { 1.0,1.0,1.0 };
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);

	glVertex3f(e1[0], e1[1], e1[2]);
	glVertex3f(e2[0], e2[1], e2[2]);
		
	
	glEnd();
}
void init(void)
{
		glClearColor(0.0, 0.0, 0.0, 0.0);
		// Enable Z-buffering, backface culling, and lighting
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);

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

void drawBall() {
	glPushMatrix();
	GLfloat ballColor[3] = { 0.5,0.5,0.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ballColor);
	glTranslatef(center_ball[0], center_ball[1], center_ball[2]);
	glutSolidSphere(ball_radius - 0.1, 40, 40);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glTranslatef(-6.5, 9, -9.0);
	glRotatef(20,0,20,0);

	//drawBall();
	drawEdge();
	mesh.drawEdges();
	mesh.drawParticles();
	

	
	glPopMatrix();
	glutSwapBuffers();
}

void idle(void)
{
	for (float t = 0; t < 1.0 / FPS; t += h) {
		//mesh.collisionResponseBall(center_ball,ball_radius);
		//mesh.FaceCollision();
		mesh.edgeCollision(e1,e2);
		mesh.nextTimeStep(h);
		center_ball[2] += 0.1;
	}
	glutPostRedisplay();
}

void timer(int v) {
	glutTimerFunc(1000 / FPS, timer, v);
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

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             // ESCAPE key
		exit(0);
		break;

	case 'w':
		mesh.particles[4].X[1] += 1.0;
		break;
	case 's':
		mesh.particles[4].X[1] -= 1.0;
		break;
	case 'a':
		mesh.particles[4].X[0] -= 1.0;
		break;
	case 'd':
		mesh.particles[4].X[0] += 1.0;
		break;

	}
}

int main(int argc, char** argv) {
	
	mesh.MakeMesh();
	//glm::vec3 wind = { 0,-1000,0 };
	//glm::vec3 wind1 = { 0,1,0 };
	//mesh.WindForce(wind);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Spring mesh demo");
	init();
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}
