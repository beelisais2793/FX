#pragma once
#include <glm/glm.hpp>

#include "vectorOperations.h"


#define EPSILON 1.0e-10

class Fluid
{

	void ApplyLaplace(float x[], float result[]) {

		result[indexXY(0, 0)] = x[indexXY(0, 1)] + x[indexXY(1, 0)] + x[indexXY(0, gridSize - 1)] + x[indexXY(gridSize - 1, 0)] - 4.0f* x[indexXY(0, 0)];
		result[indexXY(gridSize - 1, gridSize - 1)] = x[indexXY(gridSize - 1, gridSize - 2)] + x[indexXY(gridSize - 2, gridSize - 1)] + x[indexXY(0, gridSize - 1)] + x[indexXY(gridSize - 1, 0)] - 4.0f* x[indexXY(gridSize - 1, gridSize - 1)];
		result[indexXY(gridSize - 1, 0)] = x[indexXY(gridSize - 1, 1)] + x[indexXY(gridSize - 2, 0)] + x[indexXY(0, 0)] + x[indexXY(gridSize - 1, gridSize - 1)] - 4.0f* x[indexXY(gridSize - 1, 0)];
		result[indexXY(0, gridSize - 1)] = x[indexXY(1, gridSize - 1)] + x[indexXY(0, gridSize - 2)] + x[indexXY(0, 0)] + x[indexXY(gridSize - 1, gridSize - 1)] - 4.0f* x[indexXY(0, gridSize - 1)];

		for (int i = 1; i < gridSize - 1; i++) {
			result[indexXY(0, i)] = x[indexXY(0, i - 1)] + x[indexXY(0, i + 1)] + x[indexXY(1, i)] + x[indexXY(gridSize - 1, i)] - 4.0f * x[indexXY(0, i)];
			result[indexXY(gridSize - 1, i)] = x[indexXY(gridSize - 1, i - 1)] + x[indexXY(gridSize - 1, i + 1)] + x[indexXY(gridSize - 2, i)] + x[indexXY(0, i)] - 4.0f * x[indexXY(gridSize-1, i)];
			result[indexXY(i,0)] = x[indexXY(i - 1,0)] + x[indexXY(i + 1,0)] + x[indexXY(i,1)] + x[indexXY(i,gridSize - 1)] - 4.0f * x[indexXY(i,0)];
			result[indexXY(i, gridSize - 1)] = x[indexXY(i - 1, gridSize - 1)] + x[indexXY(i + 1, gridSize - 1)] + x[indexXY(i, gridSize - 2)] + x[indexXY(i,0)] - 4.0f * x[indexXY(i, gridSize - 1)];
		}

		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {
				result[indexXY(i, j)] = x[indexXY(i, j + 1)] + x[indexXY(i + 1, j)] + x[indexXY(i, j - 1)] + x[indexXY(i - 1, j)] - 4.0f * x[indexXY(i, j)];
			}
		}
	}

	void findPressureField() {

		int i = 0;
		int iMax = 1000;
		float alpha, beta, rSqrLen, rSqrLenOld, u;

		equateVec(totalCells, PressureField, divergence);
		/**/
		equateVec(totalCells, r, divergence);
		ApplyLaplace(PressureField, temp);
		subtractVec(totalCells, r, temp);

		rSqrLen = VecSqrLen(totalCells, r);

		equateVec(totalCells, d, r);


		if (rSqrLen > EPSILON)
		{
			while (i < iMax)
			{
				i++;
				ApplyLaplace(d, t);
				u = dotProduct(totalCells, d, t);

				if (u == 0)
				{
					printf("(SolveConjGrad) d'Ad = 0\n");
					break;
				}

				// How far should we go?
				alpha = rSqrLen / u;

				// Take a step along direction d
				equateVec(totalCells, temp, d);
				scalarMult(totalCells, temp, alpha);
				addVec(totalCells, PressureField, temp);

				if (i & 0x3F)
				{
					equateVec(totalCells, temp, t);
					scalarMult(totalCells, temp, alpha);
					subtractVec(totalCells, r, temp);
				}
				else
				{
					// For stability, correct r every 64th iteration
					equateVec(totalCells, r, divergence);
					ApplyLaplace(PressureField, temp);
					subtractVec(totalCells, r, temp);
				}

				rSqrLenOld = rSqrLen;
				rSqrLen = VecSqrLen(totalCells, r);

				// Converged! Let's get out of here
				if (rSqrLen <= EPSILON)
				{
					break;
				}

				// Change direction: d = r + beta * d
				beta = rSqrLen / rSqrLenOld;
				scalarMult(totalCells, d, beta);
				addVec(totalCells, d, r);
			}
		}
		
	}


public:
	int gridSize, totalCells;
	glm::vec2 *VelocityField, *AccelerationField;
	glm::vec3 *color;
	float viscosity, density, h, *PressureField, *divergence;
	float *r;
	float *d;
	float *t ;
	float *temp;

	int indexXY(int x, int y) {
		return y*gridSize + x;
	}

	Fluid(int size, float viscocity, float h) :gridSize(size), viscosity(viscocity), h(h), totalCells(size*size) {
		VelocityField = new glm::vec2[totalCells];
		AccelerationField = new glm::vec2[totalCells];
		PressureField = new float[totalCells]();
		color = new glm::vec3[totalCells];
		r = new float[totalCells];
		 d = new float[totalCells];
		t = new float[totalCells];
		temp = new float[totalCells];
		divergence = new float[totalCells]();
	}

	/*************	VELOCITY UPDATE		**************************************/
	void timestepVelocity() {
		AddForce();
		advectVelocity();
		projectVelocity();
	}

	void AddForce() {
		for (int i = 0; i < totalCells; i++) {
			VelocityField[i] += h*AccelerationField[i];
		}
	}

	void advectVelocity() {
		glm::vec2 oldPos, currentPos, vel;
		glm::vec2 *newVelField = new glm::vec2[totalCells];

		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {

				currentPos = { i - 0.5,j - 0.5 };
				vel = bilinearInterpolation(currentPos);
				oldPos = currentPos - vel*h;
				newVelField[indexXY(i, j)] = bilinearInterpolation(oldPos);
			}
		}

		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {

				if (i == 1)
				{
					VelocityField[indexXY(i, j)][0] = 0;
				}
				else
				{
					VelocityField[indexXY(i, j)][0] = 0.5 * (newVelField[indexXY(i, j)][0]
						+ newVelField[indexXY(i - 1, j)][0]);
				}

				if (j == 1)
				{
					VelocityField[indexXY(i, j)][1] = 0;
				}
				else
				{
					VelocityField[indexXY(i, j)][1] = 0.5 * (newVelField[indexXY(i, j)][1]
						+ newVelField[indexXY(i, j - 1)][1]);
				}
			}
		}

		delete[] newVelField;
	}

	void projectVelocity() {

		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {
				divergence[indexXY(i, j)] = VelocityField[indexXY(i + 1, j)][0] - VelocityField[indexXY(i, j)][0] + VelocityField[indexXY(i, j + 1)][1] - VelocityField[indexXY(i, j)][1];
				
			}
		}

		
		findPressureField();

		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {
				if (i == 1) {
					if (j == 1) {
						VelocityField[indexXY(i, j)] = glm::vec2(0, 0);
					}
					else {
						float py = PressureField[indexXY(i, j)] - PressureField[indexXY(i, j - 1)];
						VelocityField[indexXY(i, j)] = VelocityField[indexXY(i, j)] - glm::vec2(0, py);
					}
				}
				else {
					if (j == 1) {
						VelocityField[indexXY(i, j)] = VelocityField[indexXY(i, j)] - glm::vec2(PressureField[indexXY(i, j)] - PressureField[indexXY(i - 1, j)], 0);
					}
					else {
						float px = PressureField[indexXY(i, j)] - PressureField[indexXY(i - 1, j)];
						float py = PressureField[indexXY(i, j)] - PressureField[indexXY(i, j - 1)];
						VelocityField[indexXY(i, j)] = VelocityField[indexXY(i, j)] - glm::vec2(px, py);

					}
				}
			}
		}
		
	}

	glm::vec2 bilinearInterpolation(glm::vec2 &pos) {
		float vel_x, vel_y;

		int x0 = static_cast<int>(floor(pos[0] + 1) + EPSILON);
		int y0 = static_cast<int>(pos[1] + 0.5 + EPSILON);
		vel_x = BI(pos, x0, y0, 0);

		// Interpolate the y component
		x0 = static_cast<int>(pos[0] + 0.5);
		y0 = static_cast<int>(floor(pos[1] + 1) + EPSILON);
		vel_y = BI(pos, x0, y0, 1);

		return glm::vec2(vel_x, vel_y);
	}

	float BI(glm::vec2 &pos, int x0, int y0, int coord) {
		return 0.25*VelocityField[indexXY(x0, y0)][coord]+ 0.25*VelocityField[indexXY(x0, y0+1)][coord]+ 0.25*VelocityField[indexXY(x0+1, y0)][coord]+ 0.25*VelocityField[indexXY(x0+1, y0+1)][coord];
	}

	/*************	DYE UPDATE	**************************************/

	void timestepDye() {
		AddDye();
		AdvectDye();
	}

	void AddDye() {

	}

	void AdvectDye() {
		float *newField = new float[totalCells];


		for (int i = 1; i < gridSize - 1; i++) {
			for (int j = 1; j < gridSize - 1; j++) {
		
				glm::vec2 currentPos = { i - 0.5,j - 0.5 };
				glm::vec2 CenterVel = bilinearInterpolation(currentPos);
				
				glm::vec2 oldPos = currentPos - CenterVel*(h*1000);
				color[indexXY(i, j)] = scalarColorInterpolation(oldPos);
			}
		}
		delete[] newField;

	}

	glm::vec3 scalarColorInterpolation(glm::vec2 &pos) {
		int low_x = static_cast<int>(pos[0] + 0.5);
		int low_y = static_cast<int>(pos[1] + 0.5);

		glm::vec3 cx, chx;
		
		cx = ((low_y + 0.5f) - pos[1]) * color[indexXY(low_x, low_y)]
			+ (pos[1] - (low_y - 0.5f)) * color[indexXY(low_x, low_y + 1)];
	
		chx = ((low_y + 0.5f) - pos[1]) * color[indexXY(low_x + 1, low_y)]
			+ (pos[1] - (low_y - 0.5f)) * color[indexXY(low_x + 1, low_y + 1)];

		return ((low_x + 0.5f) - pos[0]) * cx
			+ (pos[0] - (low_x - 0.5f)) * chx;
	}

	~Fluid() {
		delete[] VelocityField;
		delete[] AccelerationField;
		delete[] PressureField;
		delete[] r;
		delete[] d;
		delete[] t;
		delete[] temp;
		delete[] color;
		delete[] divergence;
	}
};

