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
#include <functional>
#include <cmath>
#include <cstdlib>

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

        float   rand() { return randfloat(0,1); }
        int     randi(int min, int max) { return randint(min,max); }
        int64_t randi64(int64_t min, int64_t max) { return randint(min,max); }
        float   randf32(float min, float max) { return randfloat(min,max);}
        double  randf64(double min, double max) { return randdbl(min,max); }
        bool    flip(double p =0.5) { return flip(p) ;}
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


    /*
    template <typename T, std::size_t Alignment>
    class aligned_allocator
    {
        public:
    
            // The following will be the same for virtually all allocators.
            typedef T * pointer;
            typedef const T * const_pointer;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T value_type;
            typedef std::size_t size_type;
            typedef ptrdiff_t difference_type;
    
            T * address(T& r) const
            {
                return &r;
            }
    
            const T * address(const T& s) const
            {
                return &s;
            }
    
            std::size_t max_size() const
            {
                // The following has been carefully written to be independent of
                // the definition of size_t and to avoid signed/unsigned warnings.
                return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) / sizeof(T);
            }
    
    
            // The following must be the same for all allocators.
            template <typename U>
            struct rebind
            {
                typedef aligned_allocator<U, Alignment> other;
            };
    
            bool operator !=(const aligned_allocator& other) const
            {
                return !(*this == other);
            }
    
            void construct(T * const p, const T& t) const
            {
                void * const pv = static_cast<void *>(p);
    
                new (pv) T(t);
            }
    
            void destroy(T * const p) const
            {
                p->~T();
            }
    
            // Returns true if and only if storage allocated from *this
            // can be deallocated from other, and vice versa.
            // Always returns true for stateless allocators.
            bool operator==(const aligned_allocator& other) const
            {
                return true;
            }
    
    
            // Default constructor, copy constructor, rebinding constructor, and destructor.
            // Empty for stateless allocators.
            aligned_allocator() { }
    
            aligned_allocator(const aligned_allocator&) { }
    
            template <typename U> aligned_allocator(const aligned_allocator<U, Alignment>&) { }
    
            ~aligned_allocator() { }
    
    
            // The following will be different for each allocator.
            T * allocate(const std::size_t n) const
            {
                // The return value of allocate(0) is unspecified.
                // Mallocator returns NULL in order to avoid depending
                // on malloc(0)'s implementation-defined behavior
                // (the implementation can define malloc(0) to return NULL,
                // in which case the bad_alloc check below would fire).
                // All allocators can return NULL in this case.
                if (n == 0) {
                    return NULL;
                }
    
                // All allocators should contain an integer overflow check.
                // The Standardization Committee recommends that std::length_error
                // be thrown in the case of integer overflow.
                if (n > max_size())
                {
                    throw std::length_error("aligned_allocator<T>::allocate() - Integer overflow.");
                }
    
                // Mallocator wraps malloc().
                void * const pv = std::aligned_alloc(n * sizeof(T), Alignment);
    
                // Allocators should throw std::bad_alloc in the case of memory allocation failure.
                if (pv == NULL)
                {
                    throw std::bad_alloc();
                }
    
                return static_cast<T *>(pv);
            }
    
            void deallocate(T * const p, const std::size_t n) const
            {
                std::free(p);
            }
    
    
            // The following will be the same for all allocators that ignore hints.
            template <typename U>
            T * allocate(const std::size_t n, const U *) const
            {
                return allocate(n);
            }
    
    
            // Allocators are not required to be assignable, so
            // all allocators should have a private unimplemented
            // assignment operator. Note that this will trigger the
            // off-by-default (enabled under /Wall) warning C4626
            // "assignment operator could not be generated because a
            // base class assignment operator is inaccessible" within
            // the STL headers, but that warning is useless.
        private:
            aligned_allocator& operator=(const aligned_allocator&);
    };
    */

    template<typename T>
    struct StdVector
    {
        //std::vector<T, aligned_allocator<T,sizeof(T)>> vector;
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
        const T& operator [](size_t i) const { return vector[i]; }

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

        void reverse() { std::reverse(vector.begin(), vector.end()); }
        void sort() { std::sort(vector.begin(), vector.end()); }
        void append(const StdVector<T> & v) { std::copy(v.vector.begin(), v.vector.end(), vector.begin()); }
        void shuffle() { std::random_shuffle(vector.begin(), vector.end()); }

    };

    template<typename T>
    struct StdMatrix
    {
        StdVector<T> matrix;
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

        void random_shuffle() { std::random_shuffle(matrix.vector.begin(), matrix.vector.end()); }

        T* data() { return matrix.vector.data(); }
        size_t size() const { return matrix.vector.size(); }
        size_t rows() const { return M; }
        size_t cols() const { return N; }
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
        const char* data() const { return str.data(); }
        size_t size() const { return str.size(); }
        bool empty() { return str.empty(); }
        void clear() { str.clear(); }

        void insert(size_t i, char c) { str.insert(str.begin()+i, c); }
        void insert(size_t i, size_t n, char c) { str.insert(str.begin()+i, n, c); }
        void fill(size_t i, char c) { str.insert(0,i,c); }
        void insert(size_t i, const StdString& c) { str.insert(i, c.str); }
                
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

        bool contains(const StdString& key) {
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
    StdVector<T> fill(size_t n, T val) {
        StdVector<T> r(n);
        for(size_t i = i; i < n; i++) r[i] = val;
        return r;
    }
    template<typename T>
    StdVector<T> zero(size_t n) {
        return fill(n,(T)0);
    }
    template<typename T>
    StdVector<T> random_vector(size_t n, T min = (T)0, T max = (T)1) {
        StdVector<T> r(n);
        StdRandomUniform rnd;
        for(size_t i = 0; i < n; i++) r[i] = rnd.random(min,max);
        return r;
    }

    template<typename T>
    StdVector<T> operator + (const StdVector<T> & a, const StdVector<T> & b) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = a[i] + b[i];
        return r;
    }
    template<typename T>
    StdVector<T> operator - (const StdVector<T> & a, const StdVector<T> & b) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = a[i] - b[i];
        return r;
    }
    template<typename T>
    StdVector<T> operator * (const StdVector<T> & a, const StdVector<T> & b) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = a[i] * b[i];
        return r;
    }
    template<typename T>
    StdVector<T> operator / (const StdVector<T> & a, const StdVector<T> & b) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = a[i] / b[i];
        return r;
    }
    template<typename T>
    StdVector<T> operator % (const StdVector<T> & a, const StdVector<T> & b) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = a[i] % b[i];
        return r;
    }
    template<typename T>
    StdVector<T> operator - (const StdVector<T> & a) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = -a[i];
        return r;
    }

    template<typename T>
    void println(StdVector<T> & x) {
        std::cout << "Vector[" << x.size() << "] = ";
        for(size_t i = 0; i < x.size()-1; i++)
            std::cout << x[i] << ",";
        std::cout << x.back() << std::endl;
    }
    
    template<typename T>
    StdVector<T> foreach(const StdVector<T> & a, T (*func)(T)) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = func(a[i]);
        return r;
    }
    template<typename T>
    StdVector<T> foreach(const StdVector<T> & a, const StdVector<T> & b, T (*func)(T,T)) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = func(a[i],b[i]);
        return r;
    }
    template<typename T>
    StdVector<T> foreach(const StdVector<T> & a, const T & b, T (*func)(T,T)) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = func(a[i],b);
        return r;
    }
    template<typename T1, typename T2>
    StdVector<T1> foreach(const StdVector<T2> & a, T1 (*func)(T2)) {
        StdVector<T1> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = func(a[i]);
        return r;
    }
    template<typename T>
    StdVector<T> foreach(const StdVector<T> & a, const StdVector<T> & b, const StdVector<T> & c, T (*func)(T,T,T)) {
        StdVector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = func(a[i],b[i],c[i]);
        return r;
    }
    
    template<typename T>
    StdVector<T> abs(const StdVector<T> & a) {        
        return foreach<T>(a, std::abs);
    }
    template<typename T>
    StdVector<T> fabs(const StdVector<T> & a) {        
        return foreach<T>(a, std::fabs);
    }
    template<typename T>
    StdVector<std::div_t> div(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<std::div_t,T>(a,b,std::div);
    }
    template<typename T>
    StdVector<std::div_t> div(const StdVector<T> & a, const T & b) {        
        return foreach<std::div_t,T>(a,b,std::div);
    }
    template<typename T>
    StdVector<T> fmod(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::fmod);
    }
    template<typename T>
    StdVector<T> fmod(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::fmod);
    }
    template<typename T>
    StdVector<T> remainder(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::remainder);
    }
    template<typename T>
    StdVector<T> remainder(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::remainder);
    }
    template<typename T>
    StdVector<T> remquo(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::remquo);
    }
    template<typename T>
    StdVector<T> remquo(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::remquo);
    }
    template<typename T>
    StdVector<T> fmod(const StdVector<T> & a, const StdVector<T> & b, const StdVector<T> & c) {        
        return foreach<T>(a,b,std::fmod);
    }
    template<typename T>
    StdVector<T> fmax(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::fmax);
    }
    template<typename T>
    StdVector<T> fmax(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::fmax);
    }
    template<typename T>
    StdVector<T> fmin(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::fmin);
    }
    template<typename T>
    StdVector<T> fmin(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::fmin);
    }
    template<typename T>
    StdVector<T> fdim(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::fdim);
    }
    template<typename T>
    StdVector<T> fdim(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::fdim);
    }
    template<typename T>
    StdVector<T> exp(const StdVector<T> & a) {        
        return foreach<T>(a, std::exp);
    }
    template<typename T>
    StdVector<T> exp2(const StdVector<T> & a) {        
        return foreach<T>(a, std::exp2);
    }
    template<typename T>
    StdVector<T> expm1(const StdVector<T> & a) {        
        return foreach<T>(a, std::expm1);
    }
    template<typename T>
    StdVector<T> log(const StdVector<T> & a) {        
        return foreach<T>(a, std::log);
    }
    template<typename T>
    StdVector<T> log10(const StdVector<T> & a) {        
        return foreach<T>(a, std::log10);
    }
    template<typename T>
    StdVector<T> log2(const StdVector<T> & a) {        
        return foreach<T>(a, std::log2);
    }
    template<typename T>
    StdVector<T> log1p(const StdVector<T> & a) {        
        return foreach<T>(a, std::log1p);
    }
    template<typename T>
    StdVector<T> pow(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::pow);
    }
    template<typename T>
    StdVector<T> pow(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::pow);
    }
    template<typename T>
    StdVector<T> sqrt(const StdVector<T> & a) {        
        return foreach<T>(a, std::sqrt);
    }
    template<typename T>
    StdVector<T> cbrt(const StdVector<T> & a) {        
        return foreach<T>(a, std::cbrt);
    }
    template<typename T>
    StdVector<T> hypot(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::hypot);
    }
    template<typename T>
    StdVector<T> sin(const StdVector<T> & a) {        
        return foreach<T>(a, std::sin);
    }
    template<typename T>
    StdVector<T> cos(const StdVector<T> & a) {        
        return foreach<T>(a, std::cos);
    }
    template<typename T>
    StdVector<T> tan(const StdVector<T> & a) {        
        return foreach<T>(a, std::tan);
    }
    template<typename T>
    StdVector<T> asin(const StdVector<T> & a) {        
        return foreach<T>(a, std::asin);
    }
    template<typename T>
    StdVector<T> acos(const StdVector<T> & a) {        
        return foreach<T>(a, std::acos);
    }
    template<typename T>
    StdVector<T> atan(const StdVector<T> & a) {        
        return foreach<T>(a, std::atan);
    }
    template<typename T>
    StdVector<T> atan2(const StdVector<T> & a, const StdVector<T> & b) {        
        return foreach<T>(a,b,std::atan2);
    }
    template<typename T>
    StdVector<T> atan2(const StdVector<T> & a, const T & b) {        
        return foreach<T>(a,b,std::atan2);
    }
    template<typename T>
    StdVector<T> sinh(const StdVector<T> & a) {        
        return foreach<T>(a, std::sinh);
    }
    template<typename T>
    StdVector<T> cosh(const StdVector<T> & a) {        
        return foreach<T>(a, std::cosh);
    }
    template<typename T>
    StdVector<T> tanh(const StdVector<T> & a) {        
        return foreach<T>(a, std::tanh);
    }
    template<typename T>
    StdVector<T> asinh(const StdVector<T> & a) {        
        return foreach<T>(a, std::asinh);
    }
    template<typename T>
    StdVector<T> acosh(const StdVector<T> & a) {        
        return foreach<T>(a, std::acosh);
    }
    template<typename T>
    StdVector<T> atanh(const StdVector<T> & a) {        
        return foreach<T>(a, std::atanh);
    }
    template<typename T>
    StdVector<T> erf(const StdVector<T> & a) {        
        return foreach<T>(a, std::erf);
    }
    template<typename T>
    StdVector<T> erfc(const StdVector<T> & a) {        
        return foreach<T>(a, std::erfc);
    }
    template<typename T>
    StdVector<T> tgamma(const StdVector<T> & a) {        
        return foreach<T>(a, std::tgamma);
    }
    template<typename T>
    StdVector<T> lgamma(const StdVector<T> & a) {        
        return foreach<T>(a, std::lgamma);
    }
    template<typename T>
    StdVector<T> ceil(const StdVector<T> & a) {        
        return foreach<T>(a, std::ceil);
    }
    template<typename T>
    StdVector<T> floor(const StdVector<T> & a) {        
        return foreach<T>(a, std::floor);
    }
    template<typename T>
    StdVector<T> trunc(const StdVector<T> & a) {        
        return foreach<T>(a, std::trunc);
    }
    template<typename T>
    StdVector<T> round(const StdVector<T> & a) {        
        return foreach<T>(a, std::round);
    }
    template<typename T>
    StdVector<T> nearbyint(const StdVector<T> & a) {        
        return foreach<T>(a, std::nearbyint);
    }
    template<typename T>
    StdVector<T> rint(const StdVector<T> & a) {        
        return foreach<T>(a, std::rint);
    }
    template<typename T>
    StdVector<long> lrint(const StdVector<T> & a) {        
        return foreach<long,T>(a, std::lrint);
    }
    template<typename T>
    StdVector<long long> llrint(const StdVector<T> & a) {        
        return foreach<long long,T>(a, std::llrint);
    }

    template<typename T>
    bool find(const StdVector<T> & v, T val) {
        return std::find(v.vector.begin(),v.vector.end(), val) != v.end();
    }
    template<typename T>
    bool find_end(const StdVector<T> & v, T val) {
        return std::find_end(v.vector.begin(),v.vector.end(), val) != v.end();
    }
    template<typename T>
    bool find(const StdVector<T> & v, const StdVector<T> & val) {
        return std::find(v.vector.begin(),v.vector.end(), val.vector.begin(),val.vector.end()) != v.end();
    }
    template<typename T>
    bool find_end(const StdVector<T> & v, const StdVector<T> & val) {
        return std::find_end(v.vector.begin(),v.vector.end(), val.vector.begin(), val.vector.end()) != v.end();
    }

    template<typename T>
    T min(const StdVector<T> & x) {
        return *std::min_element(x.vector.begin(),x.vector.end());
    }
    template<typename T>
    T max(const StdVector<T> & x) {
        return *std::max_element(x.vector.begin(),x.vector.end());
    }
    template<typename T>
    size_t min_index(const StdVector<T> & x) {
        return std::distance(x.vector.begin(), std::min_element(x.vector.begin(),x.vector.end()));
    }
    template<typename T>
    size_t max_index(const StdVector<T> & x) {
        return std::distance(x.vector.begin(), std::max_element(x.vector.begin(),x.vector.end()));
    }

    template<typename T>
    void sort(StdVector<T> & x) {
        std::sort(x.vector.begin(),x.vector.end());
    }

    template<typename T>
    bool search(const StdVector<T> & a, const StdVector<T> & b) {
        return std::search(a.vector.begin(),a.vector.end(),b.vector.begin(),b.vector.end()) != a.end();
    }

    template<typename T>
    void copy(const StdVector<T> & src, StdVector<T> & dst) {
        dst.resize(src.size());
        std::copy(src.vector.begin(),src.vector.end(),dst.vector.begin());
    }
    template<typename T>
    void copy_n(const StdVector<T> & src, size_t n, StdVector<T> & dst) {
        dst.resize(n);
        std::copy_n(src.vector.begin(),src.vector.end(),n,dst.vector.begin());
    }

    template<typename T>
    size_t size(const StdVector<T> & x) {
        return x.size();
    }

    template<typename T>
    const int count(const StdVector<T> & x, T val) {
        return std::count(x.vector.begin(),x.vector.end(),val);
    }

    template <class T>
    bool consecutive_values(const StdVector<T>& c, size_t count, const T& v)
    {
        return std::search_n(std::begin(c.vector),std::end(c.vector),count,v) != std::end(c);
    }
}