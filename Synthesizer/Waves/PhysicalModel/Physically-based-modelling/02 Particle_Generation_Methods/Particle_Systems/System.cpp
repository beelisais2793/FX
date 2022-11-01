#include "System.h"

System::System()
{
	particle = new Particle[n_particles];
	for (int i = 0; i < n_particles; i++) {
		inactive[i] = n_particles - 1 - i;
	}
}

void System::clear() {
	for (int i = 0; i < n_particles; i++) {
		particle[i].active = false;
		inactive[i] = n_particles - 1 - i;
	}
}

void System::GenerateParticlesPoint(float px, float py, float pz, float vx, float vy, float vz) {
	for (int i = 0; i < n_generate; i++) {

		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;

		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
		particle[inactive[inactivecount - 1]].position[0] = px; particle[inactive[inactivecount - 1]].position[1] = py; particle[inactive[inactivecount - 1]].position[2] = pz;
		particle[inactive[inactivecount - 1]].velocity[0] = vx*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[1] = vy*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[2] = vz*((float)rand() / RAND_MAX - 0.5);
		//particle[inactive[inactivecount - 1]].velocity[0] = -50.0;
		//particle[inactive[inactivecount - 1]].velocity[1] = 0.0;
		//particle[inactive[inactivecount - 1]].velocity[2] = 0.0;
		//popping the inactive array

		particle[inactive[inactivecount - 1]].acceleration[1] = -10.0;

		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void System::GenerateParticlesDirected(float px, float py, float pz, float vx, float vy, float vz) {
	for (int i = 0; i < n_generate; i++) {
		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
		particle[inactive[inactivecount - 1]].position[0] = px; particle[inactive[inactivecount - 1]].position[1] = py; particle[inactive[inactivecount - 1]].position[2] = pz;
		particle[inactive[inactivecount - 1]].velocity[0] = vx+ 10*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[1] = vy+ 10*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[2] = vz+ 10*((float)rand() / RAND_MAX - 0.5);

		particle[inactive[inactivecount - 1]].color[0] = 0.8; particle[inactive[inactivecount - 1]].color[1] = 0.34; particle[inactive[inactivecount - 1]].color[2] = 0.18;
		particle[inactive[inactivecount - 1]].acceleration[1] = -10.0;

		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void System::GenerateParticlesRectangle() {
	for (int i = 0; i < n_generate; i++) {
		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
		particle[inactive[inactivecount - 1]].position[0] = 100.0* ((float)rand() / RAND_MAX - 0.5); particle[inactive[inactivecount - 1]].position[1] = 100.0* ((float)rand() / RAND_MAX - 0.5); particle[inactive[inactivecount - 1]].position[2] = 0;
		particle[inactive[inactivecount - 1]].velocity[0] = 0.0;
		particle[inactive[inactivecount - 1]].velocity[1] = 0.0;
		particle[inactive[inactivecount - 1]].velocity[2] = 50.0;
		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void System::GenerateParticlesDisc(float x, float y, float z, float rad) {
	
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::normal_distribution<float> distribution(1, 0.1);

	for (int i = 0; i < n_generate; i++) {

		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;

		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
	
		//float radius = rad* (1 - pow(((float)rand() / RAND_MAX), 3));
		float radius = rad*distribution(generator);
		float theta = 2.0*PI*((float)rand() / RAND_MAX);

		particle[inactive[inactivecount - 1]].position[0] = x + radius*cos(theta); particle[inactive[inactivecount - 1]].position[1] = y; particle[inactive[inactivecount - 1]].position[2] = z + radius*sin(theta);
		//particle[inactive[inactivecount - 1]].position[0] = x; particle[inactive[inactivecount - 1]].position[1] = y; particle[inactive[inactivecount - 1]].position[2] = z;
		particle[inactive[inactivecount - 1]].velocity[0] = 0.0*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[1] = -10.0*((float)rand() / RAND_MAX );
		particle[inactive[inactivecount - 1]].velocity[2] = 0.0*((float)rand() / RAND_MAX - 0.5);

		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void System::TestDeactivate() {
	for (int i = 0; i < n_particles; i++) {
		if (particle[i].active == true) {
			if (particle[i].lifespan < 1) {
				particle[i].active = false;
				inactive[inactivecount] = i;
				inactivecount++;
			}
		}
	}
}

void System::ComputeAccLennard(float sigma) {

	float lennard;

	for (int i = 0; i < n_particles; i++) {
		if (particle[i].active == true) {
			/*
			distance_from_center = sqrt((particle[i].position[0]-50.0)* (particle[i].position[0]-50.0)+ particle[i].position[1] * particle[i].position[1]+ particle[i].position[2] * particle[i].position[2]);
			particle[i].acceleration[0] = -100000.0*(particle[i].position[0]-50.0) / pow(distance_from_center, 3);
			particle[i].acceleration[1] = -100000.0*particle[i].position[1] / pow(distance_from_center, 3);
			particle[i].acceleration[2] = -100000.0*particle[i].position[2] / pow(distance_from_center, 3);
			*/
			//printf("%f \n", particle[i].acceleration[0]);

			//about a line
			distance_from_center = sqrt((particle[i].position[0])* (particle[i].position[0]) + particle[i].position[1] * particle[i].position[1]);
			particle[i].acceleration[0] = -1000.0*(particle[i].position[0]) / pow(distance_from_center, 2);
			particle[i].acceleration[1] = -1000.0*(particle[i].position[1]) / pow(distance_from_center, 2);

			//Lennard-Jones
			distance_from_center = sqrt((particle[i].position[0])* (particle[i].position[0]) + particle[i].position[1] * particle[i].position[1]+ (particle[i].position[2])* (particle[i].position[2]));
			lennard = (pow((sigma / distance_from_center), 4) - pow((sigma / distance_from_center), 2));
			particle[i].acceleration[0] = 400*lennard*(particle[i].position[0]) / distance_from_center;
			particle[i].acceleration[1] = 400*lennard*(particle[i].position[1]) / distance_from_center;
			particle[i].acceleration[2] = 400*lennard*(particle[i].position[2]) / distance_from_center;

			//particle[i].acceleration[1] = -10.0;
		}
	}
}

void System::ComputeAccLine() {


	for (int i = 0; i < n_particles; i++) {
		if (particle[i].active == true) {


			//about a line
			distance_from_center = sqrt((particle[i].position[0])* (particle[i].position[0]) + particle[i].position[2] * particle[i].position[2]);
			particle[i].acceleration[0] = -1000.0*(particle[i].position[0]) / pow(distance_from_center, 2);
			particle[i].acceleration[2] = -1000.0*(particle[i].position[1]) / pow(distance_from_center, 2);
			particle[i].acceleration[1] = 0;
		}
	}
}

void System::SurfaceSampling(Point *wall, int n_faces) {
	float a, b;

	for (int m = 0; m < n_faces; m++) {

		face[0] = wall[3 * m]; face[1] = wall[3 * m + 1]; face[2] = wall[3 * m + 2];
		for (int i = 0; i < 1; i++) {

			particle[inactive[inactivecount - 1]].radius = 3;
			//make the particle active 
			particle[inactive[inactivecount - 1]].active = true;

			//initialize the velocities of the particles
			particle[inactive[inactivecount - 1]].init();

			particle[inactive[inactivecount - 1]].position[0] = (face[0].x+ face[1].x+ face[2].x)/3; particle[inactive[inactivecount - 1]].position[1] = (face[0].y + face[1].y + face[2].y) / 3; particle[inactive[inactivecount - 1]].position[2] = (face[0].z + face[1].z + face[2].z) / 3;
			//particle[inactive[inactivecount - 1]].velocity[0] = 0.0*((float)rand() / RAND_MAX - 0.5);
			//particle[inactive[inactivecount - 1]].velocity[1] = 0.0*((float)rand() / RAND_MAX);
			//particle[inactive[inactivecount - 1]].velocity[2] = 0.0*((float)rand() / RAND_MAX - 0.5);
			particle[inactive[inactivecount - 1]].acceleration[1] = -10.0;
			//popping the inactive array
			inactive[inactivecount - 1] = inactive[inactivecount];
			inactivecount--;
		}
	}
}

void System::ComputeAccPlane() {
	for (int i = 0; i < n_particles; i++) {

		if (particle[i].active == true) {

			//particle[i].acceleration[0] = (particle[i].position[0]+200);
			particle[i].acceleration[1] = (particle[i].position[1] - 200);
			particle[i].acceleration[1] = (particle[i].position[1] + 50);

		}
	}
}

void System::integrate(float h, Point *wall, Point * unit_normal, int n_faces, float e) {

	//Find what would be the new position
	for (int j = 0; j < n_particles; j++) {
		if (particle[j].active == true) {
			for (int i = 0; i < 3; i++) {
				newPos[j][i] = particle[j].getPos()[i] + h*particle[j].velocity[i];
			}
			timestepmain[j] = h;
		}

		oldpos[j].x = particle[j].getPos()[0]; newpos[j].x = newPos[j][0];
		oldpos[j].y = particle[j].getPos()[1]; newpos[j].y = newPos[j][1];
		oldpos[j].z = particle[j].getPos()[2]; newpos[j].z = newPos[j][2];
	}
	//printf("%d",P->getSign(origin, newpos[0]));
	//printf("%d", newPos[0][0]);
	float tangent_vector;
	//Checking whether the particle would collide in the next time step

	for (int k = 0; k < n_particles; k++) {
		if (particle[k].active == true) {
		while (timestepmain[k] > 0) {
			
			for (int m = 0; m < n_faces; m++) {

				face[0] = wall[3 * m]; face[1] = wall[3 * m + 1];face[2]= wall[3 * m + 2];
				Plane P(face[0],face[1],face[2]);
				if (!(P.getSign(origin, newpos[k]) == P.getSign(origin, oldpos[k]))) {
					if (isInside_yz(face, 3, newpos[k])|| isInside(face, 3, newpos[k])) {
						//printf("Crossed! \n");
						
						//particle[k].velocity[2] = -particle[k].velocity[2];
						P_velocity = sqrt(pow(particle[k].velocity[0], 2) + pow(particle[k].velocity[1], 2) + pow(particle[k].velocity[2], 2));					
						vnorm[0] = P_velocity * unit_normal[m].x;
						vnorm[1] = P_velocity * unit_normal[m].y;
						vnorm[2] = P_velocity * unit_normal[m].z;

						//printf("%f %f %f \n", unit_normal[m].x, particle[k].velocity[1], particle[k].velocity[2]);
						particle[k].velocity[0] = particle[k].velocity[0] -  vnorm[0] - e*vnorm[0];
						particle[k].velocity[1] = particle[k].velocity[1] - vnorm[1] - e*vnorm[1];
						particle[k].velocity[2] = particle[k].velocity[2] -  vnorm[2] - e*vnorm[2];
						particle[k].update(h, d, wind, Gravity);
						//if(k==1)
						//printf("%f %f %f \n", particle[k].velocity[0], particle[k].velocity[1], particle[k].velocity[2]);
						timestepmain[k] = 0;
					}
				}
			}
			
			/*
			if (newPos[k][0] > (boxxh - 10)) {

				f = ((boxxh - 10) - particle[k].getPos()[0]) / (newPos[k][0] - particle[k].getPos()[0]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[0] = -particle[k].velocity[0];
				tangent_vector = sqrt((particle[k].velocity[1] * particle[k].velocity[1]) + (particle[k].velocity[2] * particle[k].velocity[2]));
				if (tangent_vector != 0) {
					particle[k].velocity[1] = particle[k].velocity[1] - min(mu*abs(particle[k].velocity[0]), tangent_vector)*particle[k].velocity[1] / tangent_vector;
					particle[k].velocity[2] = particle[k].velocity[2] - min(mu*abs(particle[k].velocity[0]), tangent_vector)*particle[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][0] < (boxxl + 10)) {

				f = ((boxxl + 10) - particle[k].getPos()[0]) / (newPos[k][0] - particle[k].getPos()[0]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[0] = -particle[k].velocity[0];
				tangent_vector = sqrt((particle[k].velocity[1] * particle[k].velocity[1]) + (particle[k].velocity[2] * particle[k].velocity[2]));
				if (tangent_vector != 0) {
					particle[k].velocity[1] = particle[k].velocity[1] - min(mu*abs(particle[k].velocity[0]), tangent_vector)*particle[k].velocity[1] / tangent_vector;
					particle[k].velocity[2] = particle[k].velocity[2] - min(mu*abs(particle[k].velocity[0]), tangent_vector)*particle[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][1] > (boxyh - 10)) {

				f = ((boxyh - 10) - particle[k].getPos()[1]) / (newPos[k][1] - particle[k].getPos()[1]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[1] = -particle[k].velocity[1];
				tangent_vector = sqrt((particle[k].velocity[0] * particle[k].velocity[0]) + (particle[k].velocity[2] * particle[k].velocity[2]));
				if (tangent_vector != 0) {
					particle[k].velocity[0] = particle[k].velocity[0] - min(mu*abs(particle[k].velocity[1]), tangent_vector)*particle[k].velocity[0] / tangent_vector;
					particle[k].velocity[2] = particle[k].velocity[2] - min(mu*abs(particle[k].velocity[1]), tangent_vector)*particle[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][1] < (boxyl + 10)) {
				f = ((boxyl + 10) - particle[k].getPos()[1]) / (newPos[k][1] - particle[k].getPos()[1]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[1] = -particle[k].velocity[1];
				tangent_vector = sqrt((particle[k].velocity[0] * particle[k].velocity[0]) + (particle[k].velocity[2] * particle[k].velocity[2]));
				if (tangent_vector != 0) {
					particle[k].velocity[0] = particle[k].velocity[0] - min(mu*abs(particle[k].velocity[1]), tangent_vector)*particle[k].velocity[0] / tangent_vector;
					particle[k].velocity[2] = particle[k].velocity[2] - min(mu*abs(particle[k].velocity[1]), tangent_vector)*particle[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][2] > (boxzh - 10)) {
				f = ((boxzh - 10) - particle[k].getPos()[2]) / (newPos[k][2] - particle[k].getPos()[2]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[2] = -particle[k].velocity[2];
				tangent_vector = sqrt((particle[k].velocity[0] * particle[k].velocity[0]) + (particle[k].velocity[1] * particle[k].velocity[1]));
				if (tangent_vector != 0) {
					particle[k].velocity[0] = particle[k].velocity[0] - min(mu*abs(particle[k].velocity[2]), tangent_vector)*particle[k].velocity[0] / tangent_vector;
					particle[k].velocity[1] = particle[k].velocity[1] - min(mu*abs(particle[k].velocity[2]), tangent_vector)*particle[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];

			}
			else if (newPos[k][2] < (boxzl + 10)) {
				f = ((boxzl + 10) - particle[k].getPos()[2]) / (newPos[k][2] - particle[k].getPos()[2]);
				particle[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = particle[k].getPos()[i];
				}
				particle[k].velocity[2] = -particle[k].velocity[2];
				tangent_vector = sqrt((particle[k].velocity[0] * particle[k].velocity[0]) + (particle[k].velocity[1] * particle[k].velocity[1]));
				if (tangent_vector != 0) {
					particle[k].velocity[0] = particle[k].velocity[0] - min(mu*abs(particle[k].velocity[2]), tangent_vector)*particle[k].velocity[0] / tangent_vector;
					particle[k].velocity[1] = particle[k].velocity[1] - min(mu*abs(particle[k].velocity[2]), tangent_vector)*particle[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];

			}
			*/

			if (timestepmain[k] != 0) //if not just update the state to the next time step
			{

				particle[k].update(timestepmain[k], d, wind, Gravity);
				//if(k==1)
				//printf("%f %f %f\n",particle[k].velocity[0], particle[k].velocity[1], particle[k].velocity[2]);
				timestepmain[k] = 0;
			}

		}

		particle[k].lifespan--;
		}
	}//End of for loop

}

System::~System()
{
	delete[] particle;
}
