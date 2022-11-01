# Plucked String Filter

This C++ program works by using a combination of 3 filters in series (lowpass, all pass, and delay).
Program generates a mono wave file at 44100Hz containing impulses fed by given header files.

## How to Use

Makefile included:
- To build program, run: `make build`
- To remove executable, run: `make clean`
- Makefile automatically builds program if needed upon testing

key_freqs.h: Contains pitch name definitions with their corresponding frequency in Hz.
bassline.h: Contains note values for bassline.
melody.h: Contains note values for melody.
pluck.h: Header file for main program.
pluck.cpp: Main program for calculating plucked string filter.
output.wav: Wave file generated upon running pluck program.

- Compiles using `--std=c++11`
