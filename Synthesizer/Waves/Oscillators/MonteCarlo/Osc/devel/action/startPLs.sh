#!/bin/bash
# It launchs the program, five to time, for different values of the input parameters.

sed "s|@L@|4.6|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|4.7|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|4.8|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|4.9|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|5.0|g" infile > infile5
time ./mainCor infile5  &

wait

sed "s|@L@|5.1|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|5.2|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|5.3|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|5.4|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|5.5|g" infile > infile5
time ./mainCor infile5  &

wait

sed "s|@L@|5.6|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|5.7|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|5.8|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|5.9|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|6.0|g" infile > infile5
time ./mainCor infile5  &

wait

sed "s|@L@|6.1|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|6.2|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|6.3|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|6.4|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|6.5|g" infile > infile5
time ./mainCor infile5  &

wait

sed "s|@L@|6.6|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|6.7|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|6.8|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|6.9|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|7.0|g" infile > infile5
time ./mainCor infile5  &

wait
printf "completed...\n"

sed "s|@L@|1.7|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|1.8|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|1.9|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|2.0|g" infile > infile5
time ./mainCor infile5  &

wait

sed "s|@L@|2.1|g" infile > infile1
time ./mainCor infile1  &

sed "s|@L@|2.2|g" infile > infile2
time ./mainCor infile2  &

sed "s|@L@|2.3|g" infile > infile3
time ./mainCor infile3  &

sed "s|@L@|2.4|g" infile > infile4
time ./mainCor infile4  &

sed "s|@L@|2.5|g" infile > infile5
time ./mainCor infile5  &

wait
printf "completed...\n"

