#include "../include/Tracer.hh"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Tracer::Tracer(float displayx, float displayy) {
  mscreenwidth = displayx;
  mscreenheight = displayy;
  life = 3.0;
  float radius(1.9);
  sf::Color tracercolor = sf::Color::Red;
  sf::Color red = sf::Color::Red;
  sf::Color blue = sf::Color::Blue;
  sf::Color green = sf::Color::Green;
  sf::Color indigo = sf::Color(75,0,130);
  sf::Color yellow = sf::Color::Yellow;
  sf::Color orange = sf::Color(255,128,0);
  sf::Color violet = sf::Color(140,0,255);

  circletracer.setRadius(radius);
  sf::FloatRect originRect = circletracer.getLocalBounds();
  circletracer.setOrigin( (originRect.width)/2.0, (originRect.height)/2.0 );
  circletracer.setFillColor( tracercolor );
}

void Tracer::draw( sf::RenderTarget& target, sf::RenderStates ) const {
  std::vector<sf::CircleShape>::const_iterator cit;
  for( cit = trace.begin(); cit != trace.end(); cit++ ) {
    target.draw( *cit );
  }
}

void Tracer::setPos(Pendulum* ptr) {
  sf::Vector2f x_knot = ptr->getPendulumPosition();
  circletracer.setPosition( x_knot );
  trace.push_back( circletracer );
}

void Tracer::dissolve(Pendulum* ptr) {
  srand(time(NULL));

  sf::Clock timer;
  sftime += timer.getElapsedTime();
  float life = sftime.asSeconds();
  sf::Vector2f temp = ptr->getPendulumPosition();

  for( it=trace.begin(); it != trace.end(); it++) {
    sf::Vector2f tracerposition = (*it).getPosition();
    sf::Vector2f distanceVec = temp - tracerposition;
    float distance = sqrt( pow(distanceVec.x,2) + pow(distanceVec.y,2) );
    float ratio = 255/(0.1*distance);

    tracercolor = sf::Color(255,0,0,ratio);
    (*it).setFillColor( tracercolor );

    if(life > 0.00003) {
      trace.erase(it);
      life = 0;
    }
  }
}
