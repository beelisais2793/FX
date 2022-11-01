// StdLite
%module stdlite
%{
#include "stdlite.h"
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace Std;
%}

%inline %{
typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed long long i64;
typedef unsigned long long u64;
%}

%include "stdint.i"
%include "std_common.i"
%include "std_vector.i"
%include "std_string.i"
%include "lua_fnptr.i"


%template (lua_matrix) std::vector<std::vector<SWIGLUA_REF>>;
%template (float_matrix) std::vector<std::vector<float>>;
%template (double_matrix) std::vector<std::vector<double>>;
%template (ldouble_matrix) std::vector<std::vector<long double>>;
%template (char_matrix) std::vector<std::vector<signed char>>;
%template (uchar_matrix) std::vector<std::vector<unsigned char>>;
%template (short_matrix) std::vector<std::vector<signed short>>;
%template (ushort_matrix) std::vector<std::vector<unsigned short>>;
%template (int_matrix) std::vector<std::vector<signed int>>;
%template (uint_matrix) std::vector<std::vector<unsigned int>>;
%template (long_matrix) std::vector<std::vector<signed long>>;
%template (ulong_matrix) std::vector<std::vector<unsigned long>>;
%template (llong_matrix) std::vector<std::vector<signed long long>>;
%template (ullong_matrix) std::vector<std::vector<unsigned long long>>;


%template(lua_vector) std::vector<SWIGLUA_REF>;
%template(float_vector) std::vector<float>;
%template(double_vector) std::vector<double>;
%template(ldouble_vector) std::vector<long double>;
%template(char_vector) std::vector<signed char>;
%template(uchar_vector) std::vector<unsigned char>;
%template(short_vector) std::vector<signed short>;
%template(ushort_vector) std::vector<unsigned short>;
%template(int_vector) std::vector<signed int>;
%template(uint_vector) std::vector<unsigned int>;
%template(long_vector) std::vector<signed long>;
%template(ulong_vector) std::vector<unsigned long>;
%template(llong_vector) std::vector<signed long long int>;
%template(ullong_vector) std::vector<unsigned long long int>;

%include "stdlite.h"


%template (FloatVector)  Std::StdVector<float>;
%template (DoubleVector) Std::StdVector<double>;
%template( I8Vector) Std::StdVector<i8>;
%template( U8Vector) Std::StdVector<u8>;
%template( I16Vector) Std::StdVector<i16>;
%template( U16Vector) Std::StdVector<u16>;
%template( I32Vector) Std::StdVector<i32>;
%template( U32Vector) Std::StdVector<u32>;
%template( I64Vector) Std::StdVector<i64>;
%template( U64Vector) Std::StdVector<u64>;

%template( FloatArray ) Std::StdArray<float>;
%template( DoubleArray) Std::StdArray<double>;
%template( I8Array )  Std::StdArray<i8>;
%template( U8Array )  Std::StdArray<u8>;
%template( I16Array) Std::StdArray<i16>;
%template( U16Array) Std::StdArray<u16>;
%template( I32Array) Std::StdArray<i32>;
%template( U32Array) Std::StdArray<u32>;
%template( I64Array) Std::StdArray<i64>;
%template( U64Array) Std::StdArray<u64>;
