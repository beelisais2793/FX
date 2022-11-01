Transfer Function Plotter
=========================

An  application  for  designing, manipulating and visualising the behaviour of
LTI (linear time-invariant) systems based on transfer functions.

Transfer functions are used in the analysis of the dynamic behaviour of many single-input single-output physical processes, typically within the fields of signal processing, communication theory, control theory and in micro electronics.

Features being worked on are:

  - Manipulation:
    + Driving-Point Signal Flow Graph (DPSFG) drawing tool.
    + Standard low order filter designer (1st and 2nd order low/high/band/notch filters)
    + Butterworth filter designer
    + Chebyshev type I and II filter designer
    + Elliptic filter designer
    + Bessel filter designer
  - Visualisation
    + Frequency response (aka Bode plot)
    + Pole-Zero diagram
    + 3D Pole-Zero diagram
    + Time domain step and impulse responses

And here's what it looks like.

[(See animation here)](https://i.imgur.com/c5Inpbb.gifv)


Compiling
=========

You will need Qt 5.x and cmake to build this project.

Linux
-----

```sh
$ mkdir build && cd build
$ cmake ..
$ make -j8
```

Windows
-------

Start your VS command prompt (so vcvarsall.bat is called). Change folder names as needed (the following assumes you have Qt5.9.1 built with msvc2015 installed):

```
set Qt5_DIR=C:\Qt\5.9.1\msvc2015
mkdir build
cd build
cmake -G "Visual Studio 17 2015 Win64"
msbuild "transfer function plotter.sln"
(or just double-click on the solution to open VS)
```

Running
=======

An executable is placed under ```build/bin``` called ```tfp_application(.exe)```. Executing it will open the main application.

Each plugin creates an executable for running its unit tests called ```test_<plugin name>(.exe)```. For example, if you want to run unit tests from the DPSFG plugin, you would execute ```test_dpsfg(.exe)```.

You can also run all unit tests by executing ```test_all(.exe)```.
