#include "Body.h"
#include <stdio.h>
#include <glut.h>
#include <omp.h>
#include "objloader.hpp"

using namespace std;

float h = 0.01; //step size
static const int FPS = 30;
int rotateon;
int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

Body body("cuboid");
std::vector<glm::vec3> orientedVertices;

std::vector <glm::vec3> Vert;
std::vector<std::vector<int>> indices;
bool bounding = true;
glm::vec3 collisionPoint;

Body body1("cuboid"), body2("cuboid");

void ObjectCollision(Body *b1, Body *b2, float h) {
		
	Body nextState = *b1;

	bool collide;
	nextState.RK4step(h);

	std::vector<glm::vec3> V1 = b1->getOrientatedVertices();
	std::vector<glm::vec3> V2 = nextState.getOrientatedVertices();
	glm::vec3 normal, delP, delL;

	for (int i = 0; i < V1.size(); i++) {

		for (int j = 0; j < b2->faces.size(); j++) {

			std::tie(collide, normal) = b1->isCollidingFace(V1[i], V2[i], b2->getOrientatedVertices(), b2->faces[j]);


			if (collide) {
				std::tie(delP, delL) = b1->collisionResponse(V1[i]-b1->X, normal);

				b1->p += delP / 0.30f; b1->L += delL / 2.0f;
				b2->p -= delP / 0.30f; b2->L -= delL / 2.0f;
				return;
			}
		}
	}

	nextState = *b2;

	nextState.RK4step(h);

	V1 = b2->getOrientatedVertices();
	V2 = nextState.getOrientatedVertices();

	for (int i = 0; i < V1.size(); i++) {

		for (int j = 0; j < b1->faces.size(); j++) {

			std::tie(collide, normal) = b2->isCollidingFace(V1[i], V2[i], b1->getOrientatedVertices(), b1->faces[j]);


			if (collide) {
				std::tie(delP, delL) = b2->collisionResponse(V1[i]-b2->X, normal);

				b2->p += delP / .30f; b2->L += delL / 2.0f;
				b1->p -= delP / .30f; b1->L -= delL / 2.0f;
				return;
			}
		}
	}

	//b1->CollisionTest(b2->vertices, b2->faces, h);
	//b2->CollisionTest(b1->vertices, b1->faces, h);
}

void EdgeCollision(Body *b1, Body *b2, float h) {
	glm::vec3 delP, delL;
#pragma omp parallel for
	for (int i = 0; i < b1->edge.size(); i++) {
		for (int j = 0; j < b2->edge.size(); j++) {

			glm::vec3 a = (b1->getOrientatedVertices()[b1->edge[i][0]] - b1->getOrientatedVertices()[b1->edge[i][1]]);
			glm::vec3 b = (b2->getOrientatedVertices()[b2->edge[j][0]] - b2->getOrientatedVertices()[b2->edge[j][1]]);

				glm::vec3 normal = glm::cross(a,b) / (glm::length(a) * glm::length(b));
				glm::vec3 r = b2->getOrientatedVertices()[b2->edge[j][0]] - b1->getOrientatedVertices()[b1->edge[i][0]];
				
				float s = glm::dot(r, glm::cross(b, normal)) / glm::dot(a, glm::cross(b, normal));
				float t = -glm::dot(r, glm::cross(a, normal)) / glm::dot(b, glm::cross(a, normal));
				
				glm::vec3 pa, qa, m;

				pa = b1->getOrientatedVertices()[b1->edge[i][0]]*t+ b1->getOrientatedVertices()[b1->edge[i][1]];
				qa = b2->getOrientatedVertices()[b2->edge[j][0]]*s + b2->getOrientatedVertices()[b2->edge[j][1]];

				m = pa - qa;
				
				
				if((0<s)&&(s<1)&&(0<t)&&(t<1)){
					if (glm::length(m) < 50) {
						printf("%f %f %f \n", s, t, glm::length(m));
						std::tie(delP, delL) = b1->collisionResponse(pa-b1->X, normal);
						b1->p += delP / 0.50f; b1->L += delL / 2.0f;
						b2->p -= delP / 0.50f; b2->L -= delL / 2.0f;
					}
				}
		}
	}
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
	
	if (bounding) {

		std::vector< glm::vec3 > normals;
		bool res = loadOBJfacenormal("dodecahedron.obj", Vert, normals);
		Point point;
		
		for (int i = 0; i < Vert.size()/3; i++)
		{	
			float scale = 150;
			Vert[3 * i][0] *= scale;
			Vert[3 * i+1][0] *= scale;
			Vert[3 * i+2][0] *= scale;

			Vert[3 * i][1] *= scale;
			Vert[3 * i + 1][1] *= scale;
			Vert[3 * i + 2][1] *= scale;

			Vert[3 * i][2] *= scale;
			Vert[3 * i + 1][2] *= scale;
			Vert[3 * i + 2][2] *= scale;

			indices.push_back({ 3 * i,3 * i + 1,3 * i + 2 });
		}
		/*
		Vert.push_back(glm::vec3(50.0, -100, -100));
		Vert.push_back(glm::vec3(50.0, 100, -100));
		Vert.push_back(glm::vec3(50.0, 100, 100));
		Vert.push_back(glm::vec3(50.0, -100, 100));
		Vert.push_back(glm::vec3(-100.0, -100, -100));
		Vert.push_back(glm::vec3(-100.0, 100, -100));
		Vert.push_back(glm::vec3(-100.0, 100, 100));
		Vert.push_back(glm::vec3(-100.0, -100, 100));

		indices.push_back({ 0,1,2 });
		indices.push_back({ 4,7,6,5 });
		
		//indices.push_back({ 1,2,6,5 });
		//indices.push_back({ 0,3,7,4 });
		*/
		bounding = false;

		body.X = { -30,50,0 };
		body.L = { 600,300,0 };
		body.p = { 0,-30,0 };
		body1.q = glm::angleAxis((float)3.14 / 2, glm::vec3(0, 1, 0));

		body1.p = {0,-10,0};
		body1.X = { 0,0,0 };
		body1.L = { 0,0,0 };
		body1.q = glm::angleAxis((float)3.14 / 2, glm::vec3(0,1,0));

		body2.p = { 0,30,0 };
		body2.X = { 0,-150,0 };
		body2.L = { 0,0,0 };
		//body2.q = glm::angleAxis((float)3.14 / 3, glm::vec3(0, 1, 0));
	}

	//for (int i = 100000000000; i > 0; i--);
	//printf("%d ", indices.size());
}

void drawBody(Body body) {

	glPushMatrix();
	orientedVertices = body.getOrientatedVertices();
	for (int i = 0; i < body.faces.size();i++) {
		
	/*
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.0, 0.5);
		glVertex3f(orientedVertices[body.faces[i][0]][0], orientedVertices[body.faces[i][0]][1], orientedVertices[body.faces[i][0]][2]);
		glVertex3f(orientedVertices[body.faces[i][1]][0], orientedVertices[body.faces[i][1]][1], orientedVertices[body.faces[i][1]][2]);
		glVertex3f(orientedVertices[body.faces[i][2]][0], orientedVertices[body.faces[i][2]][1], orientedVertices[body.faces[i][2]][2]);
		glVertex3f(orientedVertices[body.faces[i][3]][0], orientedVertices[body.faces[i][3]][1], orientedVertices[body.faces[i][3]][2]);
		glEnd();
		*/

		glLineWidth(3.0);
		GLfloat lineColor[3] = { 0.0,0.3,0.0 };
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
		glVertex3f(orientedVertices[body.faces[i][0]][0], orientedVertices[body.faces[i][0]][1], orientedVertices[body.faces[i][0]][2]);
		glVertex3f(orientedVertices[body.faces[i][1]][0], orientedVertices[body.faces[i][1]][1], orientedVertices[body.faces[i][1]][2]);
		glVertex3f(orientedVertices[body.faces[i][2]][0], orientedVertices[body.faces[i][2]][1], orientedVertices[body.faces[i][2]][2]);
		glVertex3f(orientedVertices[body.faces[i][3]][0], orientedVertices[body.faces[i][3]][1], orientedVertices[body.faces[i][3]][2]);
		glVertex3f(orientedVertices[body.faces[i][0]][0], orientedVertices[body.faces[i][0]][1], orientedVertices[body.faces[i][0]][2]);
		glEnd();
	
	}
	/*
	for (int i = 0; i < body.edge.size(); i++)
	{
		glLineWidth(3.0);
		GLfloat lineColor[3] = { 0.0,0.3,0.0 };
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);
		glVertex3f(orientedVertices[body.edge[i][0]][0], orientedVertices[body.edge[i][0]][1], orientedVertices[body.edge[i][0]][2]);
		glVertex3f(orientedVertices[body.edge[i][1]][0], orientedVertices[body.edge[i][1]][1], orientedVertices[body.edge[i][1]][2]);
		glEnd();
	}
	glPopMatrix();
	*/
}

void drawBounding() {
	glPushMatrix();
	
	for (int i = 0; i < indices.size(); i++) {

		/*
		glBegin(GL_POLYGON);
		glColor3f(0.3, 0.3, 0.);
		
		for (int j = 0; j < indices[i].size(); j++) {
			glVertex3f(Vert[indices[i][j]][0], Vert[indices[i][j]][1], Vert[indices[i][j]][2]);
		}
		glVertex3f(Vert[indices[i][0]][0], Vert[indices[i][0]][1], Vert[indices[i][0]][2]);
		glEnd();
		*/
		glLineWidth(3.0);
		GLfloat lineColor[3] = { 0.3,0.3,0.0 };
		glBegin(GL_LINE_STRIP);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lineColor);

		for (int j = 0; j < indices[i].size(); j++) {
			glVertex3f(Vert[indices[i][j]][0], Vert[indices[i][j]][1], Vert[indices[i][j]][2]);
		}
		glVertex3f(Vert[indices[i][0]][0], Vert[indices[i][0]][1], Vert[indices[i][0]][2]);

		glEnd();
	}
	glPopMatrix();
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glRotatef(spinup, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 1.0, 0.0);

	drawBody(body);
	//drawBody(body1);
	//drawBody(body2);
	drawBounding();
	
	if (!(collisionPoint[0] == 0 && collisionPoint[1] == 0 && collisionPoint[2] == 0)) {
		glPushMatrix();
		GLfloat ballColor[3] = { 0.7,0.2,0.1 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, ballColor);
		glTranslatef(collisionPoint[0], collisionPoint[1], collisionPoint[2]);
		glutSolidSphere(3, 40, 40);
		glPopMatrix();
		for (int i = 100000000; i > 0; i--);
	}

	glPopMatrix();
	glutSwapBuffers();
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
		
		collisionPoint = body.CollisionTest(Vert, indices, h);
		//ObjectCollision(&body, &body1, h);
		//ObjectCollision(&body1, &body2, h);
		//ObjectCollision(&body, &body2, h);
		//EdgeCollision(&body, &body1, h);
		body.RK4step(h);
		body.update();

		//body1.RK4step(h);
		//body1.update();

		//body2.RK4step(h);
		//body2.update();
	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	xchange = x - lastx;
	ychange = y - lasty;
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

int main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Rigid Body demo");

	init();
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}