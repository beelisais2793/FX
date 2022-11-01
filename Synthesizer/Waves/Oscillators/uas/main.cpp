#include "oscillator.hpp"
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

int main() {
	std::vector<oscParam> oscParams = initOscs();
	
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) return 1;
	
	SDL_AudioSpec spec;
	
	spec.freq = samplerate;
	spec.format = AUDIO_F32SYS;
	spec.channels = 1;
	spec.samples = 512;
	//spec.callback = generate;
	spec.userdata = &oscParams;
	spec.callback = [](void* userdata, Uint8* stream, int len) {
		float *streamf = (float*) stream; //SDL gives uint8, convert to float
		auto *params = (std::vector<oscParam>*) userdata;
		for (int i=0; i<len/sizeof(float); i++) {
			float wave = 0;
			
			for (oscParam &param : *params) {
				wave += oscSine(0.01, param.first, synthTime, param.second);
			}
			
			//wave += oscSine(1, (float)1000/samplerate, synthTime, 0); //for testing purposes
			streamf[i] = wave; //Reminder: index out of bounds causes a segfault
			synthTime++;
		}
	};
	
	auto audio = SDL_OpenAudioDevice(nullptr, 0, &spec, &spec, 0);
	if (audio < 0) std::cerr << "Error: " << SDL_GetError();
	SDL_PauseAudioDevice(audio, 0);
	SDL_Delay(4000);
	return 0;
}
