//    ************************************************************
//    *                    Pendulum Simulation                   *
//    *                         June 2015                        *
//    ************************************************************
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../include/Pendulum.hh"
#include "../include/Tracer.hh"

#include <iostream>


const float gDisplayx = 800;
const float gDisplayy = 800;
int main() {
  //GAME SETUP
  sf::RenderWindow window(sf::VideoMode(gDisplayx,gDisplayy), "Simulation");
  window.setFramerateLimit(60);

  // Initializing
  Pendulum pendulum( window.getSize().x, window.getSize().y );
  Tracer tracer( window.getSize().x, window.getSize().y );

  // Handling Time
  double t = 0.0;
  double dt = 1.0/60.0;

  // Add a Title:
  sf::Font font;
  sf::Text title;
  if( !font.loadFromFile("fonts/arial.ttf")) {
    std::cerr << "ERROR: Font did not load properly." << std::endl;
  }
  title.setFont(font);
  title.setString("Simple Pendulum Simulation");
  title.setCharacterSize(40);
  title.setColor(sf::Color::Green);
  sf::FloatRect recttemp = title.getLocalBounds();
  title.setPosition( (gDisplayx-recttemp.width)/2.0, recttemp.height);

  sf::Text degrees;
  degrees.setFont(font);
  degrees.setCharacterSize(40);
  degrees.setColor(sf::Color::Green);
  std::string degreesstring;
  degreesstring = pendulum.getThetaKnotString() + " degrees";
  degrees.setString(degreesstring);
  recttemp = degrees.getLocalBounds(); 
  degrees.setPosition( (gDisplayx-recttemp.width)/2.0, 2.5*(recttemp.height));

  bool small = false;
  bool large = false;
  sf::Text smallangle;
  sf::Text RK4;

  float theta_knot = pendulum.getThetaKnot();
  if( theta_knot < 10 ) {
    small = true; 
    smallangle.setFont(font);
    smallangle.setCharacterSize(40);
    smallangle.setColor(sf::Color::Green);
    smallangle.setString("Small Angle Approximation");
    recttemp = smallangle.getLocalBounds();
    smallangle.setPosition( (gDisplayx-recttemp.width)/2.0, 4*(recttemp.height));
  }
  else {
    large = true;
    RK4.setFont(font);
    RK4.setCharacterSize(40);
    RK4.setColor(sf::Color::Green);
    RK4.setString("Large Angle - RK4");
    recttemp = RK4.getLocalBounds();
    RK4.setPosition( (gDisplayx-recttemp.width)/2.0, 4*(recttemp.height));
  }

  while( window.isOpen() ) {
      sf::Event event;
      while( window.pollEvent(event) ) {
	if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
	  window.close();
	}
      }
      // UPDATING

      pendulum.chooseMethod(t,1000.0);

      tracer.setPos(&pendulum);
      tracer.dissolve(&pendulum);

      // DRAWINGS
      window.clear();
      window.draw(title);
      window.draw(degrees);
      if(small)
	window.draw( smallangle );
      if(large)
	window.draw(RK4);
    
      // Tracers
      window.draw(tracer);

      // Pendulum    
      window.draw( pendulum );

      window.display();   

      t+=dt;
 
  }
  return 0;
}
