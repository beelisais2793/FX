#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <math.h>

#include "f_sdl.h"
#include "h_oscil.h"

uint32_t my_ticks;
// Speeds in mm/ms l_0x - Spring length in x direction, l_0y - spring (natural) length in y_direction.
CHarmonic_Oscillator oscillator(2000, 40, 150, 0, 8000, 90);
int s_x = 0;
int s_y = 0;
int p_x = 320;
int p_y = 240;
int oscillator_callback(void* disp);

int main(int argc, char** argv) {
	SDL_Display disp(480, 640);
	disp.Clear(0x0000);
	my_ticks = disp.GetTicks()+1000;
	disp.AddCallBack(oscillator_callback, 0);
	disp.Flush();
	while(true) {
		disp.Clear(0x0000);
		disp.Block(s_x, s_y, 32, 32, 0xFF0000);
		disp.Block(p_x, p_y, 8, 8, 0x00FF00);
		disp.Line(p_x, p_y, p_x+oscillator.x, p_y+oscillator.y, 0x0000FF);
		disp.Update();
	}
}

int oscillator_callback(void* disp) {
	SDL_Display* my_disp = (SDL_Display*)disp;
	if(my_disp->GetTicks() - my_ticks > 10) {
		oscillator.Update();
		fprintf(stdout, "Kinetic Energy: %f J, Oscillator potential: %f J, Total: %f\n", oscillator.T, oscillator.V, oscillator.T + oscillator.V);
		s_x = p_x+oscillator.x;
		s_y = p_y+oscillator.y;
		my_ticks = my_disp->GetTicks();
	}
	return CALLBACK_OK;
}
