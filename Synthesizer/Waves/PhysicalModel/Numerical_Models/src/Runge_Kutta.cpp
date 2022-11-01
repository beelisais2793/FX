/*
Name: Alex Greenhalgh
Date: 3/31/2021
Description: 4th order Runge Kutta method, c++ for calculation, results output to RK_output.txt
for easy visualization in matlab or python.
Process: Created RungeKutta class so an object can be created to solve particular problems with method
*/

#include <fstream> //Writing out to file
using namespace std;

//RungeKutta class with the necessary functions to approximate ODE
class RungeKutta {
    public: 
        RungeKutta(int part);
        //Initializing all variables of RungeKutta Class
        int T = 0; // [s]
        double Y = 0; //[bonds/nm^2] 
        int timemax; //[s]
        int delT; // [s]
        unsigned int stepnum; //[timesteps]
        double kon; //[m3/s/mol]
        double koff; //[1/s]
        double Lim;  //[sites/nm2]
        double Tm; //[nM]

        //Functions of RungeKutta
        double firstaux(); // First auxillary function of Runge Kutta
        double secaux(double k1); // Second auxillary function of Runge Kutta
        double thirdaux(double k2); // Third auxillary function of Runge Kutta
        double fouraux(double k3); // Fourth auxillary function of Runge Kutta
        double diffeq(double Y); // Differential equation to numerically approximate
};

//Constructor for RungeKutta, input determines which variables the object is constructed with 
RungeKutta::RungeKutta(int part) {
    switch(part) {
        //Initialize variables for part 1
        case 1:
            delT = 200; //timestep
            timemax = 5000; //[s]
            stepnum = timemax / delT + 1; //+1 to include 0 as well
            kon = 3.2e-3; //[m3/s/mol]
            koff =  1.2e-3; //[1/s]
            Lim =  0.1; //[sites/nm^2]
            Tm = 0.2; //[nM]
            break;
        // Initialize Variables for part 2
        case 2:
            delT = 600; //timestep
            timemax = 60000; //[s]
            stepnum = timemax / delT + 1; //+1 to include 0 as well
            kon = 1.6e-6; //[1/s/nM]
            koff =  1.5e-4; //[1/s]
            Lim =  0.025; //[sites/nm^2]
            Tm = 50; //until 400 min, 24000 seconds
            break;
    }
}


//diffeq function to evaluate, only one input variable of Y since t isn't explicit here 
double RungeKutta::diffeq(double Y) {
    //Y is [T*Lim]
    double ans = (kon * Tm) * (Lim - Y) - koff * (Y);
    return ans;
}

//first aux function, returns k1, no input necessary since T and Y are within scope of function
double RungeKutta::firstaux() {
    double k1 = delT * diffeq(Y);
    return k1;
}

//second aux function, returns k2 w/ k1 input
double RungeKutta::secaux(double k1) {
    double k2 = delT * diffeq(Y + 0.5 * k1);
    return k2;
}

//Third aux function, returns k3 w/ k2 input
double RungeKutta::thirdaux(double k2) {
    double k3 = delT * diffeq(Y + 0.5 * k2);
    return k3;
}

//fourth aux function, returns k4 w/ k3 input
double RungeKutta::fouraux(double k3) {
    double k4 = delT * diffeq(Y + k3);
    return k4;
}

int main() {

    //Part I

    //Create instance of RungeKutta class, part1, to solve part 1
    RungeKutta part1(1);
    //Create array for output of Runge Kutta method part 1, storing T, Y, k1, k2, k3, k4 values in that order
    double outarrp1[part1.stepnum][6];

    for (unsigned int i = 0; i < part1.stepnum; ++i) {
        // Compute k1, k2, k3, k4, load values into output array 
        outarrp1[i][1] = part1.T; //[s] T is int and outarr1 is double but casting is automatic
        outarrp1[i][2] = part1.Y; //[bonds/nm^2]
        outarrp1[i][3] = part1.firstaux(); //k1
        outarrp1[i][4] = part1.secaux(outarrp1[i][3]); //k2
        outarrp1[i][5] = part1.thirdaux(outarrp1[i][4]); //k3
        outarrp1[i][6] = part1.fouraux(outarrp1[i][5]); //k4

        //Update T and Y approximation and advance to next timestep, New Y is weighted average of k1, k2, k3, k4 + Y0
        part1.Y += ((outarrp1[i][3] + 2 * outarrp1[i][4] + 2 * outarrp1[i][5] + outarrp1[i][6]) / 6); 
        part1.T += part1.delT;      
    }
    
    //print T ,Y , k1, k2, k3, k4 to output file
    ofstream fout("RK_output.txt");
    fout << "Part 1:\nT " << "Y " << "k1 " << "k2 " << "k3 " << "k4\n";
    //Output array into text file
    for(unsigned int i = 0; i<part1.stepnum; ++i) {
        fout << outarrp1[i][1] << " " << outarrp1[i][2] << " " << outarrp1[i][3] << " " << outarrp1[i][4] << " " << outarrp1[i][5] << " " << outarrp1[i][6] << "\n";
    }

    //Part II

    //Create part2 object to solve part 2
    RungeKutta part2(2);
    int switcht = 24000; // [s]
    //Create array for output of Runge Kutta method part 2, storing T, Y, k1, k2, k3, k4 values in that order
    double outarrp2[part2.stepnum][6];

    for (unsigned int i = 0; i < part2.stepnum; ++i) {
        // Compute k1, k2, k3, k4, load values into output array 
        outarrp2[i][1] = part2.T; //[s]
        outarrp2[i][2] = part2.Y; //[bonds/nm^2]
        outarrp2[i][3] = part2.firstaux(); //k1
        outarrp2[i][4] = part2.secaux(outarrp2[i][3]); //k2
        outarrp2[i][5] = part2.thirdaux(outarrp2[i][4]); //k3
        outarrp2[i][6] = part2.fouraux(outarrp2[i][5]); //k4

        //Update T and Y approximation and advance, New Y is weighted average of k1, k2, k3, k4 + Y0
        part2.Y += ((outarrp2[i][3] + 2 * outarrp2[i][4] + 2 * outarrp2[i][5] + outarrp2[i][6]) / 6);
        part2.T += part2.delT;    

        //If statement for timeswitch, occurs at 400 min
        if(  part2.T >= switcht) {
            part2.Tm = 0;
        }
    }

     //print T ,Y , k1, k2, k3, k4 to output file
    fout << "\n\nPart 2:\nT " << "Y " << "k1 " << "k2 " << "k3 " << "k4\n";
    for(unsigned int i = 0; i < part2.stepnum; ++i) {
        fout << outarrp2[i][1] << " " << outarrp2[i][2] << " " << outarrp2[i][3] << " " << outarrp2[i][4] << " " << outarrp2[i][5] << " " << outarrp2[i][6] << "\n";
    }
    fout.close();

    return 0;
}
