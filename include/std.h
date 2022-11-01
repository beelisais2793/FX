#pragma once 
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <random>
#include <memory>

namespace Std {
    inline int  randint(int min, int max) {
        float f = (float)rand() / (float)RAND_MAX;
        return min + (max-min)*f;
    }
    inline int64_t  randint64(int64_t min, int64_t max) {
        float f = (float)rand() / (float)RAND_MAX;
        return min + (max-min)*f;
    }
    inline char randchar() {
        return randint(0,255);
    }
    inline float randfloat(float min, float max) {
        float f = (float)rand() / (float)RAND_MAX;
        return min + (max-min)*f;
    }
    inline double randdbl(double min, double max) {
        double f = (double)rand() / (double)RAND_MAX;
        return min + (max-min)*f;
    }
    inline bool flip(double p = 0.5)
    {
        double f = (double)rand() / (double)RAND_MAX;
        return f > p;
    }

    struct StdRandom
    {
        StdRandom(int64_t seed=-1) { 
            if(seed == -1) srand(time(NULL));
            else srand(seed); 
        }

        float rand() { return randfloat(0,1); }
        int rand(int min, int max) { return randint(min,max); }
        int64_t rand(int64_t min, int64_t max) { return randint(min,max); }
        float rand(float min, float max) { return randfloat(min,max);}
        double rand(double min, double max) { return randdbl(min,max); }
        bool flip(double p =0.5) { return flip(p) ;}
    };

    struct StdRandomUniform
    {
    private:        
        std::mt19937 rng;
        std::uniform_real_distribution<float> distrib;
    public:
        StdRandomUniform() {    
            distrib = std::uniform_real_distribution<float>(0,1);
        }

        float operator()() { return distrib(rng); }
        float random() { return distrib(rng); }
        float random(float min, float max) { return min + (max-min)*random(); }

        void process(size_t n, std::vector<float> & r) {
            r.resize(n);
            for(size_t i = 0; i < n; i++) r[i] = random();
        }
    };

    struct StdRandomUniform64
    {
    private:
        std::random_device dev;
        std::mt19937 rng;
        std::uniform_int_distribution<int64_t> distrib;
    public:
        StdRandomUniform64() {        
            distrib = std::uniform_int_distribution<int64_t>(0,1);
        }

        int64_t operator()() { return distrib(rng); }
        int64_t random() { return distrib(rng); }
        

        void process(size_t n, std::vector<int64_t> & r) {
            r.resize(n);
            for(size_t i = 0; i < n; i++) r[i] = random();
        }
    };

    template<typename T>
    struct StdVector
    {    
        std::vector<T> vector;

        StdVector() = default;
        StdVector(size_t n) { vector.resize(n); }
        StdVector(const std::initializer_list<T> & list) { std::copy(list.begin(), list.end(), vector.begin()); }
        StdVector(const std::vector<T> & v) { vector = v; }
        StdVector(const StdVector<T> & v) { vector = v.vector; } 


        StdVector<T>& operator = (const StdVector<T> & v) {
            vector = v.vector;
            return *this;
        }

        T& operator [](size_t i) { return vector[i]; }

        void copy(const StdVector<T> & v) {
            std::copy(v.vector.begin(),v.vector.end(),vector.begin());
        }

        StdVector<T>& operator + (const StdVector<T>& v) {
            std::copy(v.vector.begin(),v.vector.end(), vector.end());
            return *this;
        }

        StdVector<T>& operator << (const T & x) {
            vector.push_back(x);
            return *this;
        }
        StdVector<T>& operator >> (T & x) {
            x = vector.back();
            vector.pop_back();
            return *this;
        }

        T& front() { return vector.front(); }
        T& back()  { return vector.back();  }

        void push_back(const T & q) { vector.push_back(q); }
        T pop_back() {
            T x = vector.back();
            vector.pop_back();
            return x;
        }

        T& at(size_t i) { return vector.at(i); }
        T* data() { return vector.data(); }
        size_t size() const { return vector.size(); }
        bool empty() const { return vector.empty(); }
        void resize(size_t n) { vector.resize(n); }
        void clear() { vector.clear(); }
        void erase(size_t i) { vector.erase(vector.begin()+i); }
        void erase(size_t i, size_t n) { vector.erase(vector.begin()+i, vector.begin()+i+n); }
        void insert(size_t i, const T & x) { vector.insert(vector.begin()+i, x); }
        void insert(size_t i, size_t n, const T & x) { vector.insert(vector.begin()+i, n, x); }
        void emplace(size_t i, const T & x) { vector.emplace(vector.begin()+i, x); }
        void fill(const T & x) { for(size_t i = 0; i < vector.size(); i++) vector[i] = x; }        
        void shuffle() { std::random_shuffle(vector.begin(), vector.end()); }

    };

    template<typename T>
    void reverse(StdVector<T> & v) { std::reverse(v.vector.begin(), v.vector.end()); }
    template<typename T>
    void sort(StdVector<T> & v) { std::sort(v.vector.begin(), v.vector.end()); }
    template<typename T>
    void append(StdVector<T> & a, const StdVector<T> & v) { std::copy(v.vector.begin(), v.vector.end(), a.vector.begin()); }

    template<typename T>
    struct StdMatrix
    {
        std::vector<T> matrix;
        size_t M,N;

        StdMatrix() { M = N = 0; }
        StdMatrix(size_t i, size_t j) { matrix.resize(i*j); M = i; N = j;}
        StdMatrix(const StdMatrix<T> & m) { matrix = m.matrix; }

        T& operator()(size_t i, size_t j) { return matrix[i*N + j]; }
        T& get(size_t i, size_t j) { return matrix[i*N + j]; }
        void set(size_t i, size_t j, const T & v) { matrix[i*N + j] = v; }

        StdMatrix& operator = ( const StdMatrix<T> & m) { matrix = m.matrix; return *this; }

        void set_row(size_t row, const StdVector<T> & v) {
            for(size_t i = 0; i < N; i++)
                matrix[row*N + i] = v[i];
        }
        void set_col(size_t col, const StdVector<T> & v) {
            for(size_t i = 0; i < M; i++)
                matrix[i*N + col] = v[i];
        }
        StdVector<T> row(size_t row) {
            StdVector<T> v(N);
            for(size_t i = 0; i < N; i++)
                v[i] = matrix[row*N + i];
            return v;
        }
        StdVector<T> col(size_t col) {
            StdVector<T> v(M);
            for(size_t i = 0; i < M; i++)
                v[i] = matrix[i*N + col];
            return v;
        }

        void shuffle() { std::random_shuffle(matrix.begin(), matrix.end()); }
        T* data() { return matrix.data(); }
        size_t size() const { return matrix.size(); }
        size_t rows() const { return M; }
        size_t cols() const { return N; }
        void resize(size_t m, size_t n) {
            M = m;
            N = n;
            matrix.resize(M*N);
        }
    };

    struct StdBitVector
    {
        std::vector<bool> vec;

        StdBitVector() = default;
        StdBitVector(size_t i) { vec.resize(i); }

        bool operator[](size_t i) { return vec[i]; }

        void flip(size_t i) { vec[i] = !vec[i]; }
        void set(size_t i) { vec[i] = true; }
        bool get(size_t i) { return vec[i]; }

        size_t size() const { return vec.size(); }
        size_t bytes() const { return size()/8; }
        void ptr(void * ptr) {
            int8_t * p = (int8_t*)ptr;
            for(size_t i = 0; i < vec.size(); i++)
                if(vec[i/8]) p[i/8] |= 1 << i%8;
                else p[i/8] &= ~(1 << i%8);
        }
    };

    struct StdString
    {
        std::string str;

        StdString() = default;
        StdString(const char * s) { str = s; }
        StdString(const std::string &s) { str = s;}
        StdString(const StdString& s) { str = s.str; }

        char& operator[](size_t i) { return str[i]; }
        char operator[](size_t i) const { return str[i]; }
        const char* data() { return str.data(); }
        size_t size() const { return str.size(); }
        bool empty() { return str.empty(); }
        void clear() { str.clear(); }

        void insert(size_t i, char c) { str.insert(str.begin()+i, c); }
        void insert(size_t i, size_t n, char c) { str.insert(str.begin()+i, n, c); }
        void fill(size_t i, char c) { str.insert(0,i,c); }
        void insert(size_t i, const StdString& c) { str.insert(i, c.str); }
        
        char __getitem__(size_t i) { return str[i]; }
        void __setitem__(size_t i, char c) { str[i] = c; }

        void erase(size_t i, size_t n =1) { str.erase(i,n); }
        void push_back(size_t c) { str.push_back(c); }
        char pop_back() { char c = str.back(); str.pop_back(); return c; }
        void append(char c, size_t count=1) { str.append(count, c); }
        StdString replace(size_t pos, size_t count, const StdString &s) { return str.replace(pos,count,s.str); }
        void resize(size_t i) { str.resize(i); }
        void mutate() { size_t n = randint(0,size()); str[n] = randchar(); }
        void uniform_mutate(float p = 0.001) {
            for(size_t i = 0; i < size(); i++)
                if(flip(p)) str[i] = randchar();
        }
        void crossover(const StdString & mom, const StdString & dad) {
            assert(mom.size() == dad.size());
            resize(mom.size());
            for(size_t i = 0; i < mom.size(); i++)
                str[i] = flip()? mom[i]:dad[i];
        }
        std::string& string() { return str; }
        StdString substr(size_t i, size_t n) { return StdString(str.substr(i,n)); }

        StdString& operator = (const StdString & s) { str = s.str; return *this; }
        StdString& operator = (const std::string & s) { str = s; return *this; }
        StdString& operator = (const char * s) { str = s; return *this; }

        StdString& operator = (int64_t i) { str = std::to_string(i); return *this; }
        StdString& operator = (uint64_t i) { str = std::to_string(i); return *this; }
        StdString& operator = (float f) { str = std::to_string(f); return *this; }
        StdString& operator = (double f) { str = std::to_string(f); return *this; }

        StdString operator + (const StdString & s) { StdString r; r = str + s.str; return r; }
        StdString operator + (const std::string & s) { StdString r = str + s; return r; }

        StdString& operator += (const StdString & s) { str += s.str; return *this; }

        bool operator == (const StdString & b) { return str == b.str; }

        int32_t to_int32(int base =10) { return std::stoi(str.c_str(),nullptr,base); }
        int64_t to_int64(int base =10) { return std::stoll(str.c_str(),nullptr,base); }
        float   to_float() { return std::stof(str); }
        double  to_double() { return std::stod(str); }

        void reverse() { std::reverse(str.begin(),str.end()); }
        void sort() { std::sort(str.begin(),str.end()); }
        void shuffle() { std::random_shuffle(str.begin(),str.end()); }

        void getline() {
            std::getline(std::cin, str);
        }

        size_t find(const StdString &s, size_t pos=0) { return str.find(s.str,pos); }
        size_t rfind(const StdString &s, size_t pos=0) { return str.rfind(s.str,pos); }
        int compare(const StdString & b) { return str.compare(b.str); }

        const char* __str__() { return str.c_str(); }
    };

    inline bool operator == (const StdString &a, const StdString &b) {
        return a.str == b.str;
    }
    inline bool operator < (const StdString &a, const StdString & b) {
        return a.str < b.str;
    }

    struct StdStringMap
    {
        std::map<StdString,StdString> map;

        StdStringMap() = default;

        StdString& operator[](const StdString& key) { return map[key]; }

        StdStringMap& operator = (const StdStringMap& m) { map = m.map; return *this; }

        bool key_in(const StdString& key) {
            return map.find(key) != map.end();
        }
        StdString& at(StdString & key) { return map.at(key); }
        bool empty() { return map.empty(); }
        size_t size() const { return map.size(); }
        void clear() { map.clear(); }
        void insert(const StdString & key, const std::string& value) { map.insert({key,value}); }
        void erase(const StdString & key) { map.erase(key); }
        
    };    

    template<typename T>
    struct StdArray
    {

        std::shared_ptr<T>  array;
        size_t M;

        StdArray() { array = NULL; }
        StdArray(size_t x) { 
            M = x;
            T * _array = new T[M];
            assert(_array != nullptr);
            memset(_array,0x00,M*sizeof(T));
            array = std::shared_ptr<T>( _array, [](T * ptr){ delete ptr; });
        }
        StdArray(size_t n, T * x) {
            M = n;
            T * _array = new T[M];
            assert(_array != nullptr);
            memcpy(_array,x,n*sizeof(T));
            array = std::shared_ptr<T>( _array, [](T * ptr){ delete ptr; });
        }
        StdArray(const StdArray<T> & a) {
            M = a.M;
            array = a.array;            
        }
        ~StdArray()  = default;

        StdArray& operator = (const StdArray<T> & a) {
            array.reset();
            M = a.M;
            array = a.array;
            return *this;
        }

        T& operator[](size_t i) { return array[i-1]; }
        T  __getitem__(size_t i) { return array.get()[i-1]; }
        void __setitem(size_t i, T val) { array.get()[i-1] = val; }

        void resize(size_t n) {            
            array.reset();
            M = n;
            T * _array = new T[M];
            assert(_array != nullptr);    
            memset(_array,0x00,M*sizeof(T));
            array = std::shared_ptr<T>( _array, [](T * ptr){ delete ptr; });
        }

        size_t size() const { return M; }  

        void print() { 
            std::cout << "Array[" << M << "] = ";
            for(size_t i = 0; i < M-1; i++)
                std::cout << array.get()[i] << ",";
            std::cout << array.get()[M-1] << std::endl;
        }      
    };

    template<typename T>
    void reverse(StdArray<T> & a) { std::reverse(a.array[0],a.array[a.size()-1]); }
    template<typename T>
    void sort(StdArray<T> & a) { std::sort(a.array[0], a.array(a.size()-1)); }
    

}