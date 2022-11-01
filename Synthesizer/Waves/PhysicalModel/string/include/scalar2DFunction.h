#ifndef SCALAR2DFUNCTION_H
#define SCALAR2DFUNCTION_H
#include <vector>

using namespace std;

class scalar2DFunction
{
	public:
		scalar2DFunction();
		virtual ~scalar2DFunction();
		vector<vector<float>> getTable ();
		vector<float> getLastString ();
		void readTable(int quantityOfXVal, int quantityOfTVal);
		void printTable();
		void printTableWithNumbers();
		void printLastString();
		void readMaxes();
		void printMaxes();
		float gettmax();
		float gettmin();
		float getxmax();
		float getxmin();
		float tstep();
		float xstep();
		int inRange(int x, int t);			  //x and t - indexes of vector<vector<float>> table
		int inRangePrint(int x, int y);
		float tderivative (int x, int t);
		float xderivative (int x, int t);
		float tderivative (int x, int t, int n); //n is the degree of derivative
		float xderivative (int x, int t, int n);
	protected:
		vector<vector<float>> table;
		float xmin, xmax, tmin, tmax;
	private:
};

#endif // SCALAR2DFUNCTION_H
