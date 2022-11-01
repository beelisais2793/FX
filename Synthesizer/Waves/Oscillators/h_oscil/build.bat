gcc -c ./f_sdl.cpp -o f_sdl.o
gcc -c ./f_main.cpp -o f_main.o
gcc -c ./h_oscil.cpp -o h_oscil.o 
gcc h_oscil.o f_sdl.o f_main.o -o phy.exe -lmingw32 -lSDL2main -lSDL2 -lm
del *.o
pause
