# Cuttlebone

**This project is no longer maintained. See my new work-in-progress replacement <https://github.com/kybr/br>.**

This is a framework / toolkit for developing distributing multimedia
applications. Use the Maker<State> and Taker<State> abstractions to distribute
and collect application state information between apps on your wired network
using udp broadcast. This system may be used with [Cuttlefish][] or in the
[AlloSphere][], using [AlloSystem][]. This framework depends on c++11 features
and POSIX networking.

## Compiling examples, tests, and experiments

    mkdir build
    cd build
    cmake ..
    make -j3

## Using with AlloSystem

Use flags.txt to compile using c++11 (and libcxx on osx). Use al::Simulator,
al::AudioRenderer, and al::GraphicsRenderer in conjunction with
cuttlebone::Maker and cuttlebone::Taker.

[Cuttlefish]: https://github.com/systemics/cuttlefish
[AlloSphere]: http://www.allosphere.ucsb.edu
[AlloSystem]: https://github.com/AlloSphere-Research-Group/AlloSystem
[AlloProject]: https://github.com/AlloSphere-Research-Group/AlloProject
