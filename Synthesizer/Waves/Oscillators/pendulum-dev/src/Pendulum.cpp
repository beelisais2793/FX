#include "../include/Pendulum.hh"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <sstream>
#include <iostream>

Pendulum::Pendulum(float displayx, float displayy) {
  mscreenwidth = displayx;
  mscreenheight = displayy;
  width = 1.0;
  height = 150.0;
  sf::Vector2f pendulumSize(width,height);
  sf::Color pendulumColor = sf::Color( sf::Color(74,235,219) );

  float radius = 3.0;
  origin.setRadius(radius/1.5);
  sf::FloatRect originRect = origin.getLocalBounds();
  origin.setOrigin( (originRect.width)/2.0, (originRect.height)/2.0 );
  origin.setFillColor( pendulumColor );
  origin.setPosition( mscreenwidth/2.0, mscreenheight/2.0 - height/2.0 );  

  bottom.setRadius(2*radius);
  originRect = bottom.getLocalBounds();
  bottom.setOrigin( (originRect.width)/2.0, (originRect.height)/2.0 );
  bottom.setFillColor( pendulumColor );
  
  pendulum.setSize( pendulumSize );
  pendulum.setOrigin( width/2.0, 0 ); // center origin
  pendulum.setFillColor( pendulumColor );
  pendulum.setPosition( mscreenwidth/2.0, mscreenheight/2.0 - height/2.0 );

  // Initial Conditions: theta_knot, theta_knot_dot = 0
  conv = 3.14192 / 180.0;
  srand( time(NULL) );
  theta_knot = (rand() % 180);        //0-180 degrees
  pendulum.setRotation( theta_knot ); //deg
  theta = theta_knot;
  theta_dot = 0.0;
  gravity = 9.8;                     // m/s^s
  omega = 10; //sqrt( gravity / height );   // rad/s
  period = 2.0*3.141592 / omega;

  // RK4 (need radians)
  u1_knot = theta_knot * conv;
  u2_knot = 0.0 * conv;

  //Initialize RK4
  u1_nth=u1_knot;
  u2_nth=u2_knot;

  count = 0;
}

void Pendulum::draw( sf::RenderTarget& target, sf::RenderStates) const {
  target.draw( pendulum );
  target.draw( origin );
  target.draw( bottom );
}

void Pendulum::updatePendulum(double time) {
  timer = time;
  theta = theta_knot*cos( omega*time );
  theta_dot = -omega*theta_knot*sin( omega*time );
  tracerang = theta*conv;
  pendulum.setRotation(theta);
}

void Pendulum::updatePendulumRK4(double time, double g) {
  timer = time;  
  count++;
  if( count % 20 ) {
    double h = 1/g; // step size
    // k,l refers to (d/dt)(u1,u2) = (u2, -u2^2*sin(u1) )
    // step size at end of calculation

    double l0= -pow(omega,2)*sin(u1_nth);
    double l1= -pow(omega,2)*sin(u1_nth+h*0.5*l0);
    double l2= -pow(omega,2)*sin(u1_nth+h*0.5*l1);
    double l3= -pow(omega,2)*sin(u1_nth+h*0.5*l2);
  
    double u2soln= u2_nth+(h/6)*(l0+2*l1+2*l2+l3);

    double k0= u2_nth;
    double k1= u2_nth+h*0.5*k0;
    double k2= u2_nth+h*0.5*k1;
    double k3= u2_nth+h*0.5*k2;

    double u1soln= u1_nth+(h/6)*(k0+2*k1+2*k2+k3);

    u1_nth=u1soln;
    u2_nth=u2soln;

    tracerang = u1_nth;
    pendulum.setRotation( u1_nth / conv );
  }
}

// If sin(theta) ~ theta, then small angle approx is sufficient, else RK4
void Pendulum::chooseMethod(double time, double h) {
  if( theta_knot < 10.0 )
    updatePendulum(time);
  else
    updatePendulumRK4(time,h);
}

void Pendulum::addDrag(double time) {
  ;
}

sf::Vector2f Pendulum::getPendulumPosition() {
  sf::Vector2f temp( -height*sin(tracerang), height*cos(tracerang) );
  sf::Vector2f origin2pendulum( mscreenwidth/2.0, mscreenheight/2.0 - height/2.0 );
  bottom.setPosition( temp+origin2pendulum );
  return temp + origin2pendulum;
}


std::string Pendulum::getThetaKnotString() {
  std::ostringstream temp;
  temp << theta_knot;
  std::string string_temp = temp.str();
  return string_temp;
}
