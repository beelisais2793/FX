#include "f_sdl.h"


void SDL_Display::Clear(uint32_t col) {
	uint8_t r, g, b;
	SDL_GetRGB(col, this->surface->format, &r, &g, &b); 
	SDL_SetRenderDrawColor(this->renderer, r, g, b, 255);
	SDL_RenderClear(this->renderer);
}

void SDL_Display::Flush(void) {
	SDL_RenderPresent(this->renderer);
	//SDL_UpdateWindowSurface(this->window);
}

SDL_Display::SDL_Display(int h, int w) {
	this->window = NULL;
	SDL_Surface* S = NULL;
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		fprintf(stderr, "SDL failed to initialise %s", SDL_GetError());
	else {
		SDL_CreateWindowAndRenderer(w, h, 0, &(this->window), &(this->renderer));
		if(this->window == NULL || this->renderer == NULL)
			fprintf(stderr, "Window could not be created. %s", SDL_GetError());
		else {
			S = SDL_GetWindowSurface(this->window);
			this->surface = S;
			SDL_RenderClear(this->renderer);
		}
	}
}

void SDL_Display::Poof(void) {
	SDL_DestroyWindow(this->window);
	SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
}

void SDL_Display::Block(int cx, int cy, int w, int h, uint32_t col) {
	uint8_t r, g, b;
	SDL_GetRGB(col, this->surface->format, &r, &g, &b); 
	SDL_SetRenderDrawColor(this->renderer, r, g, b, 255);
	SDL_Rect rect;
	rect.x = cx - (w/2);
	rect.y = cy - (h/2);
	rect.w = w;
	rect.h = h;
	SDL_RenderDrawRect(this->renderer, &rect);
}

void SDL_Display::Update(void) {
	// Update function
	for(int i = 0; i < 128; i++) {
		if(this->CallBacks[i].fn != NULL) {
			if(this->CallBacks[i].curr > 0) {
				this->CallBacks[i].curr -= 1;
				continue;
			}
			this->CallBacks[i].curr = this->CallBacks[i].freq;
			int ret_val = this->CallBacks[i].fn(this);
			if(ret_val == CALLBACK_QUITS)
				this->CallBacks[i].fn = NULL;
		}
	}
	SDL_Event ev;
	while(SDL_PollEvent(&ev)) {
		if(ev.type == SDL_QUIT)
			this->Poof();
	}
	this->Flush();
}

void SDL_Display::AddCallBack(SDL_CallBackFunction fn, uint64_t freq) {
	for(int i = 0; i < 128; i++) {
		if(this->CallBacks[i].fn == NULL) {
			this->CallBacks[i].fn = fn;
			this->CallBacks[i].freq = freq;
			this->CallBacks[i].curr = freq;
		}
	}
}

void SDL_Display::Line(int x1, int y1, int x2, int y2, uint32_t col) {
	uint8_t r, g, b;
	SDL_GetRGB(col, this->surface->format, &r, &g, &b); 
	SDL_SetRenderDrawColor(this->renderer, r, g, b, 255);
	SDL_RenderDrawLine(this->renderer, x1, y1, x2, y2);
}

uint32_t SDL_Display::GetTicks(void) {
	return (uint32_t)SDL_GetTicks();
}
