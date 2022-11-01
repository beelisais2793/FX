#!/bin/bash
# It launchs the program, five to time, for different values of the input parameters.

sed "s|@a@|0.5|g" infile > infile1
time ./mainCor infile1  &

sed "s|@a@|0.25|g" infile > infile2
time ./mainCor infile2  &


wait
printf "completed...\n"