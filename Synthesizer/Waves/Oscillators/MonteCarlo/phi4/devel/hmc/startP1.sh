#!/bin/bash
# It launchs the program, five to time, for different values of the input parameters.

#!/bin/bash
sed "s|@kappa@|0.15|g" infile > infile1
time ./Acceptance infile1  &

sed "s|@kappa@|0.16|g" infile > infile2
time ./Acceptance infile2  &

sed "s|@kappa@|0.17|g" infile > infile3
time ./Acceptance infile3  &

sed "s|@kappa@|0.19|g" infile > infile4
time ./Acceptance infile4  &

sed "s|@kappa@|0.195|g" infile > infile5
time ./Acceptance infile5 &

wait 
printf "done 1...\n"

sed "s|@kappa@|0.20|g" infile > infile1
time ./Acceptance infile1  &

sed "s|@kappa@|0.205|g" infile > infile2
time ./Acceptance infile2  &

sed "s|@kappa@|0.21|g" infile > infile3
time ./Acceptance infile3  &

sed "s|@kappa@|0.22|g" infile > infile4
time ./Acceptance infile4  &

sed "s|@kappa@|0.23|g" infile > infile5
time ./Acceptance infile5  &

wait 
printf "done 2...\n"

sed "s|@kappa@|0.24|g" infile > infile1
time ./Acceptance infile1  &

sed "s|@kappa@|0.25|g" infile > infile2
time ./Acceptance infile2  &

sed "s|@kappa@|0.26|g" infile > infile3
time ./Acceptance infile3  &

sed "s|@kappa@|0.27|g" infile > infile4
time ./Acceptance infile4  &

sed "s|@kappa@|0.28|g" infile > infile5
time ./Acceptance infile5 &

wait 
printf "done 3..\n"

sed "s|@kappa@|0.29|g" infile > infile1
time ./Acceptance infile1  &

sed "s|@kappa@|0.30|g" infile > infile2
time ./Acceptance infile2  &

sed "s|@kappa@|0.31|g" infile > infile3
time ./Acceptance infile3  &

sed "s|@kappa@|0.32|g" infile > infile4
time ./Acceptance infile4  &

sed "s|@kappa@|0.33|g" infile > infile5
time ./Acceptance infile5 &


wait 
printf "completed...\n"