#pragma once

template<typename T>
    struct ArrayVector
    {        
        std::valarray<T> vector;
        size_t channels;

        ArrayVector() {
            channels = 1;
        }
        ArrayVector(size_t size, size_t channels = 1) {
            vector.resize(size * channels);
            this->channels = channels;
        }
        ArrayVector<T>(const std::vector<T> & s, size_t chans = 1) {
            vector = s;
            channels = chans;
        }
        ArrayVector<T>(const ArrayVector<T> & s) {
            vector = s.vector;
            channels = s.channels;
        }

        size_t size() const { return vector.size(); }
        size_t num_channels() const { return channels; } 

        ArrayVector<T>& operator = (const ArrayVector<T> & v) {
            vector = v.vector;
            channels  = v.channels;
            return *this;
        }
        T& operator()(size_t i, size_t ch) {
            return vector[i*channels + ch];
        }

        ArrayVector<T> get_channel(size_t channel) {
            //ArrayVector<T> r(*this);
            //if(channels == 1) return r;
            ArrayVector<T> r(*this);
            r.vector = vector[std::slice(0,vector.size(),channel-1)];
            return r;
        }
        void set_channel(const ArrayVector<T> & v, size_t ch) {
            //size_t x = 0;
            //for(size_t i = ch; i < vector.size(); i+=channels) {
            //    vector[i] = v[x++];
            //}
            std::valarray<std::size_t> idx;
            idx.resize(size()/num_channels);
            size_t x = 0;
            for(size_t i = ch-1; i < size(); i+=channels) idx[x++] = i;
            vector[idx] = v.vector;
        }

        bool operator == (const ArrayVector<T> & s) {
            return s.channels == channels && vector.size() == s.vector.size();
        }

        ArrayVector<T> operator + (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector += s.vector;
            return r;
        }
        ArrayVector<T> operator - (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector -= s.vector;
            return r;
        }
        ArrayVector<T> operator * (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector *= s.vector;
            return r;
        }
        ArrayVector<T> operator / (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector /= s.vector;
            return r;
        }
        ArrayVector<T> operator % (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector %= s.vector;
            return r;
        }

        ArrayVector<T> operator + (const T s) {    
            ArrayVector<T> r(*this);
            r.vector += s;
            return r;
        }
        ArrayVector<T> operator - (const T s) {        
            ArrayVector<T> r(*this);
            r.vector -= s;
            return r;
        }
        ArrayVector<T> operator / (const T s) {        
            ArrayVector<T> r(*this);
            r.vector /= s;
            return r;
        }
        ArrayVector<T> operator * (const T s) {        
            ArrayVector<T> r(*this);
            r.vector *= s;
            return r;
        }
        ArrayVector<T> operator % (const T s) {        
            ArrayVector<T> r(*this);
            r.vector %= s;
            return r;
        }


        ArrayVector<T>& operator += (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector += s.vector;
            return *this;
        }
        ArrayVector<T>& operator -= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector -= s.vector;
            return *this;
        }
        ArrayVector<T>& operator *= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector *= s.vector;
            return *this;
        }
        ArrayVector<T>& operator /= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector /= s.vector;
            return *this;
        }
        ArrayVector<T>& operator %= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector %= s.vector;
            return *this;
        }

        ArrayVector<T> operator += (const T s) {
            vector += s;
            return *this;
        }
        ArrayVector<T> operator -= (const T s) {
            vector -= s;
            return *this;
        }
        ArrayVector<T> operator /= (const T s) {
            vector /= s;
            return *this;
        }
        ArrayVector<T> operator *= (const T s) {
            vector *= s;
            return *this;
        }
        ArrayVector<T> operator %= (const T s) {        
            vector %= s;
            return *this;
        }

        T sum() { return vector.sum(); }
        T min() { return vector.min(); }
        T max() { return vector.max(); }

        void resize(size_t s, size_t c) {
            channels = c;
            vector.resize(s * c);
        }

        ArrayVector<T> abs() { 
            ArrayVector<T> r(*this);
            r.vector = std::abs(vector);
            return r;
        }
        ArrayVector<T> exp() { 
            ArrayVector<T> r(*this);
            r.vector = std::exp(vector);
            return r;
        }
        ArrayVector<T> log() { 
            ArrayVector<T> r(*this);
            r.vector = std::log(vector);
            return r;
        }
        ArrayVector<T> log10() { 
            ArrayVector<T> r(*this);
            r.vector = std::log10(vector);
            return r;
        }
        ArrayVector<T> pow(const ArrayVector<T> & s) { 
            ArrayVector<T> r(*this);
            r.vector = std::pow(vector,s.vector);
            return r;
        }
        ArrayVector<T> pow(const T s) { 
            ArrayVector<T> r(*this);
            r.vector = std::pow(vector,s);        
            return r;        
        }
        ArrayVector<T> sqrt() {
            ArrayVector<T> r(*this);
            r.vector = std::sqrt(vector);
            return r;
        }
        ArrayVector<T> sin() {
            ArrayVector<T> r(*this);
            r.vector = std::sin(vector);
            return r;
        }
        ArrayVector<T> cos() {
            ArrayVector<T> r(*this);
            r.vector = std::cos(vector);
            return r;
        }
        ArrayVector<T> tan() {
            ArrayVector<T> r(*this);
            r.vector = std::tan(vector);
            return r;
        }
        ArrayVector<T> asin() {
            ArrayVector<T> r(*this);
            r.vector = std::asin(vector);
            return r;
        }
        ArrayVector<T> acos() {
            ArrayVector<T> r(*this);
            r.vector = std::acos(vector);
            return r;
        }
        ArrayVector<T> atan() {
            ArrayVector<T> r(*this);
            r.vector = std::atan(vector);
            return r;
        }
        ArrayVector<T> atan2(const T s) {
            ArrayVector<T> r(*this);
            r.vector = std::atan2(vector,s);
            return r;
        }
        ArrayVector<T> sinh() {
            ArrayVector<T> r(*this);
            r.vector = std::sinh(vector);
            return r;
        }
        ArrayVector<T> cosh() {
            ArrayVector<T> r(*this);
            r.vector = std::cosh(vector);
            return r;
        }
        ArrayVector<T> tanh() {
            ArrayVector<T> r(*this);
            r.vector = std::tanh(vector);
            return r;
        }
    };

    template<typename T> ArrayVector<T> abs(ArrayVector<T> & a) { return a.abs(); }
    template<typename T> ArrayVector<T> exp(ArrayVector<T> & a) { return a.exp(); }           
    template<typename T> ArrayVector<T> log(ArrayVector<T> & a) { return a.log(); } 
    template<typename T> ArrayVector<T> log10(ArrayVector<T> & a) { return a.log10(); }
    template<typename T> ArrayVector<T> pow(ArrayVector<T> & a, const ArrayVector<T> & s) { return a.pow(s); }
    template<typename T> ArrayVector<T> pow(ArrayVector<T> & a, const T s) { return a.pow(s); }
    template<typename T> ArrayVector<T> sqrt(ArrayVector<T> & a) { return a.sqrt(); }
    template<typename T> ArrayVector<T> sin(ArrayVector<T> & a) { return a.sin(); }
    template<typename T> ArrayVector<T> cos(ArrayVector<T> & a) { return a.cos(); }
    template<typename T> ArrayVector<T> tan(ArrayVector<T> & a) { return a.tan(); }
    template<typename T> ArrayVector<T> asin(ArrayVector<T> & a) { return a.asin(); }
    template<typename T> ArrayVector<T> acos(ArrayVector<T> & a) { return a.acos(); }
    template<typename T> ArrayVector<T> atan(ArrayVector<T> & a) { return a.atan(); }
    template<typename T> ArrayVector<T> atan2(ArrayVector<T> & a, const T s) { return a.atan2(s); }
    template<typename T> ArrayVector<T> sinh(ArrayVector<T> & a) { return a.sinh(); }
    template<typename T> ArrayVector<T> cosh(ArrayVector<T> & a) { return a.cosh(); }
    template<typename T> ArrayVector<T> tanh(ArrayVector<T> & a) { return a.tanh(); }
