/*

Name: Group 1

Student ID: B0029472, B00307589, B00287948

I declare that the following code was produced by Lars Anthony Palm, Benjamin Tracy, Thomas Mackrell as a group assignment for the IPM module and that this is our own work.

I am aware of the penalties incurred by submitting in full or in part work that is not our own and that was developed by third parties that are not appropriately acknowledged.

*/



#include <glm/glm.hpp>
#include <SDL.h>
#include <math.h>
#include <GL\glew.h>
#undef main
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;

#define IX(x,y) ((x)+(N+2)*(y)) //Derived from: Jos Stam, 2003, Real-time Fluid Dynamics for Games

/*WARNING: Do NOT change the CELL_SIZE and GRID_SIZE upwards unless you have a beast of a computer*/

#define CELL_SIZE 10 //Distance between each vertex point building one square cell.
#define GRID_SIZE 100 //Number of cells in the grid. 

/*WARNING: Do NOT change the display_Size_X and Y upwards unless you have a beast of a computer*/

#define DISPLAY_SIZE_X 1280 // Size of display on x cords.
#define DISPLAY_SIZE_Y 720 // Size of display on y cords.
#define MAX_COLOR 1024

//Global Variables

bool runSim = true;

//Set aspect ratio (world cords)
const static float ASPECT_RATIO = ((float)DISPLAY_SIZE_X / (float)DISPLAY_SIZE_Y);
const static float CELL_SIZE_X = (float)((1.0f / ((float)GRID_SIZE / (float)DISPLAY_SIZE_X))); //size of cells in grid
const static float CELL_SIZE_Y = (float)((1.0f / ((float)GRID_SIZE / (float)DISPLAY_SIZE_Y))); //size of cells in grid

string lineBreak = "-----------------------------------------";

struct color {
	float r, g, b;
};

color *colorRange[MAX_COLOR];
int x, y;

//Mouse position
int mousePosiX;
int mousePosiY;
int mousePosiZ;


//Derived from: Michael Ash, 2006, Fluid Simulation for Dummies
struct FluidCube {
	int size;
	float dt; //Delta Time (Step-Time)
	float diffussion; //Diffussion Amount
	float viscosity; // Viscosity, thickness of the fluid

	float *source;
	float *density;

	//Velocity current
	float *Vx;
	float *Vy;

	//Velocity previous
	float *Vx0;
	float *Vy0;

};
typedef struct FluidCube FluidCube;

struct Obstacle {
	Obstacle() { //Default constructor

	}

	Obstacle(int x, int y, int b) {
		this->x = x;
		this->y = y;
		this->b = b;
	}
	int x;
	int y;
	int b; // Orientation of where velocity/density is coming from
};

struct ObstacleList {
	Obstacle obstacle;
	ObstacleList *next;
};

struct ObstacleBlock {
	ObstacleBlock() {
		list = nullptr;
	}

	void add(Obstacle obstacle) {
		ObstacleList *newObstacle = new ObstacleList;
		newObstacle->obstacle = obstacle; //new linked list object
		newObstacle->next = nullptr;  //object.next = nullptr
		if (list == nullptr) { //list(node) == NULL
			list = newObstacle; //list = node object (reference to head)
			end = newObstacle;  //end = node object (reference to head)

		}
		else {
			end->next = newObstacle; // end node next reference = new node 
			end = end->next; // end node reference = next new node. 
		}
	}


	void drawObstacle() {
		ObstacleList* current = list;
		glColor3f(1.0, 1.0, 1.0); // <- White
		while (current != nullptr) {
			float pixelX = current->obstacle.x * CELL_SIZE_X;
			float pixelY = current->obstacle.y * CELL_SIZE_Y;

			glBegin(GL_QUADS);
			glVertex2f(pixelX, pixelY);
			glVertex2f(pixelX + CELL_SIZE_X, pixelY);
			glVertex2f(pixelX + CELL_SIZE_X, pixelY + CELL_SIZE_Y);
			glVertex2f(pixelX, pixelY + CELL_SIZE_Y);
			glEnd();

			current = current->next;


		}
	}

	ObstacleList* list;
	ObstacleList* end;
};
ObstacleBlock obstacles = ObstacleBlock();

//A 2D representation of a Circle containing a centre position, radius and velocity
struct Circle
{
	glm::vec2 c;
	float r;
	glm::vec2 v;

	void draw()
	{
		const int NPOINTS = 24;
		const float TWOPI = 2 * 3.14159268;
		const float STEP = TWOPI / NPOINTS;
		glBegin(GL_LINE_LOOP);
		for (float angle = 0; angle < TWOPI; angle += STEP)
		{
			glVertex2f(c.x * GRID_SIZE * CELL_SIZE_X + r * GRID_SIZE * CELL_SIZE_X * cos(angle), c.y * GRID_SIZE * CELL_SIZE_Y + r * GRID_SIZE * CELL_SIZE_Y * sin(angle));
		}
		glEnd();
	}
};
std::vector<Circle> bodies;

//Updates the position of a rigid body based on the velocity at it's centre location
void updateRigidBody(int N, float* Vx, float* Vy, Circle& circle, float dt)
{
	int x = circle.c.x*N;
	int y = circle.c.y*N;
	circle.v = glm::vec2(Vx[IX(x, y)], Vy[IX(x, y)]);
	circle.c += circle.v * dt;
}

void checkBodyCollisions(int N)
{
	//the width of a single cell
	float h = 1.0f / N;
	Circle c1, c2;
	//Collisions between bodies
	for (size_t i = 0; i < bodies.size(); i++)
	{
		c1 = bodies[i];
		for (size_t j = i + 1; j < bodies.size(); j++)
		{

			c2 = bodies[j];
			float d = glm::length(c2.c - c1.c);
			float r = c1.r + c2.r;
			if (d < r)
			{
				glm::vec2 axis = c2.c - c1.c;
				axis = glm::normalize(axis);
				bodies[i].c += axis * ((d - r) * 0.5f);
				bodies[j].c -= axis * ((d - r) * 0.5f);

			}
		}
		if (c1.c.x < h + c1.r)
		{
			bodies[i].c.x = h + c1.r;
		}
		if (c1.c.x > 1 - h - c1.r)
		{
			bodies[i].c.x = 1 - h - c1.r;
		}
		if (c1.c.y < h + c1.r)
		{
			bodies[i].c.y = h + c1.r;
		}
		if (c1.c.y > 1 - h - c1.r)
		{
			bodies[i].c.y = 1 - h - c1.r;
		}

		//Check collisions between the current body c1 and the simulation obstacles
		ObstacleList* current = obstacles.list;
		while (current != nullptr)
		{
			//Here we treat the obstacle as a circle body at the location of the obstacle, with a radius of 0.5h
			glm::vec2 obsP((float)current->obstacle.x / GRID_SIZE, (float)current->obstacle.y / GRID_SIZE);
			float d1 = glm::length(obsP - bodies[i].c);
			float r1 = c1.r + 0.5f*h;
			if (d1 < r1)
			{
				glm::vec2 axis = obsP - bodies[i].c;
				axis = glm::normalize(axis);
				bodies[i].c += axis * ((d1 - r1) * 1.001f);
			}

			current = current->next;
		}
	}
}

FluidCube *FluidCubeCreate(int size, float diffusion, float viscosity, float dt) {
	FluidCube *fluidCube = new FluidCube;
	int N = size;

	fluidCube->size = size;
	fluidCube->dt = dt; //Delta Time (Step-Time)
	fluidCube->diffussion = diffusion; //Diffussion Amount
	fluidCube->viscosity = viscosity; // Viscosity, thickness of the fluid

	fluidCube->source = new float[(N + 2) * (N + 2)]();
	fluidCube->density = new float[(N + 2) * (N + 2)]();

	//Velocity current
	fluidCube->Vx = new float[(N + 2) * (N + 2)]();
	fluidCube->Vy = new float[(N + 2) * (N + 2)]();

	//Velocity previous
	fluidCube->Vx0 = new float[(N + 2) * (N + 2)]();
	fluidCube->Vy0 = new float[(N + 2) * (N + 2)]();



	return fluidCube;
}


/* --------------- fluidCube Adding --------------- */

// Add Density
void fluidCubeAddDensity(FluidCube *fluidCube, int x, int y, float amount) {
	int N = fluidCube->size;
	fluidCube->source[IX(x, y)] += amount;
	//cout << "Density amount at point: " << (float)fluidCube->s[IX(x, y)] << endl;
}

//Add Velocity
void fluidCubeAddVelocity(FluidCube *fluidCube, int x, int y, float amountX, float amountY) {
	int N = fluidCube->size;
	int index = IX(x, y);

	fluidCube->Vx[index] += amountX;
	fluidCube->Vy[index] += amountY;

}


/*
NOTE TO SELF
i = x
j = y
k = z

b= It tells the function which array it's dealing with, and so whether each boundary cell
should be set equal or opposite its neighbor's value.
x = float based on velocity reference so it can be changed
N = size of grid
*/

static void set_bnd(int b, float *x, int N, int type) {

	int i;
	for (i = 1; i <= N; i++) {
		// set points for the 4 edges of the boundry based on the size of grid for next step based on current step
		// bounces velocity by inversing if boundry hits the boundry

		/*X Axis*/
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];

		/*Y Axis*/
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}
	/*
	set points for the 4 corners of the boundry based on size of grid for the current step
	*/

	x[IX(0, 0)] = 0.5*(x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5*(x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5*(x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5*(x[IX(N, N + 1)] + x[IX(N + 1, N)]);

	if (type == 2) {
		/*Add Obstacle Boundaries*/
		//ListNode head (list ) = obstacle.list
		ObstacleList* current = obstacles.list;

		while (current != nullptr) {
			x[IX(current->obstacle.x, current->obstacle.y)] = 0;
			current = current->next;
		}
	}
}

void addObstacle(int x, int y) {
	/*Must have at least 4x Obstacle Block for this simulation*/

	//First Obstacle Cell
	Obstacle O = Obstacle(x, y, 1);
	obstacles.add(O);

	//Second Obstacles Cell
	Obstacle O2 = Obstacle(x + 1, y, 1);
	obstacles.add(O2);

	//Third Obstacle Cell
	Obstacle O3 = Obstacle(x, y + 1, 2);
	obstacles.add(O3);

	//Fourth Obstacles Cell
	Obstacle O4 = Obstacle(x + 1, y + 1, 2);
	obstacles.add(O4);
}

/*
Function finds previous velocity

Implementation of Gauss-Seidel relaxation formula.
diffuse(N, 1, Vx0, Vx, visc, dt);

N = Size
b = checks if previous velocity x or y as 1 or 2
x = previous velocity
x0 = current velocity
diff = viscocity
dt = deltaTime / Timestep

diffuse(N, 0, s, density, diff, dt); //computes diffusion for next step in grid
Derived from: Jos Stam, 2003, Real-time Fluid Dynamics for Games
*/
void diffuse(int N, int b, float *x, float *x0, float diff, float dt) {
	int i, j, k;
	float a = dt * diff * N * N; // Delta Time * Viscocity * Size * Size

	for (k = 0; k < 20; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				x[IX(i, j)] = (x0[IX(i, j)] + a*(x[IX(i - 1, j)] + x[IX(i + 1, j)] + +x[IX(i, j - 1)] + +x[IX(i, j + 1)])) / (1 + 4 * a);
			}
		}
		set_bnd(b, x, N, 2);
	}
}




/*
Linearly interpolate from the grid of previous density values
and assign this value to the current grid cell.

advect(N, 1, Vx, Vx0, Vx0, Vy0, dt);
advect(N, 2, Vy, Vy0, Vx0, Vy0, dt);

N = size
b = if x or y
d* = current velocity of b
d0* = previous velocity of b
u* = previous velocity of x
v* = previous velocity of y
dt = delta time

advection definition: the transfer of heat or matter by the flow of a fluid,
especially horizontally in the atmosphere or the sea.

advect(N, 0, density, s, Vx, Vy, dt); //computes advection of current density for next step.
Derived from: Jos Stam, 2003, Real-time Fluid Dynamics for Games
*/
void advect(int N, int b, float *d, float *d0, float *u, float*v, float dt) {
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = dt * N; // delta time * size

	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			x = i - dt0*u[IX(i, j)];
			y = j - dt0*v[IX(i, j)];

			if (x < 0.5) {
				x = 0.5;
			}
			/*
			-----above-----
			clamp the value of x between 0.5 and N + 0.5
			-----below-----
			*/
			if (x > N + 0.5) {
				x = N + 0.5;
			}

			i0 = (int)x; //if x = 1.5, x = 1, wrap to int.
			i1 = i0 + 1;

			if (y < 0.5) {
				y = 0.5;
			}
			/*
			---- - above---- -
			clamp the value between 0.5 and N + 0.5
			---- - below---- -
			*/
			if (y > N + 0.5) {
				y = N + 0.5;
			}

			j0 = (int)y; //if y = 1.5, y = 1, wrap to int.
			j1 = j0 + 1;

			// math.h round function <---- similar----below for x and y position.
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;

			//computes current velocity of x or y
			d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
		}
	}
	set_bnd(b, d, N, 2); // ensures advection is not going outside of the boundry, if not fix it.
}


/*
project(N, Vx0, Vy0, Vx, Vy);

It is assume that the physical length of each side of the grid is
one so that the grid spacing is given by h = 1 / N.

N = size
U = array previous velocity X
v = array previous velocity Y
p = array of current velocity X / possibly pressure as well
div = array  of current velocity Y
h = square root of cell (half a cell)
Derived from: Jos Stam, 2003, Real-time Fluid Dynamics for Games
*/


void project(int N, float *u, float *v, float *p, float *div) {
	int i, j, k;
	float h;

	h = 1.0 / N;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++)
		{// takes all 4 values to...    
		 /*
		 -0.5 * h * (left cell X - right cell X + top cell Y - bottom cell Y)
		 compute and sets array of current velocity for Y

		 possibly sets magnitude and angle - > computes vector?
		 */
			div[IX(i, j)] = -0.5 * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0; //sets current velocity of X to 0
		}
	}

	set_bnd(0, div, N, 2);
	set_bnd(0, p, N, 2);

	// why 20? = becase the value P changes as we progress through the computation
	for (int k = 0; k < 20; k++)
	{
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				// Reset I and J to 1, 20x.
				// compute and sets array of current velocity for X
				// P value changes as we progress through computation.
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
			}
		}
		set_bnd(0, p, N, 2);
	}

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{	// compute and sets array of previous velocity for X and Y based on current velocity X
			u[IX(i, j)] -= 0.5 * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5 * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
		}
	}
	set_bnd(1, u, N, 2);
	set_bnd(2, v, N, 2);
}

//add_source(N, Vx, Vx0, dt);
/*
N = size
Vx = velocity
Vx0 = previous velocity
dt = time step

add_source(N, density, s, dt); //compute density into grid

*/

void add_source(int N, float * Vx, float * Vx0, float dt) {
	int i, size = (N + 2) * (N + 2);
	for (i = 0; i < size; i++)
	{
		Vx[i] += dt*Vx0[i];
	}

}

void getForces(int N, float * s, float * Vx0, float * Vy0) //reset Force to 0
{
	int i, size = (N + 2) * (N + 2);
	for (i = 0; i < size; i++)
	{
		s[i] = 0;
		Vx0[i] = 0;
		Vy0[i] = 0.0f;
	}
}

/*
N = size
s* = source
Vx = velocity for x
Vy = Velocity for y
Vx0 = previous velocity for x
Vy0 = previous velocity for y

Clears the Simulation Data
*/

void clearData(int N, float *s, float *Vx, float *Vy, float *Vx0, float *Vy0, float *density) //reset Force to 0
{
	int i, size = (N + 2) * (N + 2);
	for (i = 0; i < size; i++)
	{
		s[i] = 0.0;
		Vx[i] = 0.0;
		Vy[i] = 0.0;
		Vx0[i] = 0.0;
		Vy0[i] = 0.0;
		density[i] = 0.0;
	}
	bodies.clear();
}

/* ------------- fluidCube Time Step ------------- 
Derived from: Michael Ash, 2006, Fluid Simulation for Dummies
*/

void fluidCubeTimeStep(FluidCube *fluidCube) {
	int N = fluidCube->size;
	float visc = fluidCube->viscosity;
	float diff = fluidCube->diffussion;
	float dt = fluidCube->dt;
	float *Vx = fluidCube->Vx;
	float *Vy = fluidCube->Vy;
	float *Vx0 = fluidCube->Vx0;
	float *Vy0 = fluidCube->Vy0;
	float *s = fluidCube->source; // density of previous step
	float *density = fluidCube->density; // density of current step

	//Velocity step
	add_source(N, Vx, Vx0, dt); //compute velocity for X
	add_source(N, Vy, Vy0, dt); //compute velocity for Y
	diffuse(N, 1, Vx0, Vx, visc, dt); //computes current velocity for next step for X
	diffuse(N, 2, Vy0, Vy, visc, dt); //computes current velocity for next step for Y

	project(N, Vx0, Vy0, Vx, Vy);  //Set boundry as a square/cube and computes reaction of velocity if the boundry is hit. 

	advect(N, 1, Vx, Vx0, Vx0, Vy0, dt); //computes advection of current velocity for next step for X
	advect(N, 2, Vy, Vy0, Vx0, Vy0, dt); //computes advection of current velocity for next step for Y

	project(N, Vx, Vy, Vx0, Vy0); //Set boundry as a square/cube and computes reaction of velocity if the boundry is hit.

	//Density step
	add_source(N, density, s, dt); //compute density into grid
	diffuse(N, 0, s, density, diff, dt); //computes diffusion for next step in grid
	advect(N, 0, density, s, Vx, Vy, dt); //computes advection of current density for next step. 

	for (size_t i = 0; i < bodies.size(); i++)
	{
		updateRigidBody(N, Vx, Vy, bodies[i], dt);
	}
	checkBodyCollisions(N);
	getForces(N, s, Vx0, Vy0); //Do all of above and then reset velocity and density to 0 for x and y
}

/* ------------- Clear 'fluidCube' Data Time Step ------------- */

void clearFluidCubeTimeStep(FluidCube *fluidCube) {
	int N = fluidCube->size;
	float diff = fluidCube->diffussion;
	float *Vx = fluidCube->Vx;
	float *Vy = fluidCube->Vy;
	float *Vx0 = fluidCube->Vx0;
	float *Vy0 = fluidCube->Vy0;
	float *s = fluidCube->source; // density of previous step
	float *density = fluidCube->density; // density of current step

	clearData(N, s, Vx, Vy, Vx0, Vy0, density);
}

void drawFluidVelocity(FluidCube* fluidCube) {
	int i, j;
	int N = fluidCube->size;
	//cout << "N " << N << endl;
	float x, y;
	float h = 1.0f;
	float *Vx = fluidCube->Vx;
	float *Vy = fluidCube->Vy;
	float cellSize = CELL_SIZE;
	float sourceAlpha = 0.05;

	int velocityMultiplier = 5;
	float highestVelocity = 0.0;
	int finalCv = 0;


	for (i = 1; i <= N; i++) {
		x = (i)* h;
		for (j = 1; j <= N; j++) {
			y = (j)* h;

			float pointVeloX = Vx[IX(i, j)];
			float pointVeloY = Vy[IX(i, j)];

			/*Initial Color Value*/
			float iniCv = pointVeloX + pointVeloY;
			if (highestVelocity < abs(iniCv)) //Absolute Value of Initial Color Value
				highestVelocity = iniCv; //Set Highest Velocity = Initial Color Value

										 /*Final Color Value*/
			int finalCv = abs(round(iniCv / 0.5 * (MAX_COLOR - 1)));

			/*Clamp FinalCv MAX_COLOR */
			if (finalCv > MAX_COLOR - 1) {
				finalCv = MAX_COLOR - 1;
			}
			else if (finalCv < MAX_COLOR - (MAX_COLOR - 1)) {
				finalCv = 0;
			}


			int actualX = x - Vx[IX(i, j)];
			int actualY = y - Vy[IX(i, j)];

			/*Add Point Velocity*/
			int addPointVelo = pointVeloX + pointVeloY;

			glLineWidth(iniCv * velocityMultiplier);


			glBegin(GL_LINES);
			/*Set Color and SourceAlpha intensity based on Velocity*/
			glColor4f(colorRange[finalCv]->r, colorRange[finalCv]->g, colorRange[finalCv]->b, sourceAlpha);
			/*Set points for velocity line*/
			glVertex2f(x*CELL_SIZE_X, y*CELL_SIZE_Y);
			/*Set gradient starting color for each velocity line*/
			glColor3f(0.0f, 0.0f, 0.0f);
			/*Set width and length and direction of velocity line. */
			glVertex2f((x * CELL_SIZE_X + (Vx[IX(i, j)] * velocityMultiplier)), (y * CELL_SIZE_Y + (Vy[IX(i, j)] * velocityMultiplier)));
		}
	}
	glEnd();
	//std::cout << "Highest Velocity Value: " << highestVelocity << endl;
}

void drawFluidDensity(FluidCube* fluidCube) {

	int N = fluidCube->size;
	int size = N*N;
	float h = 1.0f;
	float sourceAlpha = 0.05;
	float sourceAlpha00 = 0.05;
	float sourceAlpha01 = 0.05;
	float sourceAlpha10 = 0.05;
	float sourceAlpha11 = 0.05;
	float d00, d01, d10, d11;
	float d00R, d01R, d10R, d11R;
	float d00G, d01G, d10G, d11G;
	float d00B, d01B, d10B, d11B;

	float highestDensity = 0;
	for (int i = 0; i < N; i++)
	{
		float x = (i)*h;
		for (int j = 0; j < N; j++) {
			float y = (j)*h;

			//Compute position of the fluid in the grid
			d00 = fluidCube->density[IX(i, j)];
			d01 = fluidCube->density[IX(i, j + 1)];
			d11 = fluidCube->density[IX(i + 1, j + 1)];
			d10 = fluidCube->density[IX(i + 1, j)];

			int d00C = round(d00 / 500 * (MAX_COLOR - 1));
			int d01C = round(d01 / 500 * (MAX_COLOR - 1));
			int d11C = round(d11 / 500 * (MAX_COLOR - 1));
			int d10C = round(d10 / 500 * (MAX_COLOR - 1));

			/*Clamp D00C Max Color */
			if (d00C > MAX_COLOR - 1) {
				d00C = MAX_COLOR - 1;
			}
			else if (d00C < MAX_COLOR - (MAX_COLOR - 1)) {
				d00C = 0;
			}

			/*Clamp D01C Max Color */
			if (d01C > MAX_COLOR - 1) {
				d01C = MAX_COLOR - 1;
			}
			else if (d01C <  MAX_COLOR - (MAX_COLOR - 1)) {
				d01C = 0;
			}

			/*Clamp D11C Max Color for */
			if (d11C > MAX_COLOR - 1) {
				d11C = MAX_COLOR - 1;
			}
			else if (d11C < MAX_COLOR - (MAX_COLOR - 1)) {
				d11C = 0;
			}

			/*Clamp D10C Max Color */
			if (d10C > MAX_COLOR - 1) {
				d10C = MAX_COLOR - 1;
			}
			else if (d10C < MAX_COLOR - (MAX_COLOR - 1)) {
				d10C = 0;
			}

			//Initialize all dxxR, dxxG, dxxB and sourceAlphaxx
			d00R = d10R = d01R = d11R = 0.0;
			d00G = d01G = d10G = d11G = 0.0;
			d00B = d01B = d10B = d11B = 0.0;
			sourceAlpha00 = sourceAlpha10 = sourceAlpha01 = sourceAlpha11 = 0.3;

			if (highestDensity < d00) {
				highestDensity = d00;
			}

			if (highestDensity < d01) {
				highestDensity = d01;
			}

			if (highestDensity < d11) {
				highestDensity = d11;
			}

			if (highestDensity < d10) {
				highestDensity = d10;

			}

			// Draw density Quads

			glBegin(GL_QUADS);
			glColor4f(colorRange[d00C]->r, colorRange[d00C]->g, colorRange[d00C]->b, sourceAlpha00);
			glVertex3f(x * CELL_SIZE_X, y * CELL_SIZE_Y, 0);

			glColor4f(colorRange[d01C]->r, colorRange[d01C]->g, colorRange[d01C]->b, sourceAlpha01);
			glVertex3f(x * CELL_SIZE_X, (y + h) * CELL_SIZE_Y, 0);

			glColor4f(colorRange[d11C]->r, colorRange[d11C]->g, colorRange[d11C]->b, sourceAlpha11);
			glVertex3f((x + h) * CELL_SIZE_X, (y + h) * CELL_SIZE_Y, 0);

			glColor4f(colorRange[d10C]->r, colorRange[d10C]->g, colorRange[d10C]->b, sourceAlpha10);
			glVertex3f((x + h) * CELL_SIZE_X, y * CELL_SIZE_Y, 0);

			glEnd();
			glColor4f(0, 0, 0, 1);

		}
	}
	//cout << "Highest Density Value: " << highestDensity << endl;
}

void initDisplayHelp() {
	cout << lineBreak << endl << endl;
	cout << "List of helpful commands: " << endl;
	cout << lineBreak << endl << endl;
	cout << "'H' to print helpful commands" << endl;
	cout << "'W' to add one obstacles at mouse position" << endl;
	cout << "'Left Ctrl' to add 1,000,000 density at mouse position." << endl;
	cout << "'Left Shift' to add 10,000,000 density at mouse position." << endl;
	cout << "'A' to add (-100, 0) velocity at the mouse position" << endl;
	cout << "'D' to add (100, 0) velocity at the mouse position" << endl;
	cout << "'S' to add one new rigid body at the mouse position" << endl;
	cout << "'Space' to clear simulation" << endl;
	cout << "'Return' to pause and resume simulation" << endl;
	cout << "'Esc' to quit simulation" << endl << endl;
	cout << lineBreak << endl << endl;
}


//Window settings
const int SCREEN_WIDTH = DISPLAY_SIZE_X;
const int SCREEN_HEIGHT = DISPLAY_SIZE_Y;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

//Add color spectra interperlation to the simulation. 
void initColors() {
	float R = 0.0f, G = 0.0f, B = 0.0f;
	int totalChanges = 4;
	float step = (float)totalChanges / MAX_COLOR;
	int change = 0;
	for (int i = 0; i < MAX_COLOR; i++) {
		colorRange[i] = new color();
		colorRange[i]->r = R;
		colorRange[i]->b = B;
		colorRange[i]->g = G;
		if (change == 0) {
			G += step;
		}
		else if (change == 1) {
			R += step;
			G -= step;

		}
		else if (change == 2) {
			B += step;
		}
		else {
			R -= step;
		}

		if (i > MAX_COLOR / totalChanges * 1)
			change = 1;
		if (i > MAX_COLOR / totalChanges * 2)
			change = 2;
		if (i >  MAX_COLOR / totalChanges * 3)
			change = 3;

	}
}

void drawBodies()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2);
	for (size_t i = 0; i < bodies.size(); i++)
	{
		bodies[i].draw();
	}
}

int main()
{
	initColors(); //Initiate Color Spectra
	initDisplayHelp(); // INitiate helpful commands.
	SDL_Init(SDL_INIT_VIDEO);

	// Request an OpenGL 3.0 context.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	gWindow = SDL_CreateWindow("Fluid Simulator - IPM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext context = SDL_GL_CreateContext(gWindow); //Create OpenGL Context and binds the gWindow to SDL.
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	glEnable(GL_BLEND);

	/* fluidCubeCreate(Size, Diffusion Amount, Viscosity Amount, Delta Time */
	FluidCube *fluidCube = FluidCubeCreate(GRID_SIZE, 0.0001f, 0.001f, 0.05f);	//Create Fluid Cube 

	float totalFluid = 5000;
	float currentFluid = totalFluid;
	bool running = true; // set running to true
	bool dataCleared = true;
	bool densityAdded = false;
	bool velocityAdded = false;


	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop

		//Get density & velocity at mouse posi and cout + get mouse posi in world cords
		SDL_GetMouseState(&mousePosiX, &mousePosiY);
		int gridX = (float)(((GRID_SIZE) / (float)DISPLAY_SIZE_X) * mousePosiX);
		int gridY = (float)(((GRID_SIZE) / (float)DISPLAY_SIZE_Y) * mousePosiY);

		//Get mouse cell position on grid (pixel cords)
		int adjustedMousePosiY = (DISPLAY_SIZE_Y - mousePosiY);
		int mouseGridPosiX = round((float)mousePosiX / CELL_SIZE_X);
		int mouseGridPosiY = round((float)adjustedMousePosiY / CELL_SIZE_Y);


		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;

			if (sdlEvent.type == SDL_KEYDOWN) {

				//-----------------KEYBOARD CONTROLS
				switch (sdlEvent.key.keysym.sym)
				{
					//If 'A' is pressed velocity is added to the left of the pointer
				case SDLK_a:
					fluidCubeAddVelocity(fluidCube, mouseGridPosiX, mouseGridPosiY, -100.0f, 0);
					cout << "Velocity amount added: -100" << endl;
					cout << lineBreak << endl << endl;
					break;
					//IF 'D' is pressed velocity is added to the right of the pointer
				case SDLK_d:
					fluidCubeAddVelocity(fluidCube, mouseGridPosiX, mouseGridPosiY, 100.0f, 0);
					cout << "Velocity amount added: 100" << endl;
					cout << lineBreak << endl << endl;
					break;
					//If 'Left Ctrl' is pressed then add density to area (1,000,000)
				case SDLK_LCTRL:
					densityAdded = true;
					fluidCubeAddDensity(fluidCube, mouseGridPosiX, mouseGridPosiY, 1000000);
					sdlEvent.key.repeat = 1;
					cout << "Density amount added: 1,000,000" << endl;
					sdlEvent.key.repeat = 1;
					densityAdded = false;
					cout << lineBreak << endl << endl;
					break;
					//If 'Left Shift' is pressed then add density to area (10,000,000)
				case SDLK_LSHIFT:
					densityAdded = false;
					fluidCubeAddDensity(fluidCube, mouseGridPosiX, mouseGridPosiY, 10000000);
					sdlEvent.key.repeat = 1;
					cout << "Density amount added: 10,000,000" << endl;
					densityAdded = true;
					cout << lineBreak << endl << endl;
					break;
					//If 'Space' is pressed then clear data from simulation
				case SDLK_SPACE:
					dataCleared = false;
					cout << "DataClear Status: False" << endl;
					clearFluidCubeTimeStep(fluidCube);
					dataCleared = true;
					cout << "DataClear Status: True" << endl;
					sdlEvent.key.repeat = 1;
					cout << "Simulation Data Cleared: SUCCESS" << endl;
					cout << lineBreak << endl << endl;
					break;
					//If 'H' is pressed cout helpful commands on control panel
				case SDLK_h:
					initDisplayHelp();
					sdlEvent.key.repeat = 1;
					break;
					//Add a circle body at the mouse position
				case SDLK_s:;
					bodies.push_back(Circle{ glm::vec2((float)mouseGridPosiX / GRID_SIZE, (float)mouseGridPosiY / GRID_SIZE), 0.01f, glm::vec2(0) });
					cout << "Successfully added 1 new rigid body: " << endl;
					cout << lineBreak << endl << endl;
					break;
                   // If 'W' is pressed insert an obstacle at mouse point
				case SDLK_w:
					addObstacle(mouseGridPosiX, mouseGridPosiY);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 1);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 2);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 3);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 4);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 5);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 6);
					addObstacle(mouseGridPosiX, mouseGridPosiY + 7);

					addObstacle(mouseGridPosiX + 1, mouseGridPosiY);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 1);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 2);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 3);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 4);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 5);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 6);
					addObstacle(mouseGridPosiX + 1, mouseGridPosiY + 7);
					sdlEvent.key.repeat = 1;
					cout << "Successfully added obstacle: " << endl;
					cout << lineBreak << endl << endl;
					break;
					//Pause & Resume Simulation by pressing 'Return'
				case SDLK_RETURN:
					runSim = !runSim;
					cout << "Successfully paused/restarted the simulation: " << endl;
					cout << lineBreak << endl << endl;
					break;
				   //Exit Simulation by pressing Esc
				case SDLK_ESCAPE:
					running = false;
					break;
				}
			}
		}

		//Release Dam Effect continuously
		if (running && runSim == true)
		{
				for (int j = 1; j <= GRID_SIZE; j++) {
					fluidCubeAddVelocity(fluidCube, 1, j, 3, 0);
					fluidCubeAddDensity(fluidCube, 1, j, 20);
				} fluidCubeTimeStep(fluidCube);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW); //Model Matrix
		glLoadIdentity(); //Identity Matrix
		glOrtho(0, DISPLAY_SIZE_X, 0, DISPLAY_SIZE_Y, 0.0f, 1.0f); //Projection Matrix

		glPointSize(10.0f);
		glLineWidth(5.0f);

		obstacles.drawObstacle();
		drawFluidDensity(fluidCube);
		drawFluidVelocity(fluidCube);
		drawBodies();

		SDL_GL_SwapWindow(gWindow); // Swap buffers
	}

	/*IMPORTANT! - DO NOT ALTER BELOW - WILL CAUSE MEMORY LEAK*/
	for (int i = 0; i < MAX_COLOR; i++) {
		delete colorRange[i];
	}

	/*Delete Linked List*/
	ObstacleList* current = obstacles.list;
	while (current != nullptr) {
		ObstacleList* temp = current;
		current = current->next;
		delete temp;
	}

	return 0;
}