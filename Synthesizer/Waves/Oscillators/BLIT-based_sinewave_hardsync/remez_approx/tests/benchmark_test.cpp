// benchmark_test.cpp

#include <iostream>
#include <time.h>

#include "../remez_approx.h"
#include "../tests/fastonebigheader.h"
using namespace std;

static const int DIV = 1000000000;

// fastersin test
static void test_fastersin()
{
    float sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV; i < DIV; i++ )
    {
        float x = 2.0f*3.141592f/DIV*i;
        sum += fastersin(x*5/3);
    }
    cout << "fastersin"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_sin_float32 test
static void test_remez_sin_float32()
{
    float sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV; i < DIV; i++ )
    {
        float x = 2.0f*3.141592f/DIV*i;
        sum += remez_sin_float32(x*5/3);
    }
    cout << "remez_sin_float32"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// sinf test
static void test_sinf()
{
    float sum = 0.0f;
    clock_t start = clock();
    for( int i = -DIV; i < DIV; i++ )
    {
        float x = 2.0f*3.141592f/DIV*i;
        sum += sinf(x*5/3);
    }
    cout << "sinf"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// sin test
static void test_sin()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV; i < DIV; i++ )
    {
        double x = 2.0*M_PI/DIV*i;
        sum += sin(x*5/3);
    }
    cout << "sin"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_sin_float64 test
static void test_remez_sin_float64()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV; i < DIV; i++ )
    {
        double x = 2.0*M_PI/DIV*i;
        sum += remez_sin_float64(x*5/3);
    }
    cout << "remez_sin_float64"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}


// remez_sin_int32 test
static void test_remez_sin_int32()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int32_t i = -DIV; i < DIV; i++ )
    {
        int32_t x = INT32_MAX/DIV*i;
        sum += remez_sin_int32(x*5/3);
    }
    cout << "remez_sin_int32"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_sin_int64 test
static void test_remez_sin_int64()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int64_t i = -DIV; i < DIV; i++ )
    {
        int64_t x = INT64_MAX/DIV*i;
        sum += remez_sin_int64(x*5/3);
    }
    cout << "remez_sin_int64"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_cos_int32 test
static void test_remez_cos_int32()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int32_t i = -DIV; i < DIV; i++ )
    {
        int32_t x = INT32_MAX/DIV*i;
        sum += remez_cos_int32(x*5/3);
    }
    cout << "remez_cos_int32"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_cos_int64 test
static void test_remez_cos_int64()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int64_t i = -DIV; i < DIV; i++ )
    {
        int64_t x = INT64_MAX/DIV*i;
        sum += remez_cos_int64(x*5/3);
    }
    cout << "remez_cos_int64"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// fastertan test
static void test_fastertan()
{
    float sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV+100; i < DIV-100; i++ )
    {
        float x = 3.141592f/2/DIV*i;
        sum += fastertan(x);
    }
    cout << "fastertan"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// tanf test
static void test_tanf()
{
    float sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV+100; i < DIV-100; i++ )
    {
        float x = 3.141592f/2/DIV*i;
        sum += tanf(x);
    }
    cout << "tanf"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}


// tan test
static void test_tan()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int i = -DIV+100; i < DIV-100; i++ )
    {
        double x = M_PI/2/DIV*i;
        sum += tan(x);
    }
    cout << "tan"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_tan_int32 test
static void test_remez_tan_int32()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int32_t i = -DIV+100; i < DIV-100; i++ )
    {
        int32_t x = INT32_MAX/2/DIV*i;
        sum += remez_tan_int32(x);
    }
    cout << "remez_tan_int32"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

// remez_tan_int64 test
static void test_remez_tan_int64()
{
    double sum = 0.0;
    clock_t start = clock();
    for( int64_t i = -DIV+100; i < DIV-100; i++ )
    {
        int64_t x = INT64_MAX/2/DIV*i;
        sum += remez_tan_int64(x);
    }
    cout << "remez_tan_int64"
    << " time:" << static_cast<double>(clock()-start)/CLOCKS_PER_SEC
    << " sum:" << sum << endl;
}

int main()
{
    test_sinf();
    test_sin();
    
    test_fastersin();
    test_remez_sin_float32();
    test_remez_sin_float64();
    test_fastersin();
    test_remez_sin_float32();
    test_remez_sin_float64();

    test_remez_sin_int32();
    test_remez_sin_int64();
    test_remez_sin_int32();
    test_remez_sin_int64();
    
    test_remez_cos_int32();
    test_remez_cos_int64();
    test_remez_cos_int32();
    test_remez_cos_int64();
    
    test_tanf();
    test_tan();
    test_fastertan();
    test_remez_tan_int32();
    test_remez_tan_int64();
    test_fastertan();
    test_remez_tan_int32();
    test_remez_tan_int64();
    
    return 0;
}

/*
g++ -O3 benchmark_test.cpp
./a.out
 
 sinf time:18.8891 sum:-1.67772e+07
 sin time:31.7055 sum:0.866025
 fastersin time:7.54966 sum:4.29497e+09
 remez_sin_float32 time:7.19581 sum:4.29497e+09
 remez_sin_float64 time:6.82226 sum:-190.64
 fastersin time:7.59518 sum:4.29497e+09
 remez_sin_float32 time:7.20716 sum:4.29497e+09
 remez_sin_float64 time:6.8095 sum:-190.64
 remez_sin_int32 time:6.44008 sum:0.594805
 remez_sin_int64 time:6.05396 sum:-0.866318
 remez_sin_int32 time:6.46647 sum:0.594805
 remez_sin_int64 time:6.06535 sum:-0.866318
 remez_cos_int32 time:6.21992 sum:1.68124e+09
 remez_cos_int64 time:6.362 sum:1.65423e+09
 remez_cos_int32 time:6.31997 sum:1.68124e+09
 remez_cos_int64 time:6.39792 sum:1.65423e+09
 tanf time:18.6343 sum:-2.3088e+06
 tan time:31.4872 sum:-6.3662e+06
 fastertan time:9.56093 sum:-2.16804e+06
 remez_tan_int32 time:14.5696 sum:-7.61073e-07
 remez_tan_int64 time:14.5381 sum:-6.42169e+06
 fastertan time:9.62786 sum:-2.16804e+06
 remez_tan_int32 time:14.6605 sum:-7.61073e-07
 remez_tan_int64 time:14.5146 sum:-6.42169e+06

*/
