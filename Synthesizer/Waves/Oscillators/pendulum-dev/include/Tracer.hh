#ifndef TRACER_HH
#define TRACER_HH

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>


#include "../include/Pendulum.hh"

class Pendulum;

class Tracer : public sf::Transformable, public sf::Drawable
{
private:
  float mscreenwidth, mscreenheight;
  sf::CircleShape circletracer;
  std::vector<sf::CircleShape> trace;
  std::vector<sf::CircleShape>::iterator it;
  sf::Color tracercolor;
  float life;
  sf::Time sftime;

  sf::Color red;
  sf::Color blue;
  sf::Color green;
  sf::Color indigo;
  sf::Color yellow;
  sf::Color orange;
  sf::Color violet;

public:
  Tracer(float,float);
  ~Tracer() {};
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void setPos(Pendulum*);
  void dissolve(Pendulum*);

};
#endif
