#!/usr/bin/env bash



g++ -O3 -std=c++17 RungeKutta.cpp -c
g++ -O3 -std=c++17 RungeKutta.o main.cpp -o integrate


./integrate
