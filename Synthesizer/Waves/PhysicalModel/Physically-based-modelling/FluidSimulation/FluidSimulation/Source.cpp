#include <iostream>

#include "Fluid.h"
#include <glut.h>
#include <ctime>

int N = 50;
float viscocity = 1, h = 0.1;

static const int FPS = 30;
const int tMAX = 10;
float timeSinceLast = 0, timeTaken;
int frameTime = 0;
clock_t initialTime = clock(), finalTime;
static int WinId;
static int WinX, WinY;
static float Force = 1, Source =10, Temp;
Fluid fluid( 50,  viscocity,  h);
static int Mouse_clicked[3];
static int click_x, click_y, xVel_Mouse, yVel_Mouse, windowsID;


void initializeField() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			fluid.VelocityField[fluid.indexXY(i, j)] = { 0.0,0.0 };
			fluid.PressureField[fluid.indexXY(i, j)] = 0.0;
			fluid.AccelerationField[fluid.indexXY(i, j)] = { 0.0,0.0 };
			fluid.color[fluid.indexXY(i, j)] = { 0.0,0.0,0.0 };
		}
	}
}

static void MouseFunc(int button, int state, int x, int y)
{
	click_x = xVel_Mouse = x;
	click_y = yVel_Mouse = y;

	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{
		button = 2;
	}

	Mouse_clicked[button] = state == GLUT_DOWN;
}
static void MotionFunc(int x, int y)
{
	xVel_Mouse = x;
	yVel_Mouse = y;
}

void preDisplay() {
	glViewport(0, 0, WinX, WinY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void velocityDisplay() {
	preDisplay();
	int i, j;
	float x, y, spacing;
	spacing = 1.0f / fluid.gridSize;
	glColor3f(1.0f,1.0f,1.0f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	for (i = 0; i < fluid.gridSize-1; i++) {
		for (j = 0; j < fluid.gridSize-1; j++) {

			x = (i + 0.5f) * spacing; y = (j + 0.5f) * spacing;
			glVertex2f(x, y);
			//glVertex2f(x + 0.01, y);
			glVertex2f(x + fluid.VelocityField[fluid.indexXY(i, j)][0], y + fluid.VelocityField[fluid.indexXY(i, j)][1]);
		}
	}
	glEnd();
	glutSwapBuffers();
}

static void inputVelCol()
{
	int x, y;

	for (x = 0; x < fluid.totalCells; x++)
	{
		fluid.AccelerationField[x] = { 0.0,0.0 };
	}

	if (!Mouse_clicked[0] && !Mouse_clicked[2]) return;

	// Convert the mouse position to a grid position
	x = static_cast<int>((xVel_Mouse / static_cast<float>(WinX)) * fluid.gridSize);
	y = static_cast<int>(((WinY - yVel_Mouse) / static_cast<float>(WinY)) * fluid.gridSize);

	// Enfore the clicking to the velocity
	if (Mouse_clicked[0])
	{
	
		fluid.AccelerationField[fluid.indexXY(x, y)][0] = Force * (xVel_Mouse - click_x) *0.5;
		fluid.AccelerationField[fluid.indexXY(x, y)][1] = Force * (click_y - yVel_Mouse) *0.5;
	}

	// Add fluid
	if (Mouse_clicked[2])
	{
		printf("clicked!\n");
		glm::vec3 a = { 0.5f, 0.5f, 0.5f };
		fluid.color[fluid.indexXY(x, y)] += h*Source * a;

	}

	click_x = xVel_Mouse;
	click_y = yVel_Mouse;
}

void idle(void)
{

	inputVelCol();

	//for (float t = 0; t < 1.0 / FPS; t += h) {
		fluid.timestepVelocity();
		fluid.timestepDye();
	//}

	finalTime = clock();
	timeTaken = (float)((finalTime - initialTime)) / CLOCKS_PER_SEC;
	initialTime = finalTime;
	timeSinceLast += timeTaken;
	//while ((timeSinceLast + (float)(clock() - initialTime) / CLOCKS_PER_SEC)  < 1.0 / FPS);

	timeSinceLast = 0;
	glutSetWindow(windowsID);
	glutPostRedisplay();
}

void timer(int v) {
	glutTimerFunc(1000 / FPS, timer, v);
}

void reshapeFunc(int newWidth, int newHeight)
{
	glutSetWindow(windowsID);
	glutReshapeWindow(newWidth, newHeight);
	WinX = newWidth;
	WinY = newHeight;
	glutPostRedisplay();
}

void display() {

	//Drawing the quads
	glBegin(GL_QUADS);
	float spacing = 1.0f / fluid.gridSize;
	for (int i = 0; i < fluid.gridSize; i++) {
		for (int j = 0; j < fluid.gridSize; j++) {
			float x = (i + 0.5f) * spacing;
			float y = (j + 0.5f) * spacing;

			glColor3f(fluid.color[fluid.indexXY(i, j)][0], fluid.color[fluid.indexXY(i, j)][1], fluid.color[fluid.indexXY(i, j)][2]);
			glVertex2f(x, y);
			glColor3f(fluid.color[fluid.indexXY(i+1, j)][0], fluid.color[fluid.indexXY(i+1, j)][1], fluid.color[fluid.indexXY(i+1, j)][2]);
			glVertex2f(x+spacing, y);
			glColor3f(fluid.color[fluid.indexXY(i+1, j+1)][0], fluid.color[fluid.indexXY(i+1, j+1)][1], fluid.color[fluid.indexXY(i+1, j+1)][2]);
			glVertex2f(x+spacing, y + spacing);
			glColor3f(fluid.color[fluid.indexXY(i, j+1)][0], fluid.color[fluid.indexXY(i, j+1)][1], fluid.color[fluid.indexXY(i, j+1)][2]);
			glVertex2f(x, y + spacing);
		}
	}

	glEnd();
	glutSwapBuffers();
}


void glutloop() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WinX, WinY);

	windowsID = glutCreateWindow("Stable fluids");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);


	preDisplay();

	glutTimerFunc(100, timer, 0);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	glutReshapeFunc(reshapeFunc);
	glutIdleFunc(idle);
	glutDisplayFunc(display);

}
int main(int argc, char** argv) {

	glutInit(&argc, argv);

	initializeField();

	

	WinX = 512;
	WinY = 512;

	glutloop();
	glutMainLoop();
	

	std::cout << "Team Turbulence";
	getchar();
}

