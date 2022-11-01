#pragma once

   template<typename T>
    struct ArrayMatrix {
        std::vector<ArrayVector<T>> channels;
        ArrayMatrix(size_t chan, size_t samps) {
            channels.resize(chan);
            for(size_t i = 0; i < chan; i++)
                channels[i].resize(chan,samps);
        }
        ArrayMatrix(const ArrayVector<T> & v) {
            channels.resize(v.num_channels());
            for(size_t i = 0; i < v.num_channels(); i++) 
                channels[i] = v.get_channel(i);
        }
        ArrayMatrix(const ArrayMatrix<T> & m) {
            channels = m.channels;
        }

        void resize(size_t chan, size_t samps) {
            channels.resize(chan);
            for(size_t i = 0; i < chan; i++)
                channels[i].resize(samps);

        }
        size_t num_channels() const { return channels.size(); }
        size_t samples_per_channel(size_t ch = 0) const { return channels[ch].size()/channels.size(); }
        size_t size(size_t ch=0) const { return channels[ch].size(); }

        void deinterleave(const ArrayVector<T> & s) {            
            resize(s.num_channels(),s.samples_per_channel());                        
            for(size_t i = 0; i < s.num_channels(); i++) {
                channels.row(i) = s.get_channel(i).vector;
            }            
        }    
        ArrayVector<T> interleave() { 
            ArrayVector<T> r;
            r.resize(samples_per_channel(),num_channels());
            for(size_t i = 0; i < channels.size(); i++)
                r.set_channel(channels[i]);
            return r;
        }

        
        ArrayVector<T>& operator[](size_t ch) { return channels[ch]; }
        ArrayVector<T>& get_channel(size_t c) { return channels[c]; }
        void set_channel(size_t c, const ArrayVector<T> & s) { channels[c] = s; }

        ArrayMatrix<T>& operator = (const ArrayMatrix<T> & v) {        
            channels  = v.channels;
            return *this;
        }
        T& operator()(size_t c, size_t n) {
            return channels[c][n];
        }

        bool check(const ArrayMatrix<T> & b) {
            if(channels.size() != b.channels.size()) return false;
            for(size_t i = 0; i < channels.size(); i++)
                if(channels[i].size() != b.channels[i].size()) return false;
            return true;
        }
        bool operator == (const ArrayMatrix<T> & b) {
            return check(b);
        }
        ArrayMatrix<T> operator + (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] +  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator - (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] -  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator * (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] *  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator / (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] / m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator % (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] % m.channels[i];
            return r;
        }

        ArrayMatrix<T> operator + (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] + s;
            return r;
        }
        ArrayMatrix<T> operator - (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] - s;
            return r;
        }
        ArrayMatrix<T> operator * (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] * s;
            return r;
        }
        ArrayMatrix<T> operator / (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] / s;
            return r;
        }
        ArrayMatrix<T> operator % (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] % s;
            return r;
        }


        ArrayMatrix<T> abs() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].abs();
            return r;
        }
        ArrayMatrix<T> exp() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].exp();
            return r;
        }
        ArrayMatrix<T> log() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].log();
            return r;
        }
        ArrayMatrix<T> log10() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].log10();
            return r;
        }
        ArrayMatrix<T> pow(const ArrayVector<T> & s) { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].pow(s);
            return r;
        }
        ArrayMatrix<T> pow(const T s) { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].pow(s);
            return r;
        }
        ArrayMatrix<T> sqrt() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sqrt();
            return r;
        }
        ArrayMatrix<T> sin() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sin();
            return r;
        }
        ArrayMatrix<T> cos() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].cos();
            return r;
        }
        ArrayMatrix<T> tan() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].tan();
            return r;
        }
        ArrayMatrix<T> asin() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].asin();
            return r;
        }
        ArrayMatrix<T> acos() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].acos();
            return r;
        }
        ArrayMatrix<T> atan() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].atan();
            return r;
        }
        ArrayMatrix<T> atan2(const T s) {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].atan2(s);
            return r;
        }
        ArrayMatrix<T> sinh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sinh();
            return r;
        }
        ArrayMatrix<T> cosh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].cosh();
            return r;
        }
        ArrayMatrix<T> tanh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].tanh();
            return r;
        }
    };

    template<typename T> ArrayMatrix<T> abs(ArrayMatrix<T> & a) { return a.abs(); }
    template<typename T> ArrayMatrix<T> exp(ArrayMatrix<T> & a) { return a.exp(); }           
    template<typename T> ArrayMatrix<T> log(ArrayMatrix<T> & a) { return a.log(); } 
    template<typename T> ArrayMatrix<T> log10(ArrayMatrix<T> & a) { return a.log10(); }    
    template<typename T> ArrayMatrix<T> pow(ArrayMatrix<T> & a, const T s) { return a.pow(s); }
    template<typename T> ArrayMatrix<T> sqrt(ArrayMatrix<T> & a) { return a.sqrt(); }
    template<typename T> ArrayMatrix<T> sin(ArrayMatrix<T> & a) { return a.sin(); }
    template<typename T> ArrayMatrix<T> cos(ArrayMatrix<T> & a) { return a.cos(); }
    template<typename T> ArrayMatrix<T> tan(ArrayMatrix<T> & a) { return a.tan(); }
    template<typename T> ArrayMatrix<T> asin(ArrayMatrix<T> & a) { return a.asin(); }
    template<typename T> ArrayMatrix<T> acos(ArrayMatrix<T> & a) { return a.acos(); }
    template<typename T> ArrayMatrix<T> atan(ArrayMatrix<T> & a) { return a.atan(); }
    template<typename T> ArrayMatrix<T> atan2(ArrayMatrix<T> & a, const T s) { return a.atan2(s); }
    template<typename T> ArrayMatrix<T> sinh(ArrayMatrix<T> & a) { return a.sinh(); }
    template<typename T> ArrayMatrix<T> cosh(ArrayMatrix<T> & a) { return a.cosh(); }
    template<typename T> ArrayMatrix<T> tanh(ArrayMatrix<T> & a) { return a.tanh(); }

    template<typename T> ArrayMatrix<T> interleave(const std::vector<ArrayVector<T>> & c) {
        ArrayMatrix<T> m(c.size(),c[0].size());
        for(size_t i = 0; i < c.size(); i++)
            m.set_channel(i,c[i]);
        return m;
    }
