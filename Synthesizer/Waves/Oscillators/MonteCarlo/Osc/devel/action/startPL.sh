#!/bin/bash
# It launchs the program, five to time, for different values of the input parameters.

sed "s|@L@|0.8|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|0.82|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|0.84|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|0.86|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|0.88|g" infile > infile5
time ./mainCor infile5  &

wait 

sed "s|@L@|0.92|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|0.94|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|0.96|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|0.98|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|0.9|g" infile > infile5
time ./mainCor infile5  &

wait
printf "completed"