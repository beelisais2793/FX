#include "SimpleHarmonicOscillator.hxx"
#include <iostream>

int main(void)
{
    SpringMass SHO(0.5,1.5, 0.3);
    std::cout << SHO.getEquilibriumPosition() << std::endl;
    SHO.displace(0.2);

    SimplePendulum pend(1., 0.5);
    std::cout << pend.getAngularVelocity() << std::endl;
    pend.displace(0.05);
}