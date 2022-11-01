#pragma once
#include <Eigen/Eigen>

namespace SimpleEigen {
typedef float SampleType;

struct SampleVector
{
    // this will be replaced with eigen (or arrayfire, or another backend) later.
    Eigen::ArrayXf vector;
    size_t channels;

    SampleVector() {
        channels = 1;
    }
    SampleVector(size_t size, size_t channels = 1) {
        vector.resize(size * channels);
        this->channels = channels;
    }
    SampleVector(const std::vector<SampleType> & s, size_t chans = 1) {
        vector.resize(s.size()*chans);
        memcpy(vector.data(),s.data(),s.size()*chans*sizeof(SampleType));
        channels = chans;
    }
    SampleVector(std::initializer_list<SampleType> & l, size_t chans = 1)  {        
        std::vector<SampleType> s(l.begin(),l.end());
        vector.resize(s.size()*chans);
        channels = chans;
        memcpy(vector.data(),s.data(),s.size()*chans*sizeof(SampleType));
    }
    SampleVector(const SampleVector & s) {
        vector = s.vector;
        channels = s.channels;
    }    
    SampleVector(size_t channels, const Eigen::ArrayXf& a) {
        vector = a;
        this->channels = channels;
    }

    size_t size() const { return vector.size(); }
    size_t num_channels() const { return channels; } 
    void resize(size_t s, size_t c=-1) {
        if(c != -1) {
            channels = c;
            vector.resize(s * c);
        }
        else {
            vector.resize(s);        
        }
    }

    SampleVector& operator = (const SampleVector & v) {
        vector = v.vector;
        channels  = v.channels;
        return *this;
    }    
    SampleType& operator()(size_t i, size_t ch) {
        return vector[i*channels + ch];
    }

    SampleVector get_channel(size_t channel) {        
        SampleVector r(size()/channels);
        r.vector = vector(Eigen::seq(channel,vector.size(),channels));
        return r;
    }
    SampleVector get_channel(size_t channel) const {        
        SampleVector r(size()/channels);
        r.vector = vector(Eigen::seq(channel,vector.size(),channels));
        return r;
    }
    void set_channel(const SampleVector & v, size_t ch) {
        size_t x = 0;
        for(size_t i = ch; i < vector.size(); i+=channels) {
            vector[i] = v.vector[x++];
        }        
    }

    bool operator == (const SampleVector & s) {
        return s.channels == channels && vector.size() == s.vector.size();
    }

    SampleType* data() { return vector.data(); }

    SampleType sum() { return vector.sum(); }
    SampleType min() { return vector.minCoeff(); }
    SampleType max() { return vector.maxCoeff(); }
    
    void set_channels(size_t c) { vector.resize(vector.size()/channels*c); channels = c; }
    void fill(SampleType x) { vector.fill(x); }
    void zeros() { vector.Zero(); }
    void ones()  { vector.Ones();  }
    void random() { vector.Random(); }

    void copy(const SampleVector & in, size_t pos, size_t n) {
        vector.resize((pos+n));
        memcpy(vector.data(),in.vector.data()+pos,n*sizeof(SampleType));
    }
    SampleVector slice(size_t i, size_t n) {
        SampleVector r(n);
        memcpy(r.data(), vector.data()+i, n * sizeof(SampleType));
        return r;
    }
    SampleVector stride_slice(const SampleVector & in, size_t stride, size_t pos, size_t n)
    {
        SampleVector r(n/stride);
        size_t x = pos;
        for(size_t i = 0; i < n; i++) {
            r.vector[i] = vector[x];
            x += stride;
        } 
        return r;
    }
    SampleVector operator + (const SampleVector &s) {
        assert(*this == s);
        SampleVector r(*this);
        r.vector += s.vector;
        return r;
    }
    SampleVector operator - (const SampleVector &s) {
        assert(*this == s);
        SampleVector r(*this);
        r.vector -= s.vector;
        return r;
    }
    SampleVector operator * (const SampleVector &s) {
        assert(*this == s);
        SampleVector r(*this);
        r.vector *= s.vector;
        return r;
    }
    SampleVector operator / (const SampleVector &s) {
        assert(*this == s);
        SampleVector r(*this);
        r.vector /= s.vector;
        return r;
    }
    
    SampleVector operator + (const SampleType s) {    
        SampleVector r(*this);
        r.vector += s;
        return r;
    }
    SampleVector operator - (const SampleType s) {        
        SampleVector r(*this);
        r.vector -= s;
        return r;
    }
    SampleVector operator / (const SampleType s) {        
        SampleVector r(*this);
        r.vector /= s;
        return r;
    }
    SampleVector operator * (const SampleType s) {        
        SampleVector r(*this);
        r.vector *= s;
        return r;
    }
    

    SampleVector& operator += (const SampleVector& s) {        
        assert(*this == s);
        vector += s.vector;
        return *this;
    }
    SampleVector& operator -= (const SampleVector& s) {        
        assert(*this == s);
        vector -= s.vector;
        return *this;
    }
    SampleVector& operator *= (const SampleVector& s) {        
        assert(*this == s);
        vector *= s.vector;
        return *this;
    }
    SampleVector& operator /= (const SampleVector& s) {        
        assert(*this == s);
        vector /= s.vector;
        return *this;
    }
    

    SampleVector operator += (const SampleType s) {
        vector += s;
        return *this;
    }
    SampleVector operator -= (const SampleType s) {
        vector -= s;
        return *this;
    }
    SampleVector operator /= (const SampleType s) {
        vector /= s;
        return *this;
    }
    SampleVector operator *= (const SampleType s) {
        vector *= s;
        return *this;
    }
    

    

    
    SampleVector abs() const { 
        SampleVector r(*this);
        r.vector = vector.abs();
        return r;
    }
    SampleVector abs2() const { 
        SampleVector r(*this);
        r.vector = vector.abs2();
        return r;
    }
    SampleVector inverse() const { 
        SampleVector r(*this);
        r.vector = vector.inverse();
        return r;
    }
    SampleVector exp() const { 
        SampleVector r(*this);
        r.vector = vector.exp();
        return r;
    }
    SampleVector log() const { 
        SampleVector r(*this);
        r.vector = vector.log();
        return r;
    }
    SampleVector log1p() const { 
        SampleVector r(*this);
        r.vector = vector.log1p();
        return r;
    }
    SampleVector log10() const  { 
        SampleVector r(*this);
        r.vector = vector.log10();
        return r;
    }
    SampleVector pow(const SampleVector & s) const { 
        SampleVector r(*this);
        r.vector = vector.pow(s.vector);
        return r;
    }
    SampleVector pow(const SampleType s) const { 
        SampleVector r(*this);
        r.vector = vector.pow(s);        
        return r;        
    }
    SampleVector sqrt() const {
        SampleVector r(*this);
        r.vector = vector.sqrt();
        return r;
    }
    SampleVector rsqrt() const {
        SampleVector r(*this);
        r.vector = vector.rsqrt();
        return r;
    }
    SampleVector square() const {
        SampleVector r(*this);
        r.vector = vector.square();
        return r;
    }
    SampleVector sin() const  {
        SampleVector r(*this);
        r.vector = vector.sin();
        return r;
    }
    SampleVector cos() const {
        SampleVector r(*this);
        r.vector = vector.cos();
        return r;
    }
    SampleVector tan() const {
        SampleVector r(*this);
        r.vector = vector.tan();
        return r;
    }
    SampleVector asin() const {
        SampleVector r(*this);
        r.vector = vector.asin();
        return r;
    }
    SampleVector acos() const {
        SampleVector r(*this);
        r.vector = vector.acos();
        return r;
    }
    SampleVector atan() const {
        SampleVector r(*this);
        r.vector = vector.atan();
        return r;
    }    
    SampleVector sinh() const {
        SampleVector r(*this);
        r.vector = vector.sinh();
        return r;
    }
    SampleVector cosh() const {
        SampleVector r(*this);
        r.vector = vector.cosh();
        return r;
    }
    SampleVector tanh() const {
        SampleVector r(*this);
        r.vector = vector.tanh();
        return r;
    }
    SampleVector ceil() const { 
        SampleVector r(*this);
        r.vector = vector.ceil();
        return r;        
    }
    SampleVector floor() const { 
        SampleVector r(*this);
        r.vector = vector.floor();
        return r;
    }
    SampleVector round() const { 
        SampleVector r(*this);
        r.vector = vector.round();
        return r;
    }
};

struct SampleMatrix {
    Eigen::ArrayXXf channels;
    SampleMatrix(size_t chan, size_t samps) {
        channels.resize(chan,samps);
    }
    SampleMatrix(const SampleVector & v) {
        channels.resize(v.num_channels(), v.size()/v.num_channels());
        for(size_t i = 0; i < v.num_channels(); i++)             
            channels.row(i) = v.get_channel(i).vector;
    }
    SampleMatrix(const Eigen::ArrayXXf & c) {
        channels = c;
    }

    SampleType * data() { return channels.data(); }
    void interleave(const SampleVector & s) {
        channels.resize(s.num_channels());
        for(size_t i = 0; i < s.num_channels(); i++) {
            channels.row(i) = s.get_channel(i).vector;
        }
    }    
    SampleVector deinterleave() { 
        SampleVector r;
        r.resize(channels.rows() * channels.cols());
        for(size_t i = 0; i < channels.rows(); i++)            
        {
            SampleVector tmp(1,channels(i));            
            r.set_channel(tmp,i);
        }
        return r;
    }

    size_t num_channels() const { return channels.rows(); }
    size_t cols() const { return channels.cols(); }
    
    SampleVector operator[](size_t ch) { 
        SampleVector tmp(1,channels(ch));
        return tmp;
     }
    SampleVector get_channel(size_t c) { 
        SampleVector tmp(1,channels(c));
        return tmp;
    }
    void set_channel(size_t c, const SampleVector & s) 
    { 
        channels.row(c) = s.vector;
    }

    SampleMatrix& operator = (const SampleMatrix & v) {        
        channels  = v.channels;
        return *this;
    }
    SampleType& operator()(size_t c, size_t n) {
        return channels(c,n);
    }
    bool operator == (const SampleMatrix & b) {
        return channels.rows() == b.channels.rows() && channels.cols() == b.channels.cols();
    }
    SampleMatrix operator + (const SampleMatrix & m) {        
        SampleMatrix r(*this);
        r.channels = channels + m.channels;
        return r;
    }
    SampleMatrix operator - (const SampleMatrix & m) {
        assert(*this == m);
        SampleMatrix r(*this);
        r.channels = channels - m.channels;
        return r;
    }
    SampleMatrix operator * (const SampleMatrix & m) {
        assert(*this == m);
        SampleMatrix r(*this);
        r.channels = channels * m.channels;
        return r;
    }
    SampleMatrix operator / (const SampleMatrix & m) {
        assert(*this == m);
        SampleMatrix r(*this);
        r.channels = channels / m.channels;
        return r;
    }
    
    SampleMatrix operator + (const SampleType s) {        
        SampleMatrix r(*this);        
        r.channels = channels + s;        
        return r;
    }
    SampleMatrix operator - (const SampleType s) {        
        SampleMatrix r(*this);        
        r.channels = channels - s;
        return r;
    }
    SampleMatrix operator * (const SampleType s) {        
        SampleMatrix r(*this);        
        r.channels = channels * s;
        return r;
    }
    SampleMatrix operator / (const SampleType s) {        
        SampleMatrix r(*this);        
        r.channels = channels / s;
        return r;
    }
    

     SampleMatrix abs() const { 
        SampleMatrix r(*this);        
        r.channels = channels.abs();
        return r;
    }
    SampleMatrix abs2() const  { 
        SampleMatrix r(*this);        
        r.channels = channels.abs2();
        return r;
    }
    SampleMatrix inverse()const  { 
        SampleMatrix r(*this);        
        r.channels = channels.inverse();
        return r;
    }
    SampleMatrix exp() const  { 
        SampleMatrix r(*this);        
        r.channels = channels.exp();
        return r;
    }
    SampleMatrix log() const  { 
        SampleMatrix r(*this);        
        r.channels = channels.log();
        return r;
    }
    SampleMatrix log1p() const  { 
        SampleMatrix r(*this);        
        r.channels = channels.log1p();
        return r;
    }
    SampleMatrix log10() const  { 
        SampleMatrix r(*this);        
        r.channels = channels.log10();
        return r;
    }
    SampleMatrix pow(const SampleType s) const  { 
        SampleMatrix r(*this);        
        r.channels = channels.pow(s);
        return r;
    }
    SampleMatrix sqrt() const  {
        SampleMatrix r(*this);        
        r.channels = channels.sqrt();
        return r;
    }
    SampleMatrix rsqrt() const  {
        SampleMatrix r(*this);        
        r.channels = channels.rsqrt();
        return r;
    }
    SampleMatrix square() const  {
        SampleMatrix r(*this);        
        r.channels = channels.square();
        return r;
    }
    SampleMatrix sin() const  {
        SampleMatrix r(*this);        
        r.channels = channels.sin();
        return r;
    }
    SampleMatrix cos() const  {
        SampleMatrix r(*this);        
        r.channels = channels.cos();
        return r;
    }
    SampleMatrix tan() const {
        SampleMatrix r(*this);        
        r.channels = channels.tan();
        return r;
    }
    SampleMatrix asin() const {
        SampleMatrix r(*this);        
        r.channels = channels.asin();
        return r;
    }
    SampleMatrix acos() const {
        SampleMatrix r(*this);
        r.channels = channels.acos();
        return r;
    }
    SampleMatrix atan() const {
        SampleMatrix r(*this);
        r.channels = channels.atan();
        return r;
    }    
    SampleMatrix sinh() const {
        SampleMatrix r(*this);        
        r.channels = channels.sinh();
        return r;
    }
    SampleMatrix cosh() const {
        SampleMatrix r(*this);        
        r.channels = channels.cosh();
        return r;
    }
    SampleMatrix tanh() const {
        SampleMatrix r(*this);        
        r.channels = channels.tanh();
        return r;
    }
    SampleMatrix ceil() const {
        SampleMatrix r(*this);        
        r.channels = channels.ceil();
        return r;
    }
    SampleMatrix floor() const {
        SampleMatrix r(*this);        
        r.channels = channels.floor();
        return r;
    }
    SampleMatrix round() const {
        SampleMatrix r(*this);        
        r.channels = channels.round();
        return r;
    }
};

inline SampleVector abs(const SampleVector & m) { return m.abs(); }
inline SampleVector abs2(const SampleVector & m) { return m.abs2(); }
inline SampleVector inverse(const SampleVector & m) { return m.inverse(); }
inline SampleVector exp(const SampleVector & m) { return m.exp(); }
inline SampleVector log(const SampleVector & m) { return m.log(); }
inline SampleVector log1p(const SampleVector & m) { return m.log1p(); }
inline SampleVector log10(const SampleVector & m) { return m.log10(); }
inline SampleVector pow(const SampleVector & m, const SampleVector & p) { return m.pow(p); }
inline SampleVector pow(const SampleVector & m, const SampleType p) { return m.pow(p); }
inline SampleVector sqrt(const SampleVector & m) { return m.sqrt(); }
inline SampleVector rsqrt(const SampleVector & m) { return m.rsqrt(); }
inline SampleVector square(const SampleVector & m) { return m.square(); }
inline SampleVector sin(const SampleVector & m) { return m.sin(); }
inline SampleVector cos(const SampleVector & m) { return m.cos(); }
inline SampleVector tan(const SampleVector & m) { return m.tan(); }
inline SampleVector asin(const SampleVector & m) { return m.asin(); }
inline SampleVector acos(const SampleVector & m) { return m.acos(); }
inline SampleVector atan(const SampleVector & m) { return m.atan(); }
inline SampleVector sinh(const SampleVector & m) { return m.sinh(); }
inline SampleVector cosh(const SampleVector & m) { return m.cosh(); }
inline SampleVector tanh(const SampleVector & m) { return m.tanh(); }
inline SampleVector ceil(const SampleVector & m) { return m.ceil(); }
inline SampleVector floor(const SampleVector & m) { return m.floor(); }
inline SampleVector round(const SampleVector & m) { return m.round(); }

inline SampleMatrix abs(const SampleMatrix & m) { return m.abs(); }
inline SampleMatrix abs2(const SampleMatrix & m) { return m.abs2(); }
inline SampleMatrix inverse(const SampleMatrix & m) { return m.inverse(); }
inline SampleMatrix exp(const SampleMatrix & m) { return m.exp(); }
inline SampleMatrix log(const SampleMatrix & m) { return m.log(); }
inline SampleMatrix log1p(const SampleMatrix & m) { return m.log1p(); }
inline SampleMatrix log10(const SampleMatrix & m) { return m.log10(); }
inline SampleMatrix pow(const SampleMatrix & m, const SampleType &p) { return m.pow(p); }
inline SampleMatrix sqrt(const SampleMatrix & m) { return m.sqrt(); }
inline SampleMatrix rsqrt(const SampleMatrix & m) { return m.rsqrt(); }
inline SampleMatrix square(const SampleMatrix & m) { return m.square(); }
inline SampleMatrix sin(const SampleMatrix & m) { return m.sin(); }
inline SampleMatrix cos(const SampleMatrix & m) { return m.cos(); }
inline SampleMatrix tan(const SampleMatrix & m) { return m.tan(); }
inline SampleMatrix asin(const SampleMatrix & m) { return m.asin(); }
inline SampleMatrix acos(const SampleMatrix & m) { return m.acos(); }
inline SampleMatrix atan(const SampleMatrix & m) { return m.atan(); }
inline SampleMatrix sinh(const SampleMatrix & m) { return m.sinh(); }
inline SampleMatrix cosh(const SampleMatrix & m) { return m.cosh(); }
inline SampleMatrix tanh(const SampleMatrix & m) { return m.tanh(); }
inline SampleMatrix ceil(const SampleMatrix & m) { return m.ceil(); }
inline SampleMatrix floor(const SampleMatrix & m) { return m.floor(); }
inline SampleMatrix round(const SampleMatrix & m) { return m.round(); }

}