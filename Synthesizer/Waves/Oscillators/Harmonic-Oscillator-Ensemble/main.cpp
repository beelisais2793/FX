//main.cpp
#include "vose.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

# define PI 3.14159265358979323846

double potential(double*);
double kinetic(double*);
double Hamiltonian(double*, double*, unsigned int);
void update_state(double*, double*, unsigned int, double);
void write_state(FILE*, double, double*, double*, unsigned int);

int main(int argc, char *argv[])
{
    auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 mt(seed); // Seeds Mersenne Twister with Device RNG
    
    
    // Define the density array for position
    unsigned int grid_size = 10000; //number of grid points
    
    double *density = new double[grid_size];
    double arg_scale = 2*PI/grid_size;
    double norm = 0;
    for (unsigned int i = 0; i<grid_size; i++)
    {
        density[i] = 0.5*sin(arg_scale*i)+1;
        norm += density[i];
    }
    
    //Normalize position density
    for (unsigned int i = 0; i<grid_size; i++)
    {
        density[i] /= norm;
    }
    
    // Sample locations
    vose *sampler = new vose(density, grid_size, mt);
    
    double length = 10;
    
    unsigned int particle_number = 10;
    
    double *positions = new double[grid_size];
    for (unsigned int i=0; i<particle_number; i++)
    {   
        positions[i] = sampler->alias_method()*length/grid_size;
    }

    //Sort locations (just so IC is nicer)
    sort(positions, positions+particle_number);
    
    // Define momentum density
    double *mom_density = new double[grid_size];
    double mass = 1.5, Temperature = 0.0009;
    arg_scale = -1/(2*mass*Temperature);
    norm = 0;
    for (unsigned int i = -floor(grid_size/2); i<floor(grid_size/2); i++)
    {
        mom_density[i] = exp(arg_scale*i*i);
        norm += mom_density[i];
    }
    
    //Normalize momentum density
    for (unsigned int i = 0; i<grid_size; i++)
    {
        mom_density[i] /= norm;
    }
    
    double *momenta = new double[grid_size];
    double mom_scale = 0.0005;
    unsigned int j;
    for (unsigned int i=0; i<particle_number; i++)
    {   
        j = sampler->alias_method(); //momentum index
        momenta[i] = (j-floor(grid_size/2))*mom_scale; //convert j to momentum
    }
    
    for (unsigned int i=0; i<particle_number; i++)
    {
        cout << momenta[i] << '\t';
    }
    cout << endl;
    
    /*--------------------------------------------*/
    
	//Create output file
	FILE *output_file;
	output_file = fopen("output.tsv", "w");
	fprintf(output_file, "Time\tPositions\tMomenta\n");
    
    // Hamiltonian solution part
    double dt = 0.001;
    double max_time = 200;
    
    for (double t = 0; t<max_time; t+=dt)
    {
        update_state(positions, momenta, particle_number, dt);
        write_state(output_file, t, positions, momenta, particle_number);
    }
    
    return 0;
}

double potential(double *positions, unsigned int particle_number)
{
    double k = 0.1;
    double potential_energy = 0;
    double scale = k/2;
    double length = 10;
    
    for (unsigned int i = 0; i<=particle_number; i++)
    {
        if (i==0)
            potential_energy += scale*pow(positions[0],2);
        else if (i==particle_number)
            potential_energy += scale*pow(length-positions[i],2);
        else
            potential_energy += scale*pow(positions[i]-positions[i-1],2);
    }
    
    return potential_energy;
}

double kinetic(double *momenta, unsigned int particle_number)
{
    double kinetic_energy = 0;
    double mass = 10;
    double scale = 1/(2*mass);
    
    for (unsigned int i = 0; i<particle_number; i++)
    {
        kinetic_energy += scale*pow(momenta[i],2);
    }
    
    return kinetic_energy;
}

double Hamiltonian(double *positions,double *momenta,unsigned int particle_number)
{
    return potential(positions,particle_number) + kinetic(momenta,particle_number);
}

//Gauss-Seidel I think
void update_state(double *pos, double *mom, unsigned int part_num, double dt)
{
    double dx = 0.001, dp = 0.001; //finite differences
    double H_pplus, H_pminus, H_xplus, H_xminus;
    double p_grad, x_grad;
    
    for (unsigned int i = 0; i<part_num; i++)
    {
        //Calculate phase space gradient
        mom[i] += dp;
        H_pplus = Hamiltonian(pos, mom, part_num);
        mom[i] -= 2*dp;
        H_pminus = Hamiltonian(pos, mom, part_num);
        mom[i] += dp;
        p_grad = (H_pplus-H_pminus)/(2*dp);
        
        pos[i] += dx;
        H_xplus = Hamiltonian(pos, mom, part_num);
        pos[i] -= 2*dx;
        H_xminus = Hamiltonian(pos, mom, part_num);
        pos[i] += dx;
        x_grad = (H_xplus-H_xminus)/(2*dx);
        
        //Update
        mom[i] -= dt*x_grad;
        pos[i] += dt*p_grad;
        
    }
    //cout << Hamiltonian(pos, mom, part_num) << endl;
}

void write_state(FILE *fp, double t, double *pos, double *mom, unsigned int part_num)
{
    //Write time
    fprintf(fp,"%f\t",t);
    
    //Write positions
    for (unsigned int i = 0; i<part_num; i++)
    {
        fprintf(fp,"%f\t",pos[i]);
    }
    
    //Write momenta
    for (unsigned int i = 0; i<part_num; i++)
    {
        fprintf(fp,"%f",mom[i]);
        if (i<part_num-1)
            fprintf(fp,"\t");
    }
    
    fprintf(fp,"\n");
    
}