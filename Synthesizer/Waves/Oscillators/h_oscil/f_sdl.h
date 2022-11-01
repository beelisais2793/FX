#ifndef F_SDL_H
#define F_SDL_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>

#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 480

#define CALLBACK_QUITS -1
#define CALLBACK_OK 0

typedef int(*SDL_CallBackFunction)(void*);

typedef struct {
	SDL_CallBackFunction fn = NULL;
	uint64_t freq, curr; // Frequency and "current counter", 0 = Every cycle.
} SDL_CallBack;

class SDL_Display {
public:
	SDL_Display(int h, int w);
	
	void Clear(uint32_t col);
	void Line(int x1, int y1, int x2, int y2, uint32_t col);
	void Block(int cx, int cy, int w, int h, uint32_t col);
	
	void Poof(void);
	void Flush(void);
	
	void Update(void);
	
	uint32_t GetTicks(void);
	
	void AddCallBack(SDL_CallBackFunction fn, uint64_t freq);
private:
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
	SDL_CallBack CallBacks[128];
	
};


#endif
