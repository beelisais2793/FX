#pragma once

#include <iostream>
#include <random>

float freq = 440.f; //A4
float synthTime = 0;
const int samplerate = 44100;
const int oscs = 4096;
const std::size_t LUTSize = 512; //Must be a power of two, for performance reasons.
std::vector<float> LUT;
typedef std::pair<float, float> oscParam; //Normalised frequency (freq/samplerate) and phase

inline float RNG(const float min, const float max) {
        std::random_device rand_device;
        std::default_random_engine generator(rand_device());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
}

std::vector<float> distribute(float min, float max, int amount) {
	std::vector<float> nums;
	for (float i=min;i<max;i+=(max-min)/amount) {
		nums.push_back(i);
	}
	return nums;
}

void initLUT(std::vector<float> &lut) {
	lut.reserve(LUTSize);
	for (int i=0;i<LUTSize;i++) {
		lut.push_back(sin(i/(LUTSize/M_PI/2)));
	}
}

inline float LUTLerp(std::vector<float> &vec, float index) {
	return vec[floor(index)]+(index-floor(index))*((vec[ceil(index)]-vec[floor(index)])*(index-floor(index)));
	//return vec[floor(index)]+(vec[floor(index)]-vec[ceil(index)])*fmod(index,1);
}

inline float bhaskaraSin(float x) {
	x+=x;
	float c = 2*(x/2-std::floor(x/2));
	x = M_PI*(x-std::floor(x));
 	float b = (16*x*(M_PI-x))/(5*(M_PI*M_PI)-4*x*(M_PI-x));
 	return ((c>1)?-b:b);
}

inline float oscSine(float amp, float normf, float time, const float phase) { // normf is NORMALISED frequency (freq/samplerate), not real frequency.
// 	return amp*sin(2*M_PI*(phase+time)*normf); //cmath sin
// 	return amp*LUT[int(freq*LUTSize*(phase+time)/samplerate) & LUTSize-1]; //sine LUT without interp, bitwise AND //FASTEST
// 	return amp*LUTLerp(LUT, fmod(freq*LUTSize*(phase+time)/samplerate,LUTSize)); //sine LUT with interp, modulo
// 	return amp*LUTLerp(LUT, int(freq*LUTSize*(phase+time)/samplerate) & LUTSize-1); //sine LUT with interp, int ver, bitwise AND
// 	return (float)(int(freq*LUTSize*(phase+time)/samplerate) & LUTSize-1); //sawtooth, for LUT index testing
	float normTime = (phase+time)*normf;
	return amp*LUTLerp(LUT, (normTime-floor(normTime))*LUTSize);
// 	return amp*bhaskaraSin((phase+time)*normf); //Bhaskara I sine approximation
}

std::vector<oscParam> initOscs() {
	std::vector<oscParam> params;
	std::vector<float> freqs = distribute(16.35,22050,oscs);
	for (float freq : freqs) {
		params.push_back(oscParam(freq/samplerate, RNG(0,freq)));
	}
	initLUT(LUT);
	/*
	for (oscParam param : params) {
		std::cerr << param.second << ", ";
	}
	*/
	return params;
}
