#pragma once
class Ball
{

public:
	float position[3] = {0.0, 0.0, 0.0};
	float velocity[3] = {0.0, 0.0, 0.0 };
	float acceleration[3] = {0.0, 0.0, 0.0 };
	float gravity[3] = { 0.0, 0.0, 0.0 };

	float mass = 10;

	Ball();
	float * getPos();
	void update(float h, float d, float * wind, char G);
	~Ball();
};

