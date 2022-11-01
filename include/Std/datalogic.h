#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <deque>
#include <queue>
#include <set>
#include <stack>
#include <regex>
#include <random>

#include "StdObject.h"

namespace Std {
    union Anything
    {    
        char c;
        unsigned char uc;
        int8_t i8;
        uint8_t u8;
        int16_t i16;
        uint16_t u16;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;
        float f;
        double d;
    };

    enum AnyType
    {
        TYPE_STRING,
        TYPE_CHAR,
        TYPE_UCHAR,
        TYPE_INT8,
        TYPE_UINT8,
        TYPE_INT16,
        TYPE_UINT16,
        TYPE_INT32,
        TYPE_UINT32,
        TYPE_INT64,
        TYPE_UINT64,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_VOID
    };


    struct BasicType : public StdObject
    {
        char    c;    
        bool    b;
        double  d;    
        void * ptr;    
        std::string str;        
        std::vector<char> char_vector;
        std::vector<unsigned char> byte_vector;
        std::vector<int8_t>  int8_vector;
        std::vector<uint8_t> uint8_vector;
        std::vector<int16_t>  int16_vector;
        std::vector<uint16_t> uint16_vector;
        std::vector<int32_t>  int32_vector;
        std::vector<uint32_t> uint32_vector;
        std::vector<int64_t>  int64_vector;
        std::vector<uint64_t> uint64_vector;    
        std::vector<float>  float_vector;
        std::vector<double> double_vector;    
        std::vector<long double> ldouble_vector;    
        std::vector<std::string> string_vector;    
        int key;

        
        BasicType& operator = (const BasicType& rhs)
        {
            c = rhs.c;
            b = rhs.b;
            d = rhs.d;
            ptr = rhs.ptr;
            str = rhs.str;
            char_vector = rhs.char_vector;
            byte_vector = rhs.byte_vector;
            int8_vector = rhs.int8_vector;
            uint8_vector = rhs.uint8_vector;
            int16_vector = rhs.int16_vector;
            uint16_vector = rhs.uint16_vector;
            int32_vector = rhs.int32_vector;
            uint32_vector = rhs.uint32_vector;
            int64_vector = rhs.int64_vector;
            uint64_vector = rhs.uint64_vector;
            float_vector = rhs.float_vector;
            double_vector = rhs.double_vector;
            ldouble_vector = rhs.ldouble_vector;
            string_vector = rhs.string_vector;
            return *this;
        }
    };


    bool operator < (const BasicType &a, const BasicType & b) { return a.key < b.key; }
    bool operator == (const BasicType & a, const BasicType b) { return a.key == b.key; }


    template<class A, class B>
    struct Pair : public StdObject
    {
        std::pair<A,B> p;

        

        Pair() {}
        Pair(const Pair<A,B> & pair) : p(pair.p) {}
        Pair(const A& a, const B& b) { p = std::make_pair(a,b); }
        Pair(const std::pair<A,B> & pair) : p(*pair) {} 

        A& first() { return p.first(); }
        B& second()  { return p.second();}
        
    };

}
