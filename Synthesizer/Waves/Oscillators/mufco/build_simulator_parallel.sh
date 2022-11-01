#!/bin/bash

./build_simulator.sh

cd build

#Create folders if needed
if [ ! -d parallel ]; then
  mkdir parallel
else  
  rm parallel/mufcoSim_parallel
fi

#Go into parallel folder
cd parallel

#Copy autotools files MUFCO PARALLEL
if [[ ! -e configure.ac ]]; then
	cp ../../src/parallel/configure.ac  .
fi
cp ../../src/parallel/Makefile.am           .

#First installation MUFCO PARALLEL
if [[ ! -e ltmain.sh ]]; then
  libtoolize
fi
if [[ ! -e configure ]]; then
	autoreconf
fi
if [[ ! -e Makefile.in ]]; then
	automake --include-deps --add-missing --foreign
fi
if [[ ! -e Makefile ]]; then
	./configure
fi

#MUFCO make
make

#Copy MUFCO executable in the main folder 
cp mufcoSim_parallel ../..
cd ../..
