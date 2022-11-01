## BoidUGens ##

Chaotic oscillator based on a boids artificial life simulation. Forked from [SWUGens](http://www2.informatik.hu-berlin.de/~oberthol/SWUGens-0.01/) by Oswald Berthold.

## Installation

OS X and Linux:

    mkdir build && cd build
    cmake -DSC_PATH="/path/to/supercollider/" ..
    make

Here, `/path/to/supercollider/` is the path to a directory of the SuperCollider source code. The path should contain a file at `include/plugin_interface/SC_PlugIn.h`. If you get a warning that `SC_PlugIn.h` could not be found, then `SC_PATH` is not set correctly.

After building, install the directory as you would a quark. You can copy/move/symlink it in your Extensions directory, or go to `Quarks.gui` > Install a folder.

If you know how to build on Windows, please let me know. The process is identical to building sc3-plugins, but I couldn't find any specific instructions.