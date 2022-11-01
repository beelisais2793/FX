#ifndef PENDULUM_HH
#define PENDULUM_HH

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <fstream>

class Pendulum : public sf::Drawable, public sf::Transformable
{
private:
  sf::Vector2f pendulumSize;
  float width,height;
  float mscreenwidth, mscreenheight;
  sf::Color pendulumColor;
  sf::RectangleShape pendulum;
  sf::CircleShape origin,bottom;
  double theta_knot;
  double omega;
  double gravity;
  double drag;
  double theta;
  double theta_dot;
  double timer;
  double conv,tracerang;
  double period;
  int count;

  //RK4
  double u1_knot, u2_knot, u1_nth, u2_nth;
public:
  Pendulum(float,float);
  ~Pendulum() {};
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  sf::Vector2f getpendulumSize() { return pendulumSize; }
  void updatePendulum(double);
  void updatePendulumRK4(double,double);
  void chooseMethod(double,double);
  void addDrag(double);
  sf::Vector2f getPendulumPosition();
  std::string getThetaKnotString();
  double getThetaKnot() {return theta_knot;}
};
#endif
