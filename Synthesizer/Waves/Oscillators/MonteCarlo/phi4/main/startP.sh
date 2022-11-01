#!/bin/bash

sed "s|@kappa@|0.171|g" infile > infile1
time ./phi4 infile1  &

sed "s|@kappa@|0.172|g" infile > infile2
time ./phi4 infile2  &

sed "s|@kappa@|0.173|g" infile > infile3
time ./phi4 infile3  &

sed "s|@kappa@|0.174|g" infile > infile4
time ./phi4 infile4  &

sed "s|@kappa@|0.256|g" infile > infile5
time ./phi4 infile5  &


wait 
printf "completed...\n"