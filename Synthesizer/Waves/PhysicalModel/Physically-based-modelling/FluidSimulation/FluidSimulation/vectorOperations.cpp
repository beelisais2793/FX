#pragma once
#include "vectorOperations.h"
#include <string.h>

void addVec(int n, float r[], const float v[])
{
	for (int i = 0; i < n; i++)
	{
		r[i] = r[i] + v[i];
	}
}

void subtractVec(int n, float r[], const float v[])
{
	for (int i = 0; i < n; i++)
	{
		r[i] = r[i] - v[i];
	}
}

void equateVec(int n, float v1[], const float v2[])
{
	memcpy(v1, v2, n * sizeof(float));
}

void scalarMult(int n, float v[], float s)
{
	for (int i = 0; i < n; i++)
	{
		v[i] *= s;
	}
}

float dotProduct(int n, const float v1[], const float v2[])
{
	float dot = 0;
	for (int i = 0; i < n; i++)
	{
		dot += v1[i] * v2[i];
	}
	return dot;
}

float VecSqrLen(int n, const float v[])
{
	return dotProduct(n, v, v);
}