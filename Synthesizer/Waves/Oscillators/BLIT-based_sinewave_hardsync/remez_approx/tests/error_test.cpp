// g++ -O3 error.test.cpp

#include <fstream>
#include <cmath>
#include "../remez_approx.h"
#include "fastonebigheader.h" // https://code.google.com/p/fastapprox/
using namespace std;

int main()
{
    const int div = 1000;
    {
        ofstream f("fastersin_error.dat");
        for( int i = -div; i < div; i++ )
        {
            float x = 2.0*M_PI*i/div;
            f << x
              << ", "
              << ::fabs(::fastersin(x) - ::sinf(x))
              << endl;
        }
    }
    
    {
        ofstream f("fastertan_error.dat");
        for( int i = 0; i < div-100; i++ )
        {
            float x = M_PI/2*i/div;
            f << x
            << ", "
            << ::fabs(::fastertan(x) - ::tanf(x))
            << endl;
        }
    }
    
    {
        ofstream f("remez_sin_float32_error.dat");
        for( int i = -div; i < div; i++ )
        {
            float x = 2.0*M_PI*i/div;
            f << x
            << ", "
            << ::fabs(::remez_sin_float32(x) - ::sinf(x))
            << endl;
        }
    }
    
    {
        ofstream f("remez_sin_int32_error.dat");
        for( int i = -4*div; i < 4*div; i++ )
        {
            double x = M_PI*i/div;
            f << x
            << ", "
            << ::fabs(::remez_sin_int32(INT32_MAX/div*i) - ::sin(x))
            << endl;
        }
    }
    
    {
        ofstream f("remez_cos_int32_error.dat");
        for( int i = -4*div; i < 4*div; i++ )
        {
            double x = M_PI*i/div;
            f << x
            << ", "
            << ::fabs(::remez_cos_int32(INT32_MAX/div*i) - ::cos(x))
            << endl;
        }
    }
    
    {
        ofstream f("remez_tan_int32_error.dat");
        for( int i = 0; i < div-100; i++ )
        {
            double x = M_PI/2*i/div;
            f << x
            << ", "
            << ::fabs(::remez_tan_int32(INT32_MAX/2/div*i) - ::tan(x))
            << endl;
        }
    }

    return 0;
}
