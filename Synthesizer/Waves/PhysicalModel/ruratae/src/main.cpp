#include "ruratae.hpp"
#include <stdio.h>

int main(int argc, char* argv[])
{
  ruratae::instrument instr(10, 10);
	int a = instr.create_particle(ruratae::particle_params());
  int b = instr.create_particle(ruratae::particle_params(
		ruratae::vec3(1, 0, 0), ruratae::vec3(0), 1.0f));
	int c = instr.create_spring(ruratae::spring_params(a, b, 3000000, 2000000));
	instr.set_listener(ruratae::vec3(0, 1, 0));
  for (int i = 0; i < 100; i++)
  {
    float out;
    instr.process(&out, 1);
		printf("%f\n", out * 100);
	}
  return 0;
}