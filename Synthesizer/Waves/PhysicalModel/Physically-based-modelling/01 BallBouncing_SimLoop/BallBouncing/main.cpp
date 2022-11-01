#include "components.h"
#include "components.cpp"
#include "gameloop.cpp"

/* trigger function that will print to the terminal when called
 *
 * @param (Collider* c1) first collider involved in collision
 * @param (Collider* cc) first second involved in collision
*/
void printOnCollide(Collider* c1, Collider* c2){
  printf("collide");
}

/* trigger function that will freeze the colliding balls and change their colors when called
 *
 * @param (Collider* c1) first collider involved in collision
 * @param (Collider* cc) first second involved in collision
*/
void freezeTag(Collider* c1, Collider* c2){
  // Freeze both objects
  Physics* p1 = (Physics*) c1->parent->getComponent("Physics").front();
  Physics* p2 = (Physics*) c2->parent->getComponent("Physics").front();
  p1->dx = 0;
  p1->dy = 0;
  p2->dx = 0;
  p2->dy = 0;

  // Change their color to red
  CircleRender* cr1 = (CircleRender*) c1->parent->getComponent("CircleRender").front();
  CircleRender* cr2 = (CircleRender*) c2->parent->getComponent("CircleRender").front();
  cr1->setColor(1, 0, 0);
  cr2->setColor(1, 0, 0);
}

/* trigger function that cause the balls to "bounce" when called
 *
 * @param (Collider* c1) first collider involved in collision
 * @param (Collider* cc) first second involved in collision
*/
void Bounce(Collider* c1, Collider* c2, float dt){
  // Access the physics components of both balls
  Physics* p1 = (Physics*) c1->parent->getComponent("Physics").front();
  Physics* p2 = (Physics*) c2->parent->getComponent("Physics").front();

  // Calculate new velocities
  float newv1x = (p1->dx * (p1->mass - p2->mass) +
    (2 * p2->mass * p2->dx)) / (p1->mass + p2->mass);
  float newv1y = (p1->dy * (p1->mass - p2->mass) +
    (2 * p2->mass * p2->dy)) / (p1->mass + p2->mass);

  float newv2x = (p2->dx * (p2->mass - p1->mass) +
    (2 * p1->mass * p1->dx)) / (p1->mass + p2->mass);
  float newv2y = (p2->dy * (p2->mass - p1->mass) +
    (2 * p1->mass * p1->dy)) / (p1->mass + p2->mass);

    // Assign velocities
    p1->dx = newv1x;
    p1->dy = newv1y;

    p2->dx = newv2x;
    p2->dy = newv2y;

    // Back the balls off by one step of their new velocities so that collision does not register twice
    p1->parent->x += newv1x * dt;
    p1->parent->y += newv1y * dt;

    p2->parent->x += newv2x * dt;
    p2->parent->y += newv2y * dt;
}

/* Creates a ball GameObject and associated components
 *
 * @param (double x) the starting x position of the ball
 * @param (double y) the starting y position of the ball
 * @param (double dx) the starting x speed of the ball
 * @param (double dy) the starting y speed of the ball
 * @param (double radius) the starting radius of the ball
*/
GameObject* createBall(double x, double y, double dx, double dy, double radius) {
  float mass = PI * pow(radius, 2);
  GameObject* obj = new GameObject(x, y);
  CircleRender* circleRender = new CircleRender(obj, radius);
  Collider* collider = new Collider(obj, radius);
  Physics* physics = new Physics(obj, dx, dy, mass);
  WallBounceScript* wallBounceScript = new WallBounceScript(obj, radius);
  collider->addTrigger(Bounce);
  return obj;
}

/* Creates a number of balls and starts the game loop
 *
*/
int main() {
  // Set up objects
  createBall(.5, .5, -.00045, 0, .1);
  createBall(-.25, .5, .00045, 0, .2);
  createBall(-.75, .45, .0001, .0002, .1);
  createBall(0, 0, .0007, -.00005, .15);
  createBall(.6, -.45, .0003, -.0002, .05);
  createBall(-.35, -.45, .0003, -.0002, .05);

  // Run loop
  ODLGameLoop_initOpenGL();
}
