#!/bin/bash

There_are_all_needed_programs=true
if [[ -z `which gnuplot` ]]
then
	echo 'You need to install gnuplot to create animation'
	There_are_all_needed_programs=false
fi
if [[ -z `which gifsicle` ]]
then
	echo 'You need to install gifsicle to optim animation'
	There_are_all_needed_programs=false
fi
if (( ${There_are_all_needed_programs} == 'true' )) 
then
	./bin/Debug/string <in >./gnuplot/animation.dat
	cd gnuplot
	gnuplot animator.gp
	gifsicle -i animation.gif -O3 -o optim.gif
fi
