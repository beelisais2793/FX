#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/traits.hpp>
#include <QDebug>
#include <iomanip>

using namespace boost::numeric::ublas;
    
/*
    Transfer function class implemented as 
    State Space system with A, B, C, D
    control matrices
*/
class TransferFunction
{
public:
    TransferFunction(double a, double b, double c, double d, double x0 = 0.0, bool doPrint = false);
    TransferFunction(matrix<double> a, matrix<double> b, matrix<double> c, matrix<double> d, double x0 = 0.0, bool doPrint = false);


    // Computes next output with given input
    double computeNextOutput(double nextInput);

    /*
        Simulates output of transfer function with
        given input U during time T.
        If T is not given it creates automaticaly
    */
    matrix<double> simulate(const vector<double>& U, double T = 10.0);

    // void clearInitialValues() {}

    // debug
    static void print(QString name, matrix<double>& M);
    static void print(QString name, vector<double>& V);

private:
    double initialValue = 0.0;
    double prevInput = 0.0;

    // Матрицы пространства состояний
    matrix<double> A;
    matrix<double> B;
    matrix<double> C;
    matrix<double> D;

    // Матриц для рассчета выходного сигнала
    matrix<double> xout;
    matrix<double> M;
    matrix<double> expM;
    matrix<double> Ad;
    matrix<double> Bd0;
    matrix<double> Bd1;

    void init(bool doPrint);
    void calc_matrix();
    bool invertMatrix(const matrix<double>& input, matrix<double>& inverse);
    matrix<double> expm(const matrix<double> H, double t, const int p);
};


#endif // TRANSFERFUNCTION_H
