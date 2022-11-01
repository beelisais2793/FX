#!/bin/bash

#Create folders if needed
if [ ! -d build ]; then
  mkdir build
else
  rm build/mufcoSim  
fi

if [ ! -d build/common ]; then
  mkdir build/common
fi

if [ ! -d build/graphics_engine ]; then
  mkdir build/graphics_engine
fi

if [ ! -d build/simulator ]; then
  mkdir build/simulator
fi

#Go into building folder
cd build

#Copy autotools files MUFCO
if [[ ! -e configure.ac ]]; then
	cp ../src/configure.ac  .
fi
cp ../src/Makefile.am                 .
cp ../src/common/Makefile.am          common
cp ../src/graphics_engine/Makefile.am graphics_engine
cp ../src/simulator/Makefile.am       simulator

#First installation MUFCO
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
cp mufcoSim ..
cd ..
