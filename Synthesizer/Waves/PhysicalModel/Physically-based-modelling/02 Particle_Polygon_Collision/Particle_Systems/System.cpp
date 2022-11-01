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

void System::GenerateParticlesPoint() {
	for (int i = 0; i < n_generate; i++) {
		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
		particle[inactive[inactivecount - 1]].position[0] = 0; particle[inactive[inactivecount - 1]].position[1] = 0; particle[inactive[inactivecount - 1]].position[2] = 0;
		particle[inactive[inactivecount - 1]].velocity[0] = 100.0*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[1] = 100.0*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[2] = 100.0*((float)rand() / RAND_MAX - 0.5);
		//particle[inactive[inactivecount - 1]].velocity[0] = 50.0;
		//particle[inactive[inactivecount - 1]].velocity[1] = 50.0;
		//particle[inactive[inactivecount - 1]].velocity[2] = 50.0;
		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void System::GenerateParticlesDisc() {
	for (int i = 0; i < n_generate; i++) {
		//make the particle active 
		particle[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the particles
		particle[inactive[inactivecount - 1]].init();
		particle[inactive[inactivecount - 1]].position[0] = 50.0* ((float)rand() / RAND_MAX - 0.5); particle[inactive[inactivecount - 1]].position[1] = 50.0* ((float)rand() / RAND_MAX - 0.5); particle[inactive[inactivecount - 1]].position[2] = 50.0* ((float)rand() / RAND_MAX - 0.5);
		//particle[inactive[inactivecount - 1]].velocity[0] = 100.0*((float)rand() / RAND_MAX - 0.5);
		//particle[inactive[inactivecount - 1]].velocity[1] = 100.0*((float)rand() / RAND_MAX - 0.5);
		//particle[inactive[inactivecount - 1]].velocity[2] = 100.0*((float)rand() / RAND_MAX - 0.5);
		particle[inactive[inactivecount - 1]].velocity[0] = 50.0;
		particle[inactive[inactivecount - 1]].velocity[1] = 50.0;
		particle[inactive[inactivecount - 1]].velocity[2] = 50.0;
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

void System::ComputeAcc() {

	for (int i = 0; i < n_particles; i++) {
		if (particle[i].active == true) {
			/*
			distance_from_center = sqrt((particle[i].position[0]-50.0)* (particle[i].position[0]-50.0)+ particle[i].position[1] * particle[i].position[1]+ particle[i].position[2] * particle[i].position[2]);
			particle[i].acceleration[0] = -100000.0*(particle[i].position[0]-50.0) / pow(distance_from_center, 3);
			particle[i].acceleration[1] = -100000.0*particle[i].position[1] / pow(distance_from_center, 3);
			particle[i].acceleration[2] = -100000.0*particle[i].position[2] / pow(distance_from_center, 3);
			*/
			//printf("%f \n", particle[i].acceleration[0]);

			distance_from_center = sqrt((particle[i].position[0]-50)* (particle[i].position[0]-50) + particle[i].position[1] * particle[i].position[1]);
			particle[i].acceleration[0] = -10000.0*(particle[i].position[0]-50) / pow(distance_from_center, 3);
			particle[i].acceleration[1] = -10000.0*(particle[i].position[1]) / pow(distance_from_center, 3);
		}
	}
}

void System::integrate(float h, Point *wall, Point * unit_normal, int n_faces) {

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

						vnorm[0] = particle[k].velocity[0] * unit_normal[m].x;
						vnorm[1] = particle[k].velocity[1] * unit_normal[m].y;
						vnorm[2] = particle[k].velocity[2] * unit_normal[m].z;

						//printf("%f %f %f \n", vnorm[0], particle[k].velocity[1], particle[k].velocity[2]);
						particle[k].velocity[0] = particle[k].velocity[0] - 2.0 * vnorm[0] / unit_normal[m].x;
						particle[k].velocity[1] = particle[k].velocity[1] - 2.0 * vnorm[1] / unit_normal[m].y;
						particle[k].velocity[2] = particle[k].velocity[2] - 2.0 * vnorm[2] / unit_normal[m].z;
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
