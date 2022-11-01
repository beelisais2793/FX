#pragma once
#include <math.h>
#include <cmath>

struct Point {
	float x;
	float y;
	float z;
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	Point(float x, float y, float z=0.0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct Plane{
	float a, b, c, d;
	Plane() {
		a = 0; b = 0; c = 0; d = 0;
	}
	Plane(Point p1, Point p2, Point p3) {
		float a1 = p2.x - p1.x;
		float b1 = p2.y - p1.y;
		float c1 = p2.z - p1.z;
		float a2 = p3.x - p1.x;
		float b2 = p3.y - p1.y;
		float c2 = p3.z - p1.z;
		a = b1 * c2 - b2 * c1;
		b = a2 * c1 - a1 * c2;
		c = a1 * b2 - b1 * a2;
		d = (-a * p1.x - b * p1.y - c * p1.z);
		float norm = sqrt(a*a + b*b + c*c);
		a = a / norm; b = b / norm; c = c / norm; d = d / norm;
	}
	bool getSign(Point p1, Point p2) {
		//printf("\n %f %f %f %f \n", p1.x, p2.x, p1.y, p2.y);
		float x = a*p1.x + b*p1.y + c*p1.z + d;
		float y = a*p2.x + b*p2.y + c*p2.z + d;
		return ((x < 0) == (y < 0));
	}
	float getDistance(Point p) {
		return abs(a*p.x + b*p.y + c*p.z + d)/sqrt(a*a+b*b+c*c);
	}
};

bool isInside(Point polygon[], int n, Point p);
bool isInside_yz(Point polygon[], int n, Point p);
bool isInside_xz(Point polygon[], int n, Point p);
