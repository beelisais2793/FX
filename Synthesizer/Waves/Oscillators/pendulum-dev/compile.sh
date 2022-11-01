#!/bin/bash

echo "Compiling..."
echo " "
cd src/
g++ -c main.cpp Pendulum.cpp Tracer.cpp -I/Documents/SFML/SFML_SRC/include 
echo "Linking..."
echo " "

if [ -e *.cpp~ ] ;
then
    rm *.cpp~
fi

mv *.o ../linkers
cd ../linkers

g++ main.o Pendulum.o Tracer.o -o pendulum -L/Documents/SFML/SFML_SRC/lib -lsfml-graphics -lsfml-window -lsfml-system

mv pendulum ../
cd ../

if [ -e compile.sh~ ] ;
then 
    rm compile.sh~ 
fi

cd include/
if [ -e *.hh~ ];
then
    rm *.hh~
fi

cd ..
echo "Executing..."
echo " "
./pendulum

