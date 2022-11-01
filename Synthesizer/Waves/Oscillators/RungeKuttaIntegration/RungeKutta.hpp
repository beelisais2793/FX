#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <algorithm>
#include <iterator>
#include <string>   
#include <fstream>
                    
                    


#ifdef USEDOUBLES
typedef double Real;
#else
typedef float Real;
#endif






class RungeKutta{


	private:
		Real stepwidth;
		Real xmin;
		Real xmax;
		unsigned int Nbins;
		Real TimeStep;
		Real NumberTimeSteps;
		std::vector<std::vector<Real>> Positions;
		std::vector<std::vector<Real>> Velocities;
                void GenerateRandomVelos( void );
		bool InitVeloSet;
                void ComputeSingleTimeStep( const std::vector<Real>& PositionsOld , 
		                            std::vector<Real>& Positions ,
                                            const std::vector<Real>& VelocitiesOld , 
		                            std::vector<Real>& Velocities , Real time ,
		                            std::function<Real( Real , Real , Real )> Force );




        public:
		RungeKutta( void ){};
		RungeKutta( Real x1, Real x2 , Real ts , unsigned int N , unsigned int Ntime );
		void Set_xmin( Real x );
		void Set_xmax( Real x );
		void Set_Nbins( unsigned int N );
		void Set_TimeStep( Real x );
		void Set_NumberTimeSteps( unsigned int N );
		void Integrate( std::function< double( Real , Real , Real )> Force );
		void SetInitialVelocities( std::vector<Real>& InitVelos );
                void WriteOutput( std::string fname );
		std::vector<std::vector<Real>> GivePositions( void );
		std::vector<std::vector<Real>> GiveVelocities( void );

};
