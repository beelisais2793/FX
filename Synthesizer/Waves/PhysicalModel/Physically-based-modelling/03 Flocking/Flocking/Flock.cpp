#include "Flock.h"
#define INF 100000.0

Flock::Flock()
{
	boid = new Boid[n_Boids];
	for (int i = 0; i < n_Boids; i++) {
		inactive[i] = n_Boids - 1 - i;
	}

	repelBoid.position = { 150,150,0 };
	leadBoid[0].position = { 100,20,0 };
	leadBoid[1].position = { 100,-20,0 };
}

void Flock::clear() {
	for (int i = 0; i < n_Boids; i++) {
		boid[i].active = false;
		inactive[i] = n_Boids - 1 - i;
	}
}

void Flock::GenerateBoidsPoint() {
	for (int i = 0; i < n_generate; i++) {
		//make the boid active 
		boid[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the boids
		boid[inactive[inactivecount - 1]].init();
		boid[inactive[inactivecount - 1]].position.x = 0; boid[inactive[inactivecount - 1]].position.y = 0; boid[inactive[inactivecount - 1]].position.z = 0;
		boid[inactive[inactivecount - 1]].velocity[0] = 100.0*((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].velocity[1] = 100.0*((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].velocity[2] = 100.0*((float)rand() / RAND_MAX - 0.5);
		//boid[inactive[inactivecount - 1]].velocity[0] = 50.0;
		//boid[inactive[inactivecount - 1]].velocity[1] = 50.0;
		//boid[inactive[inactivecount - 1]].velocity[2] = 50.0;
		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void Flock::GenerateBoidsRectangle() {
	for (int i = 0; i < n_generate; i++) {
		//make the boid active 
		boid[inactive[inactivecount - 1]].active = true;
		//initialize the velocities of the boids
		boid[inactive[inactivecount - 1]].init();
		boid[inactive[inactivecount - 1]].position.x = 100.0* ((float)rand() / RAND_MAX - 0.5); boid[inactive[inactivecount - 1]].position.y = 100.0* ((float)rand() / RAND_MAX - 0.5); boid[inactive[inactivecount - 1]].position.z = 0;
		boid[inactive[inactivecount - 1]].velocity[0] = 0.0;
		boid[inactive[inactivecount - 1]].velocity[1] = 0.0;
		boid[inactive[inactivecount - 1]].velocity[2] = 50.0;
		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void Flock::GenerateBoids(int nos) {
	n_generate = nos;
	//float px = 100.0* ((float)rand() / RAND_MAX - 0.5), py = 100.0* ((float)rand() / RAND_MAX - 0.5), pz = 100.0* ((float)rand() / RAND_MAX - 0.5);
	
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::normal_distribution<float> distribution_r((float)rand() / RAND_MAX, 0.05);
	std::normal_distribution<float> distribution_g((float)rand() / RAND_MAX, 0.05);
	std::normal_distribution<float> distribution_b((float)rand() / RAND_MAX, 0.05);
	
	for (int i = 0; i < n_generate; i++) {

		//make the boid active 
		boid[inactive[inactivecount - 1]].active = true;

		//initialize the lifespan
		boid[inactive[inactivecount - 1]].init();

		boid[inactive[inactivecount - 1]].color[0] = 0.1+(float)rand() / RAND_MAX/3; 
		boid[inactive[inactivecount - 1]].color[1] = 0.1+(float)rand() / RAND_MAX/3; 
		boid[inactive[inactivecount - 1]].color[2] = 0.1+(float)rand() / RAND_MAX/3;

		boid[inactive[inactivecount - 1]].color[0] = distribution_r(generator);
		boid[inactive[inactivecount - 1]].color[1] = distribution_g(generator);
		boid[inactive[inactivecount - 1]].color[2] = distribution_b(generator);

		boid[inactive[inactivecount - 1]].position.x = 50.0* ((float)rand() / RAND_MAX - 0.5)-100;
		boid[inactive[inactivecount - 1]].position.y = 150.0* ((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].position.z = 0.0* ((float)rand() / RAND_MAX - 0.5);

		boid[inactive[inactivecount - 1]].velocity[0] = 50.0* ((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].velocity[1] = 100.0* ((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].velocity[2] = 0.0* ((float)rand() / RAND_MAX - 0.5);

		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
		
	}
}

void Flock::GenerateBoidsDisc(float x, float y, float z, float rad) {

	for (int i = 0; i < n_generate; i++) {

		//make the boid active 
		boid[inactive[inactivecount - 1]].active = true;

		//initialize the velocities of the boids
		boid[inactive[inactivecount - 1]].init();

		std::default_random_engine generator;
		std::normal_distribution<float> distribution(0.5, 0.25);

		float radius = rad* (1 - pow(((float)rand() / RAND_MAX), 3));
		//float radius = rad*distribution(generator);
		float theta = 2.0*PI*((float)rand() / RAND_MAX);
		//boid[inactive[inactivecount - 1]].position.x = x + radius*cos(theta); boid[inactive[inactivecount - 1]].position.y = y; boid[inactive[inactivecount - 1]].position.z = z + radius*sin(theta);
		boid[inactive[inactivecount - 1]].position.x = x; boid[inactive[inactivecount - 1]].position.y = y; boid[inactive[inactivecount - 1]].position.z = z;
		boid[inactive[inactivecount - 1]].velocity[0] = 10.0*((float)rand() / RAND_MAX - 0.5);
		boid[inactive[inactivecount - 1]].velocity[1] = -10.0*((float)rand() / RAND_MAX );
		boid[inactive[inactivecount - 1]].velocity[2] = 5.0*((float)rand() / RAND_MAX - 0.5);

		//popping the inactive array
		inactive[inactivecount - 1] = inactive[inactivecount];
		inactivecount--;
	}
}

void Flock::TestDeactivate() {

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			boid[i].acceleration = { 0.0,0.0,0.0 };
			float vel = 50;
			if (glm::length(boid[i].velocity) > vel) boid[i].velocity = boid[i].velocity / glm::length(boid[i].velocity)*vel;
			if (boid[i].lifespan < 1) {
				boid[i].active = false;
				inactive[inactivecount] = i;
				inactivecount++;

			}	
		}
	}
}

void Flock::ComputeAccAlign() {
	
	glm::vec3 steering = {0.0,0.0,0.0};
	float count, visionRadius = 50;

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			count = 0;
			for (int j = 0; j < n_Boids; j++) {
				if ((i != j) && (boid[j].active == true) && (glm::distance(boid[i].position, boid[j].position) < visionRadius)) {
					steering += boid[j].velocity;
					//printf("%d %d \n", i, j);
					count+=1.0;
				}
			}

			float maxvalue = 10;

			if (count > 0)
			{
				boid[i].acceleration += steering / glm::length(steering)*maxvalue;
			}
			//printf("%f %f %f %f\n", boid[i].acceleration.x, boid[i].acceleration.y, boid[i].acceleration.z,count);
		}
	}
}

void Flock::ComputeAccCohesion() {

	glm::vec3 steering = { 0.0,0.0,0.0 };
	float count, visionRadius = 80;

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			count = 0;
			for (int j = 0; j < n_Boids; j++) {
				if ((i != j) && (boid[j].active == true) && (glm::distance(boid[i].position, boid[j].position) < visionRadius)) {
					steering += boid[j].position;
					//printf("%d %d \n", i, j);
					count += 1.0;
				}
			}

			float maxvalue = 200;

			if (count > 0)
			{
				steering = steering/count - boid[i].position;
				boid[i].acceleration += steering / glm::length(steering)*maxvalue;
			}

		}
	}
}

void Flock::ComputeAccSeparation() {

	glm::vec3 steering = { 0.0,0.0,0.0 };
	float count, visionRadius = 5;
	float maxvalue = 250,d;

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			count = 0;
			for (int j = 0; j < n_Boids; j++) {
				d = glm::distance(boid[i].position, boid[j].position);
				if ((i != j) && (boid[j].active == true) &&  d < visionRadius) {
					steering += (boid[i].position-boid[j].position)/(d*d);
					count += 1.0;
				}
			}
			if (count > 0)
			{
				steering = steering / count;
				boid[i].acceleration += steering * maxvalue;
			}

		}
	}
}

void Flock::FollowLeadParticle() {
	float magnitude = 100;
	for (int k = 0; k < no_leads; k++) {
		for (int i = 0; i < n_Boids; i++) {
			if (boid[i].active == true) {
				boid[i].acceleration += magnitude * (leadBoid[k].position - boid[i].position) / glm::length(leadBoid[k].position - boid[i].position);
			}
		}
	}
	printf("%f ", leadBoid[0].position.x);
}

void Flock::FollowLeadParticleForcely() {
	float magnitude = 3;
	for (int k=0;k<no_leads;k++)
	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			boid[i].velocity += magnitude * (leadBoid[k].position - boid[i].position) / glm::length(leadBoid[k].position - boid[i].position);
		}
	}
}

void Flock::RepelBoid() {
	float magnitude = 100;
	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {
			boid[i].acceleration -= magnitude * (repelBoid.position - boid[i].position) / glm::length(repelBoid.position - boid[i].position);
		}
	}
}

void Flock::integrate(float h, Point *wall, Point * unit_normal, int n_faces) {

	float boundary = 150;
	//Find what would be the new position
	for (int j = 0; j < n_Boids; j++) {
		if (boid[j].active == true) {
			for (int i = 0; i < 3; i++) {
				newPos[j][i] = boid[j].position[i] + h*boid[j].velocity[i];
			}
			timestepmain[j] = h;
		}

		oldpos[j].x = boid[j].position.x; newpos[j].x = newPos[j][0];
		oldpos[j].y = boid[j].position.y; newpos[j].y = newPos[j][1];
		oldpos[j].z = boid[j].position.z; newpos[j].z = newPos[j][2];
	}
	//printf("%d",P->getSign(origin, newpos[0]));
	//printf("%d", newPos[0][0]);
	float tangent_vector;
	//Checking whether the boid would collide in the next time step

	for (int k = 0; k < n_Boids; k++) {
		if (boid[k].active == true) {
		while (timestepmain[k] > 0) {

			if(newPos[k][0]>boundary || newPos[k][0]<-boundary) boid[k].velocity.x = -boid[k].velocity.x;
			if (newPos[k][1]>boundary || newPos[k][1]<-boundary) boid[k].velocity.y = -boid[k].velocity.y;

			
			for (int m = 0; m < n_faces; m++) {

				face[0] = wall[3 * m]; face[1] = wall[3 * m + 1];face[2]= wall[3 * m + 2];
				Plane P(face[0],face[1],face[2]);
				if (!(P.getSign(origin, newpos[k]) == P.getSign(origin, oldpos[k]))) {
					if (isInside_yz(face, 3, newpos[k])|| isInside(face, 3, newpos[k])) {
						//printf("Crossed! \n");
						
						//boid[k].velocity[2] = -boid[k].velocity[2];
						P_velocity = sqrt(pow(boid[k].velocity[0], 2) + pow(boid[k].velocity[1], 2) + pow(boid[k].velocity[2], 2));					
						vnorm[0] = P_velocity * unit_normal[m].x;
						vnorm[1] = P_velocity * unit_normal[m].y;
						vnorm[2] = P_velocity * unit_normal[m].z;

						//printf("%f %f %f \n", unit_normal[m].x, boid[k].velocity[1], boid[k].velocity[2]);
						boid[k].velocity[0] = boid[k].velocity[0] -  vnorm[0] ;
						boid[k].velocity[1] = boid[k].velocity[1] - vnorm[1] ;
						boid[k].velocity[2] = boid[k].velocity[2] -  vnorm[2] ;
						boid[k].update(h, d, wind, Gravity);
						//if(k==1)
						//printf("%f %f %f \n", boid[k].velocity[0], boid[k].velocity[1], boid[k].velocity[2]);
						timestepmain[k] = 0;
					}
				}
			}
			/*
			
			if (newPos[k][0] > (boxxh - 10)) {

				f = ((boxxh - 10) - boid[k].position.x) / (newPos[k][0] - boid[k].position.x);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[0] = -boid[k].velocity[0];
				tangent_vector = sqrt((boid[k].velocity[1] * boid[k].velocity[1]) + (boid[k].velocity[2] * boid[k].velocity[2]));
				if (tangent_vector != 0) {
					boid[k].velocity[1] = boid[k].velocity[1] - min(mu*abs(boid[k].velocity[0]), tangent_vector)*boid[k].velocity[1] / tangent_vector;
					boid[k].velocity[2] = boid[k].velocity[2] - min(mu*abs(boid[k].velocity[0]), tangent_vector)*boid[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][0] < (boxxl + 10)) {

				f = ((boxxl + 10) - boid[k].position.x) / (newPos[k][0] - boid[k].position.x);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[0] = -boid[k].velocity[0];
				tangent_vector = sqrt((boid[k].velocity[1] * boid[k].velocity[1]) + (boid[k].velocity[2] * boid[k].velocity[2]));
				if (tangent_vector != 0) {
					boid[k].velocity[1] = boid[k].velocity[1] - min(mu*abs(boid[k].velocity[0]), tangent_vector)*boid[k].velocity[1] / tangent_vector;
					boid[k].velocity[2] = boid[k].velocity[2] - min(mu*abs(boid[k].velocity[0]), tangent_vector)*boid[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][1] > (boxyh - 10)) {

				f = ((boxyh - 10) - boid[k].position.y) / (newPos[k][1] - boid[k].position.y);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[1] = -boid[k].velocity[1];
				tangent_vector = sqrt((boid[k].velocity[0] * boid[k].velocity[0]) + (boid[k].velocity[2] * boid[k].velocity[2]));
				if (tangent_vector != 0) {
					boid[k].velocity[0] = boid[k].velocity[0] - min(mu*abs(boid[k].velocity[1]), tangent_vector)*boid[k].velocity[0] / tangent_vector;
					boid[k].velocity[2] = boid[k].velocity[2] - min(mu*abs(boid[k].velocity[1]), tangent_vector)*boid[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][1] < (boxyl + 10)) {
				f = ((boxyl + 10) - boid[k].position.y) / (newPos[k][1] - boid[k].position.y);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[1] = -boid[k].velocity[1];
				tangent_vector = sqrt((boid[k].velocity[0] * boid[k].velocity[0]) + (boid[k].velocity[2] * boid[k].velocity[2]));
				if (tangent_vector != 0) {
					boid[k].velocity[0] = boid[k].velocity[0] - min(mu*abs(boid[k].velocity[1]), tangent_vector)*boid[k].velocity[0] / tangent_vector;
					boid[k].velocity[2] = boid[k].velocity[2] - min(mu*abs(boid[k].velocity[1]), tangent_vector)*boid[k].velocity[2] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];
			}
			else if (newPos[k][2] > (boxzh - 10)) {
				f = ((boxzh - 10) - boid[k].position.z) / (newPos[k][2] - boid[k].position.z);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[2] = -boid[k].velocity[2];
				tangent_vector = sqrt((boid[k].velocity[0] * boid[k].velocity[0]) + (boid[k].velocity[1] * boid[k].velocity[1]));
				if (tangent_vector != 0) {
					boid[k].velocity[0] = boid[k].velocity[0] - min(mu*abs(boid[k].velocity[2]), tangent_vector)*boid[k].velocity[0] / tangent_vector;
					boid[k].velocity[1] = boid[k].velocity[1] - min(mu*abs(boid[k].velocity[2]), tangent_vector)*boid[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];

			}
			else if (newPos[k][2] < (boxzl + 10)) {
				f = ((boxzl + 10) - boid[k].position.z) / (newPos[k][2] - boid[k].position.z);
				boid[k].update(f*h, d, wind, Gravity);
				for (int i = 0; i < 3; i++) {
					newPos[k][i] = boid[k].getPos()[i];
				}
				boid[k].velocity[2] = -boid[k].velocity[2];
				tangent_vector = sqrt((boid[k].velocity[0] * boid[k].velocity[0]) + (boid[k].velocity[1] * boid[k].velocity[1]));
				if (tangent_vector != 0) {
					boid[k].velocity[0] = boid[k].velocity[0] - min(mu*abs(boid[k].velocity[2]), tangent_vector)*boid[k].velocity[0] / tangent_vector;
					boid[k].velocity[1] = boid[k].velocity[1] - min(mu*abs(boid[k].velocity[2]), tangent_vector)*boid[k].velocity[1] / tangent_vector;
				}
				timestepmain[k] = (1 - f)*timestepmain[k];

			}
			*/

			if (timestepmain[k] != 0) //if not just update the state to the next time step
			{

				boid[k].update(timestepmain[k], d, wind, Gravity);
				//if(k==1)
				//printf("%f %f %f\n",boid[k].velocity[0], boid[k].velocity[1], boid[k].velocity[2]);
				timestepmain[k] = 0;
			}

		}

		boid[k].lifespan--;
		}
	}//End of for loop

}

void Flock::SphericalObstacle(float radius, float safe_radius, float threashold_time, glm::vec3 center) {

	float R = radius + safe_radius, s_close, Dc, d, vs, Dt, Vt, Tt;
	glm::vec3 Xis, Xclose, vPerp, Xt;

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {

			Xis = center - boid[i].position;
			s_close = glm::dot(Xis, boid[i].velocity / glm::length(boid[i].velocity));

			//printf("%f \n", s_close);
			if (s_close >= 0) {

				Dc = threashold_time * glm::length(boid[i].velocity);

				if (s_close <= Dc) {

					Xclose = boid[i].position + s_close*boid[i].velocity / glm::length(boid[i].velocity);
					d = glm::length(Xclose - center);

					if (d <= R) {
						//printf("%f \n", d);
						vPerp = Xclose - center; vPerp = vPerp / glm::length(vPerp);
						Xt = center + R*vPerp;
						Dt = glm::length(Xt - boid[i].position);
						Vt = glm::dot(boid[i].velocity, Xt - boid[i].position)/Dt;
						Tt = Dt / Vt;
						vs = glm::length(glm::cross((boid[i].velocity / glm::length(boid[i].velocity)), (Xt - boid[i].position))) / Tt;
						//vs = glm::length(glm::cross((boid[i].velocity / glm::length(boid[i].velocity)), (Xt - boid[i].position)))*glm::dot(boid[i].velocity, Xt - boid[i].position)/pow(glm::length(Xt-boid[i].position),2);
						boid[i].acceleration += 5 * vs / Tt * vPerp;
					}
				}
			}
		}
	}
}

void Flock::LineStop(glm::vec3 p1, glm::vec3 p2) {
	
	Point P1 = { p1.x,p1.y,p1.z }, P2 = { p1.x,p1.y,p1.z };
	float d, magnitude = 10000;
	glm::vec3 normal, b;

	for (int i = 0; i < n_Boids; i++) {
		if (boid[i].active == true) {

			b = glm::dot((p2 - p1), boid[i].position - p1)*(p2 - p1) / pow(glm::length(p2 - p1), 2);
			d = glm::length( boid[i].position - p1 - b);
			normal = (boid[i].position - p1 - b) ;
			
			
			//if ((glm::length(b)<glm::length(p1-p2))&&(d<10)) {
			if (onSegment_glm(p1,boid[i].position-normal,p2) && (d<10)) {
				boid[i].acceleration = normal / (d*d*d*d / magnitude);

				//boid[i].acceleration /= glm::length(boid[i].acceleration) * 10;
				//printf("%f \n", glm::length(glm::cross((boid[i].position - p1), (p1 - p2))) / glm::length(p1 - p2));

				
			}
		}
	}
}

void Flock::Obstacles(std::vector<glm::vec3> vertices, int noVertices) {
	for (int i = 0; i < noVertices-1; i++) {
		LineStop(vertices[i], vertices[i + 1]);
	}
}

Flock::~Flock()
{
	delete[] boid;
}
