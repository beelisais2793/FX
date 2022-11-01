# GENAC

## Introduction

Genac is a symbolic analog circuit analyzer and synthesizer. Given a circuit in
a specific netlist form ([see samples][1]) it can compute the transfer function
of the circuit as a symbolic maths expression using [Modified Nodal Analysis
(MNA)][2]. It can also generate circuits with some certain criteria through
genetic algorithms. Genac uses [GiNaC][3] to do the symbolic maths for both
analyzer and synthesizer modes.

Genac was my final year project at the Department of Electrical and Electronics
Engineering, 9 Eylul University. I refactor and improve it in my spare time, so
do not expect too much from this project :)

## Build

Minimum Qt 5.5.0 required.
```
sudo apt-get install pkg-config libginac-dev qt5-default cmake
cd to/this/direectory
mkdir build && cd build
cmake ..
make
```

## Run

Run the analyzer:

```
$ src/genac analyze -n ../samples/circuit1.nl -l ../samples/LIB
*  V(1)
*  ----- =
*  V(4)
*                            R1
*  -------------------------------------------------------
*  R2*s*R3*C2+R2*s*C2*R1+s*R3*C2*R1+C1*R2*s^2*R3*C2*R1+R1

```

Run genetic algorithm based synthesizer:

```
$ src/genac genetic-synthesize \
--numerator 001   \
--denominator 111 \
--ncapacitors 2   \
--nresistors 3    \
--active-element OPAMP \
--population-size 15 \
--ngenerations 500 \
--selection-type RANK \
--crossover-type ONEPOINT \
--crossover-probability 0.80 \
--mutation-probability 0.01 # if not provided, defaults to adaptive mutation probability.
```

## Plot genetic algorithm records

Here is an example plot:

```
./graph.py ~/Documents/ga-circuit-records/different-filters/run1-1/GARecords.txt \
           ~/Documents/ga-circuit-records/different-filters/run2-3/GARecords.txt \
           ~/Documents/ga-circuit-records/different-filters/run3-3/GARecords.txt \
           ~/Documents/ga-circuit-records/different-filters/run4-1/GARecords.txt -f -F -d
```

[1]: ./samples
[2]: https://www.swarthmore.edu/NatSci/echeeve1/Ref/mna/MNA1.html
[3]: https://www.ginac.de/
