#!/bin/bash
# It launchs the program, once to time, for different values of the input parameters.


sed "s|@kappa@|0.15|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.155|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.16|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.165|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.17|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.175|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.18|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.185|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.19|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.195|g" infile > infile1
time ./Acceptance infile1
wait

sed "s|@kappa@|0.20|g" infile > infile1
time ./Acceptance infile1
wait