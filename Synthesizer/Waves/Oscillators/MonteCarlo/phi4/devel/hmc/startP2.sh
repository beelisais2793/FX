#!/bin/bash
# It launchs the program, five to time, for different values of the input parameters.

#!/bin/bash

sed "s|@kappa@|0.21|g" infile > infile1
time ./ACorrelation infile1  &

sed "s|@kappa@|0.212|g" infile > infile2
time ./ACorrelation infile2  &

sed "s|@kappa@|0.214|g" infile > infile3
time ./ACorrelation infile3  &

sed "s|@kappa@|0.216|g" infile > infile4
time ./ACorrelation infile4  &

sed "s|@kappa@|0.218|g" infile > infile5
time ./ACorrelation infile5 &

wait
printf "done...\n"
