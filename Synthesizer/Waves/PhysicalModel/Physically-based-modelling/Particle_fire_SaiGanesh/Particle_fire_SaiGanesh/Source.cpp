#include <iostream>
#include <SDL.h>
#include "screen.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Swarm.h"


#undef main

using namespace std;

int main()
{
	srand(time(NULL));

	screen screen;

	if (screen.init() == false) {
		cout << "Error initalizing SDL." << endl;
	}
	
	Swarm swarm;

	//Game loop
	while (true)
	{

		//draw particles

		int elapsed = SDL_GetTicks();

		screen.clear();
		swarm.update();

		unsigned char green = (1 + sin(elapsed * 0.0001)) * 128;
		unsigned char red = (1 + sin(elapsed * 0.0002)) * 128;
		unsigned char blue = (1 + sin(elapsed * 0.0003)) * 128;

		const Particle * const pParticles = swarm.getParticles();
		
		for (int i = 0; i < Swarm::NPARTICLES; i++) {
		
			Particle particle = pParticles[i];
			
			int x = (particle.m_x + 1) * screen::SCREEN_WIDTH / 2;
			int y = (particle.m_y) * screen::SCREEN_WIDTH / 2 + screen::SCREEN_HEIGHT/2;
			screen.setPixel(x, y, red, green, blue);
		}

		//Draw the screen
		screen.update();

		if (screen.processEvents() == false)
		{
			break;
		}

	}

	screen.close();

	int a;
	cin >> a;
	return 0;
}
