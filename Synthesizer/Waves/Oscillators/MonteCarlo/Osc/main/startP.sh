#!/bin/bash
# It launchs the programs, five to time, for different values of the input parameters.

time ./mainEnergy infile1  &

time ./mainEnergy infile2  &

time ./mainEnergy infile3  &

time ./mainEnergy infile4  &

time ./mainEnergy infile5  &

time ./mainEnergy infile6  &

wait
printf "completed...\n"