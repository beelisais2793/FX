#pragma once
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "geometry.h"

#include <tuple>
#include <vector>


class Body
{

public:

	float m;
	glm::vec3 X, V, w, L, p;
	glm::mat3 I0, I0_inv, R, I;
	glm::quat q;

	std::vector <glm::vec3> vertices;
	std::vector<std::vector<int>> faces;
	std::vector<std::vector<int>> edge;
	float Cr = 1;

	Body() {
		V = { 0,0,0 };
	};

	Body(const char* input) {
		
		if (strcmp(input, "cuboid")==0) {

			float l = 50, b = 50, h = 50;

			X = { 0,0,0 }; m = 1; p = { 0.0f ,20.0f, 0.0 }; L = { 400.0,0,0.0 };
			
			float I_arr[9] = { m / 12 * (b*b + h*h), 0, 0,
				0, m / 12 * (l*l + h*h), 0,
				0, 0, m / 12 * (l*l + b*b) };

			I0 = glm::make_mat3(I_arr);
			I0_inv = glm::inverse(I0);

			q = glm::angleAxis((float)3.14/3, glm::vec3(0,1,1));

			R = glm::toMat3(q);
			//w = R*I0_inv* glm::transpose(R)*L;
			
		
			vertices.push_back({ 0.0,0.0,0.0 });
			vertices.push_back({ l,0,0 });
			vertices.push_back({ 0,b,0 });
			vertices.push_back({ 0,0,h });
			vertices.push_back({ l,b,0 });
			vertices.push_back({ l,0,h });
			vertices.push_back({ 0,b,h });
			vertices.push_back({ l,b,h });

			for (int i = 0; i < vertices.size(); i++) {
				vertices[i][0] -= l / 2;
				vertices[i][1] -= b / 2;
				vertices[i][2] -= h / 2;
			}

			faces.push_back({ 0,1,4,2 });
			faces.push_back({ 3,5,7,6 });
			faces.push_back({ 1,4,7,5 });
			faces.push_back({ 0,2,6,3 });
			faces.push_back({ 0,1,5,3 });
			faces.push_back({ 2,4,7,6 });

			edge.push_back({ 0,1 });
			edge.push_back({ 1,4 });
			edge.push_back({ 4,2 });
			edge.push_back({ 2,0 });
			edge.push_back({ 3,5 });
			edge.push_back({ 5,7 });
			edge.push_back({ 7,6 });
			edge.push_back({ 6,3 });
			edge.push_back({ 0,3 });
			edge.push_back({ 5,1 });
			edge.push_back({ 7,4 });
			edge.push_back({ 6,2 });
		}
		
	};

	 Body derivative(Body body) {

		Body deriv;

		deriv.X = body.p/body.m;

		glm::quat w_q = glm::quat(0.0f, body.w);
		deriv.q = 0.5f * w_q *  body.q;

		deriv.p = { 0,0,0 };
		deriv.L = { 0,0,0 };

		return deriv;
	}

	 Body add(Body *p1, Body *p2) {
		 Body ans("cuboid");
		 ans.X = p1->X + p2->X;
		 ans.p = p1->p + p2->p;
		 ans.q = p1->q + p2->q;
		 ans.L = p1->L + p2->L;
		 ans.vertices = p1->vertices;
		 ans.faces = p1->faces;
		 return ans;
	}

	 Body multiply(Body *p1, float h) {
		 Body ans("cuboid");
		 ans.X = p1->X * h;
		 ans.p = p1->p * h;
		 ans.q = p1->q * h;
		 ans.L = p1->L * h;
		 ans.vertices = p1->vertices;
		 ans.faces = p1->faces;
		 return ans;
	 }

	 std::vector<glm::vec3> getOrientatedVertices() {
		 std::vector<glm::vec3> orientedVertices;
		 for (int i = 0; i < vertices.size(); i++) {
			 orientedVertices.push_back(X + glm::toMat3(glm::normalize(q)) * vertices[i]);
		 }
		 return orientedVertices;
	 }

	 glm::mat3 w_star() {
		 float w_arr[9] = { 0, -w[2],w[1], w[2], 0, -w[0], -w[1], w[0], 0 };
		 return(glm::make_mat3(w_arr));
	 }

	 void update() {
		 q = glm::normalize(q);
		 R = glm::toMat3(q);
		 I = (R)* I0 * glm::transpose(R);
		 w = glm::inverse(I) * L;
	 }

	void eulerStep(float h) {

		*this = add((this), &multiply(&derivative(*(this)), h));
		//std::cout << glm::to_string(R) << std::endl;
	}

	void RK2step(float h) {
		Body K1, K2;
		K1 = derivative((*this));
		K2 = derivative(add(this, &multiply(&K1, 0.5*h)));
		*this = add(this, &multiply(&K2, h));
	}

	void RK4step(float h) {
		Body K1, K2, K3, K4;
		K1 = derivative((*this));
		K2 = derivative(add(this, &multiply(&K1, 0.5*h)));
		K3 = derivative(add(this, &multiply(&K2, 0.5*h)));
		K4 = derivative(add(this, &multiply(&K3, h)));
		*this = add(this, &multiply(&K1, h / 6));
		*this = add(this, &multiply(&K2, h / 3));
		*this = add(this, &multiply(&K3, h / 3));
		*this = add(this, &multiply(&K4, h / 6));
	}

	glm::vec3 CollisionTest(std::vector <glm::vec3> Vert, std::vector<std::vector<int>> indices, float h) {
		
		Body nextState = *this;

		bool collide;
		nextState.RK4step(h);

		std::vector<glm::vec3> V1 = getOrientatedVertices();
		std::vector<glm::vec3> V2 = nextState.getOrientatedVertices();
		glm::vec3 normal, delP, delL;
		
		for (int i = 0; i < V1.size(); i++) {

			for (int j = 0; j < indices.size(); j++) {
				//printf("Loop(collide): %d %d\n", i, j);
				std::tie(collide, normal) = isCollidingFace(V1[i], V2[i], Vert, indices[j]);
				//printf("Loop: %d %d\n", i, j);

				if(collide){

				//if (((V1[i][0] - Vert[3][0])*(V2[i][0] - Vert[3][0])) < 0) {

					std::tie(delP,delL) = collisionResponse(V1[i]-X,normal);

					p += delP; L += delL;
					//L = { 0,0,0 }; p = {0,0,0};
					
					//update();
					//RK4step(h);
					//w = { 0,0,0 }; V = { 0,0,0 };
					return (V1[i]);

				}
			}
		}

		return glm::vec3(0, 0, 0);
		
	}

	std::tuple<bool,glm::vec3> isCollidingFace(glm::vec3 oldPos, glm::vec3 newPos, std::vector<glm::vec3> Vert, std::vector<int> indices_) {
		
		glm::vec3 p1 = Vert[indices_[0]]; glm::vec3 p2 = Vert[indices_[1]]; glm::vec3 p3 = Vert[indices_[2]];
			float a1 = p2.x - p1.x;
			float b1 = p2.y - p1.y;
			float c1 = p2.z - p1.z;
			float a2 = p3.x - p1.x;
			float b2 = p3.y - p1.y;
			float c2 = p3.z - p1.z;
			float a = b1 * c2 - b2 * c1;
			float b = a2 * c1 - a1 * c2;
			float c = a1 * b2 - b1 * a2;
			float d = (-a * p1.x - b * p1.y - c * p1.z);
			float norm = sqrt(a*a + b*b + c*c);
			a = a / norm; b = b / norm; c = c / norm; d = d / norm;
			float x = a*oldPos.x + b*oldPos.y + c*oldPos.z + d;
			float y = a*newPos.x + b*newPos.y + c*newPos.z + d;
			bool planeCollide = !((x < 0) == (y < 0));

			glm::vec3 normal(a, b, c);

			
			if (planeCollide) {

				Point polygon[10], point;
				for (int i = 0; i < indices_.size(); i++) {
					polygon[i] = Point(Vert[indices_[i]][0], Vert[indices_[i]][1], Vert[indices_[i]][2]);
					printf("Polygon Vertices %f %f %f\n", Vert[indices_[i]][0], Vert[indices_[i]][1], Vert[indices_[i]][2]);
				}

				point = Point(oldPos[0], oldPos[1], oldPos[2]);
				
				printf("\n", planeCollide);
				if (!(isInside_yz(polygon, indices_.size(), point) || isInside(polygon, indices_.size(), point) || isInside_xz(polygon, indices_.size(), point)))
				{
					printf("Vertex Check %f %f %f\n", oldPos[0], oldPos[1], oldPos[2]);
					planeCollide = false;
					printf("%d\n", planeCollide);
				}
			}

			return std::make_tuple(planeCollide, normal);
	}

	std::tuple<glm::vec3,glm::vec3> collisionResponse(glm::vec3 ra, glm::vec3 normal) {

		glm::vec3 p_dot_a = p / m + glm::cross(w, ra);
		float v_ = glm::dot(p_dot_a, normal);
		float j_imp = (-(1.0f + Cr)*v_) / (1.0f / m + glm::dot(normal, glm::cross(glm::inverse(I)*ra*normal, ra)));
		if (abs(j_imp) > 20) j_imp = 20 * j_imp / abs(j_imp);

		return std::make_tuple(j_imp * normal, j_imp * glm::cross(ra, normal));

	}

	~Body();
};

