# Simple Harmonic Oscillator
## Mass on a Spring
To run a simulation of the mass on a spring experiment create an instance of the `MassSpring` class:
```
#include "SimpleHarmonicOscillator.hxx"

int main(void)
{
    const double mass = 0.5; // kg
    const double k = 1.2;    // Nm^-1
    const double b = 0.3;    // Coefficient of damping
    
    MassSpring sho(mass, k, b);
    
    sho.displace(0.5); // Displace mass by 0.5m

};
```

This can then be compiled by running (assuming it is saved as `main.cxx`):

```
$ export SHOROOT=location/of/SimpleHarmonicOscillator.hxx
$ g++ -std=c++14 -I $SHOROOT $SHOROOT/SimpleHarmonicOscillator.cxx main.cxx
```
