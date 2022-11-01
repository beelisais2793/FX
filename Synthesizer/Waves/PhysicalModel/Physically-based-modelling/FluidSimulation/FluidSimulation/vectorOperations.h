#pragma once
#include <stdio.h>

void addVec(int n, float r[], const float v[]);


void subtractVec(int n, float r[], const float v[]);


void equateVec(int n, float v1[], const float v2[]);


void scalarMult(int n, float v[], float s);


float dotProduct(int n, const float v1[], const float v2[]);


float VecSqrLen(int n, const float v[]);
