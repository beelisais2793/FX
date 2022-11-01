#include <iostream>
#include <fstream>
#include <functional>
#include <cmath>

#include "RungeKutta.hpp"






class DuffingOszi{


	private:
		double gamma;
		double a;
		double b;
		double omega;
		double F0;


	public:
		DuffingOszi( double gamma , double a1 , double b1 , double omega1 , double F );
		double GiveForce( double position , double velocity , double time );

};


DuffingOszi::DuffingOszi( double gamma1 , double a1 , double b1 , double omega1 , double F ){
	gamma  =  -gamma1;
	a      =   2 * a1;
	b      =  -4 * b1;
	omega  =  omega1;
	F0     =  F;
}



double DuffingOszi::GiveForce( double position , double velocity , double time ){

	return gamma * velocity + a * position + 
		   b * position * position * position + 
		   F0 * cos( omega * time );
}






int main( void ){


	//RungeKutta integrator;

	//integrator.Set_xmin( 0.10 );
	
	double timestep = 0.1;

	RungeKutta integrator( (double) 0.0 , (double) 1.0 , timestep , (unsigned int) 100 ,
			        1000 );
	DuffingOszi oscillator( 1 , 0.25 , 0.5 , 0.1 , 1 );
	// create function pointer of force routine
	auto Force = std::bind( &DuffingOszi::GiveForce , oscillator ,  
			std::placeholders::_1 , 
			std::placeholders::_2 , 
			std::placeholders::_3 );
	integrator.Integrate( Force );

	integrator.WriteOutput( "Duffing.dat" );

	std::vector<std::vector<Real>> Positions  =  integrator.GivePositions();
	std::vector<std::vector<Real>> Velocities =  integrator.GiveVelocities();

	std::ofstream outfile;
	outfile.open( "StrangeAttractor.dat" );

	for ( auto j = 0 ; j < Positions[0].size() ; j++ ){
		for ( auto i = 0 ; i < Positions.size() ; i++ ){
	        	outfile << Positions[i][j] << "   " <<
	        		     Velocities[i][j] << std::endl;
	        }
	}
	outfile.close();
	







}
