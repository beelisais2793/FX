#include "RungeKutta.hpp"






RungeKutta::RungeKutta( Real x1, Real x2 , Real ts , unsigned int N , unsigned int Ntime ){

	xmin             =  x1;
	xmax             =  x2;
	Nbins            =  N;
	TimeStep         =  ts;
	NumberTimeSteps  =  Ntime;

	Real dx = ( xmax - xmin ) / Nbins;
	Positions.resize( NumberTimeSteps );
	Velocities.resize( NumberTimeSteps );
	for ( auto i = 0 ; i < NumberTimeSteps ; i++ ){
		Positions[ i ].resize( Nbins );
		Velocities[ i ].resize( Nbins );
		// set initial positions
	}
	for ( auto i = 0 ; i < Nbins ; i++ ){
		Positions[0][i] = xmin + dx * i + 0.5 * dx;
	}
	InitVeloSet  =  false;
}



void RungeKutta::Set_xmin( Real x ){
	xmin  =  x;
}



void RungeKutta::Set_xmax( Real x ){
	xmax  =  x;
}



void RungeKutta::Set_Nbins( unsigned int N ){
	Nbins  =  N;
}



void RungeKutta::Set_TimeStep( Real x ){
	TimeStep  =  x;
}



void RungeKutta::Set_NumberTimeSteps( unsigned int N ){
	NumberTimeSteps  =  N;
}



void RungeKutta::Integrate( std::function<Real( Real , Real , Real )> Force ){


	if ( !InitVeloSet ){
		GenerateRandomVelos();
	}

	Real time = 0;
	for ( auto i = 1 ; i < NumberTimeSteps ; i++ ){
		ComputeSingleTimeStep( Positions[i-1] , 
				       Positions[i] , 
				       Velocities[i-1] ,
				       Velocities[i] ,
				       time , Force );
		time  =  time  +  TimeStep;
	}
}



void RungeKutta::ComputeSingleTimeStep( const std::vector<Real>& PositionsOld , 
		                        std::vector<Real>& Positions ,
                                        const std::vector<Real>& VelocitiesOld , 
		                        std::vector<Real>& Velocities , Real time ,
		                        std::function<Real( Real , Real , Real )> Force ){


	std::vector<Real> PosOld;
	PosOld = Positions;
	// compute velocities
	for ( auto i = 0 ; i < Nbins ; i++ ){
		Real TstepHalf  = time + TimeStep * 0.5;
		Real force = Force( PositionsOld[i] , VelocitiesOld[i] , time );
		Real v1  =  force * TimeStep;
		Real k1  =  TimeStep * v1;
		force    =  Force( PositionsOld[i] + 0.5*k1 , 
				   VelocitiesOld[i] + 0.5*v1 , TstepHalf );
		Real v2  =  force * TimeStep;
		Real k2  =  TimeStep * v2;
		force    =  Force( PositionsOld[i] + 0.5*k2 , 
				   VelocitiesOld[i] + 0.5*v2 , TstepHalf );
		Real v3  =  force * TimeStep;
		Real k3  =  TimeStep * v3;
		force    =  Force( PositionsOld[i] + 0.5*k3 , 
				   VelocitiesOld[i] + 0.5*v3 , TstepHalf );
		Real v4  =  force * TimeStep;
		Real k4  =  TimeStep * v2;
		Positions[i] =  PositionsOld[i] + ( k1 +  2*k2  +  2*k3  + k4 ) / 6;
		Velocities[i]  =  Velocities[i] + ( v1 +  2*v2  +  2*v3  + v4 ) / 6;
	}
}



void RungeKutta::GenerateRandomVelos( void ){

	// First create an instance of an engine.
	std::random_device rnd_device;
        // Specify the engine and distribution.
	std::mt19937 mersenne_engine { rnd_device() };  // Generates random floats
	// use Gaussian distribution
        std::normal_distribution<Real> dist { 0 , 1 };
        
        auto gen = [ &dist, &mersenne_engine ](){
                       return dist( mersenne_engine );
                   };

	std::generate( std::begin( Velocities[0] ), Velocities[0].end() , gen );
	//for ( auto i = 0 ; i < Velocities.size() ; i++ ){
	//	std::cout << Velocities[i] << std::endl;
	//}
}



void RungeKutta::WriteOutput( std::string fname ){
	

	std::ofstream outfile;
	outfile.open( fname );

	for ( auto i = 0 ; i < NumberTimeSteps ; i++ ){
		Real time = i * TimeStep;
		outfile << time << "   ";
		for ( auto j = 0 ; j < Nbins ; j++ ){
			outfile << Positions[i][j] << "   ";
		}
		outfile << std::endl;
	}
	outfile.close();
}



std::vector<std::vector<Real>> RungeKutta::GivePositions( void ){
	return Positions;
}



std::vector<std::vector<Real>> RungeKutta::GiveVelocities( void ){
	return Velocities;
}
