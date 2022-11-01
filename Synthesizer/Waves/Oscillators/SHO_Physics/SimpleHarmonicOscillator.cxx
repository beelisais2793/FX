//======================================================================//
//                      Simple Harmonic Oscillator                      //
//  Classes for simulating a mass on a spring with the option of adding //
//  a value for coefficient of damping.                                 //
//                                                                      //
//  author   :   Kristian Zarebski                                      //
//  date     :   last modified  2018-01-01                              //
//                                                                      //
//======================================================================//

#include "SimpleHarmonicOscillator.hxx"
#include <cmath>
#include <iostream>
#include <stdlib.h>

// Initialise the components of the SpringMass class and create output
SpringMass::SpringMass(double mass, double spg_const, double damp_coef)
{
    _spg_const = spg_const;
    _mass = mass;
    _damp_coef = damp_coef;
    _ang_vel = pow(_spg_const/_mass, 0.5);
    _equil_dis = _mass*g/_spg_const;
    std::ofstream out_file("SHO_springmass_m_"+std::to_string(_mass)+"_k_"\
    +std::to_string(_spg_const)+"_b_"+std::to_string(_damp_coef)+".txt");
    out_file.close();
}

// Returns the new maximum displacement with damping applied
double SpringMass::_get_damped_max_at(double time)
{
    return _initial_dis*exp(-1*(_damp_coef*time)/(2*_mass));
}

// Return the Force Magnitude and Direction
double SpringMass::getAcceleration(double x)
{
    return (_spg_const*(x-_equil_dis)+_mass*g)/_mass;
}

// Insert Data Point Into Output File for amplitude at given time
void SpringMass::_record_data(double time, double amplitude)
{
    std::ofstream out_file("SHO_m_"+std::to_string(_mass)+"_k_"\
    +std::to_string(_spg_const)+"_b_"+std::to_string(_damp_coef)+".txt",
    std::ios::out | std::ios::app);
    
    out_file << time << "\t" << amplitude;
    out_file << "\t" << getAcceleration(amplitude) << std::endl;
    out_file.close();
}

// Calculate the amplitude of displacement at a given time
double SpringMass::_get_displacement(double time)
{
    double _dis = _get_damped_max_at(time);
    _dis *= cos(_ang_vel*time);

    return _dis;
}

// Set initial displacement (and release the mass!)
void SpringMass::displace(double amplitude, double time)
{
    _initial_dis = amplitude;

    for(int i{0}; i<=10*time; ++i)
    {
        _record_data(i/10., _get_displacement(i/10.));
    }
}

// Return the angular velocity omega = sqrt(k/m)
double SpringMass::getAngularVelocity()
{
    return _ang_vel;
}

double SpringMass::getEquilibriumPosition(){ return _equil_dis;}


SimplePendulum::SimplePendulum(double mass, double length)
{
    _mass = mass;
    _length = length;
    _ang_vel = pow(abs(g)/_length, 0.5);
    std::ofstream out_file("SHO_simplependulum_m_"+std::to_string(_mass)+"_l_"\
    +std::to_string(_length)+".txt");
    out_file.close();
}

double SimplePendulum::_get_displacement(double time)
{
    return _initial_dis*cos(_ang_vel*time);
}

void SimplePendulum::_record_data(double time, double amplitude)
{
    std::ofstream out_file("SHO_simplependulum_m_"+std::to_string(_mass)+"_l_"\
    +std::to_string(_length)+".txt",
    std::ios::out | std::ios::app);

    out_file << time << "\t" << amplitude;
    out_file << "\t" << getAcceleration(amplitude) << std::endl;
    out_file.close();
}

void SimplePendulum::displace(double amplitude, double time)
{
    if(abs(amplitude) - abs(sin(amplitude)) > 1E-2)
    {
        std::cout << "Displacement angle too large for simple pendulum, ";
        std::cout << "approximations no longer valid!" << std::endl;

        exit(EXIT_FAILURE);
    }
    _initial_dis = amplitude;

    for(int i{0}; i<=10*time; ++i)
    {
        _record_data(i/10., _get_displacement(i/10.));
    }
}

double SimplePendulum::getAngularVelocity()
{
    return _ang_vel;
}

double SimplePendulum::getAcceleration(double theta)
{
    return _mass*g*sin(_ang_vel*theta);
}