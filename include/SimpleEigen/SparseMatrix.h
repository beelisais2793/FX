#pragma once

namespace SimpleEigen {
template<typename T>
using EigenSparseMatrix = Eigen::SparseMatrix<T>;



// a hack to index from lua
template<typename T>
class SparseMatrixView : public Base
{
private:
    EigenSparseMatrix<T> *matrix;
    size_t row;
    size_t col;
    bool bUseRows;
public:

    SparseMatrixView(EigenSparseMatrix<T> *m, size_t r)
    {
        matrix = m;
        row = r;
        col = 0;
        bUseRows = true;
    }

    T __getitem(size_t i) { 
        if(bUseRows == true)
            return (*matrix)(row,i); 
        else 
            return (*matrix)(i,col); 
    }
    void  __setitem(size_t i, const T& val) { 
        if(bUseRows == true)
            (*matrix)(row,i) = val; 
        else 
            (*matrix)(i,col) = val;
        
    }

    void use_rows() { bUseRows = true; }
    void use_cols() { bUseRows = false; }
    size_t rows() { return matrix->rows(); }
    size_t cols() { return matrix->cols(); }
    void   set_row(size_t r) { row = r; }
    void   set_col(size_t c) { col = c;}
    
};



template<typename T>
struct SparseMatrix
{
    EigenSparseMatrix<T> matrix;

    SparseMatrix() {}
    SparseMatrix(int x, int y) { matrix = EigenSparseMatrix<T>(x,y); }
    SparseMatrix(int x, int y, const T& val) { matrix = EigenSparseMatrix<T>(x,y); matrix.fill(val);  }
    SparseMatrix(const SparseMatrix<T>& m) { matrix = m.matrix;  }    
    SparseMatrix(const EigenSparseMatrix<T>& m) { matrix = m; }            
    
    T& operator()(size_t i, size_t j) { return matrix(i,j); }
    
    void random(int x, int y) { matrix =  EigenSparseMatrix<T>::Random(x,y); }
    void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
    void identity(int x, int y) { matrix = EigenSparseMatrix<T>::Identity(x,y); }
    void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
    void zero(int x, int y) { matrix = EigenSparseMatrix<T>::Zero(x,y); }
    void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
    void ones(int x, int y) { matrix = EigenSparseMatrix<T>::Ones(x,y); }
    void ones() { matrix.setOnes(matrix.rows(),matrix.cols()); }

    T get(int i, int j) { return matrix(i,j); }
    void set(int i, int j, T v) { matrix(i,j) = v; }
    //void swap(const SparseMatrix<T>& m) { matrix.swap(m.matrix); }
    T norm() { return matrix.norm(); }
    //T lpNorm1() { return matrix.lpNorm<1>(); }
    //T lpNorm() { return matrix.lpNorm<matrix.Infinity>(); }
    T squaredNorm() { return matrix.squaredNorm(); }

    bool all() { return matrix.all(); }
    bool allFinite() { return matrix.allFinite(); }
    bool any() { return matrix.any(); }
    bool count() { return matrix.count(); }

    size_t rows() const { return matrix.rows(); }
    size_t cols() const { return matrix.cols(); }
    void resize(int x, int y) { matrix.resize(x,y); }

    
    //SparseMatrix<T> reshaped(size_t i, size_t j) { return SparseMatrix<T>(matrix.reshaped(i,j)); }

    T* data() { return matrix.data(); }    
    //SparseMatrix<T> asDiagonal() { return SparseMatrix<T>(matrix.asDiagonal()); }

    
    SparseRowVector<T> operator[](int index) { 
        if(index < 0) index += rows();
        EigenSparseMatrix<T> r = matrix.row(index);
        return RowVector<T>(r);        
    }
    SparseMatrixView<T> __getitem(int r) { 
        if(r < 0) r += rows();
        return MatrixView<T>(&matrix, r);
     }
    void __setitem(int r, const RowVector<T> & v) { 
        if(r < 0) r += rows();
        matrix.row(r) = v.vector; 
    }
    void normalize()
    {
       matrix.normalize();
    }
    SparseMatrix<T> normalized() { 
        SparseMatrix<T> r; 
        r.matrix = matrix.normalized(); 
        return r; 
    }    

    void fill(T v) { matrix.fill(v); }
    SparseMatrix<T> eval() { return SparseMatrix<T>(matrix.eval()); }
    bool hasNaN() { return matrix.hasNaN(); }
    size_t innerSize() { return matrix.innerSize(); }
    size_t outerSize() { return matrix.outerSize(); }    
    bool isMuchSmallerThan(const SparseMatrix<T>& n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
    bool isOnes() { return matrix.isOnes(); }
    bool isZero() { return matrix.isZero(); }
    //SparseMatrix<T> lazyAssign(const SparseMatrix<T>& other) { return SparseMatrix<T>(matrix.lazyAssign(other.matrix)); }
    //size_t nonZeros() { return matrix.nonZeros(); }
    
    
    SparseRowVector<T> row_vector(size_t row) { RowVector<T> r; r.vector = matrix.row(row); return r; }    
    SparseRowVector<T> row(size_t row) { 
        RowVector<T> r;        
        r.vector = matrix.row(row); 
        return r; 
    }        
    void set_row(size_t row, std::vector<T> & v)    {        
        for(size_t i = 0; i < cols(); i++)
            matrix(row,i) = v[i];
    }
    void set_row_vector(size_t row, RowVector<T> & v)    {        
        for(size_t i = 0; i < cols(); i++)
            matrix(row,i) = v(i);
    }
    void set_row_matrix(size_t row, SparseMatrix<T> & v, size_t src_row)    {        
        for(size_t i = 0; i < cols(); i++)
            matrix(row,i) = v.matrix(src_row,i);
    }

    
    SparseColVector<T> col_vector(size_t col) { ColVector<T> r; r.vector = matrix.col(col); return r; }
    SparseColVector<T> col(size_t col)     { 
        ColVector<T> r;        
        r.vector = matrix.col(col); 
        return r; 
    }
    void set_col(size_t col, std::vector<T> & v)    {
        assert(rows() == v.size());        
        for(size_t i = 0; i < rows(); i++)
            matrix(i,col) = v[i];
    }
    void set_col_vector(size_t col, ColVector<T> & v)    {
        assert(rows() == v.size());        
        for(size_t i = 0; i < rows(); i++)
            matrix(i,col) = v(i);
    }
    void set_col_matrix(size_t col, SparseMatrix<T> & v, size_t row)    {
        assert(rows() == v.cols());
        assert(cols() == v.rows());
        for(size_t i = 0; i < rows(); i++)
            matrix(i,col) = v.matrix(row,i);

    }

    
    SparseMatrix<T> leftCols(SparseMatrix<T>& r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
    SparseMatrix<T> middleCols(SparseMatrix<T>& r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
    SparseMatrix<T> rightCols(SparseMatrix<T>& r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
    SparseMatrix<T> topRows(SparseMatrix<T>& r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
    SparseMatrix<T> middleRows(SparseMatrix<T>& r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
    SparseMatrix<T> bottomRows(SparseMatrix<T>& r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
    SparseMatrix<T> topLeftCorner(SparseMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
    SparseMatrix<T> topRightCorner(SparseMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

    
    SparseMatrix<T> adjoint()  { SparseMatrix<T> m; m.matrix = matrix.adjoint().eval(); return m; }
    SparseMatrix<T> transpose() { SparseMatrix<T> m; m.matrix = matrix.transpose().eval(); return m; }
    SparseMatrix<T> diagonal() { SparseMatrix<T> m; m.matrix = matrix.diagonal().eval(); return m; }        
    SparseMatrix<T> reverse() { SparseMatrix<T> m; m.matrix = matrix.reverse().eval(); return m; }    
    SparseMatrix<T> replicate(size_t i, size_t j) { SparseMatrix<T> m; m.matrix = matrix.replicate(i,j).eval(); return m; }
    
    void adjointInPlace() {  matrix.adjointInPlace(); }    
    void transposeInPlace() { matrix.transposeInPlace(); }
    void reverseInPlace() { matrix.reverseInPlace(); }

    
    SparseMatrix<T> adjointInto(SparseMatrix<T> & m) { m.matrix = matrix.adjoint().eval(); return m; }    
    SparseMatrix<T> transposeInto(SparseMatrix<T> & m) { m.matrix = matrix.transpose().eval();return m; }    
    SparseMatrix<T> diagonalInto(SparseMatrix<T> & m) { m.matrix = matrix.diagonal().eval();return m; }
    SparseMatrix<T> reverseInto(SparseMatrix<T> & m) { m.matrix = matrix.reverse().eval();return m; }
    SparseMatrix<T> replicateInto(SparseMatrix<T> & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
    
    /*
    SparseMatrix<T> outer_product(SparseMatrix<T> & m)
    {   return SparseMatrix<T>(matrix * m.t().matrix); }
    */

    T sum()    {        
        return matrix.sum();        
    }
    T prod()    {        
        return matrix.prod();        
    }
    T mean()    {        
        return matrix.mean();        
    }
    T minCoeff()    {        
        return matrix.minCoeff();        
    }
    T maxCoeff()    {        
        return matrix.maxCoeff();        
    }    
    T trace()    {        
        return matrix.trace();        
    }

    
    
    // can't rowwise in Lua at this time.   
    SparseMatrix<T> addToEachRow(SparseMatrix<T> & v)    {
        SparseMatrix<T> r(*this);        
        r.matrix = r.matrix.rowwise() + RowVector<T>(v.matrix).vector;
        return r;
    }
    /*
    // put std::vector into matrix
    void map(std::vector<T> & a, size_t rows, size_t cols)    {
        matrix.resize(rows,cols);        
        for(size_t i = 0; i < rows; i++)
            for(size_t j = 0; i < cols; i++)
                matrix(i,j) = a[i*cols + j];
    }
    // get matrix to std::vector
    void unmap(std::vector<T> & r)    {
        r.resize(rows()*cols());
        for(size_t i = 0; i < rows(); i++)
            for(size_t j=0; j < cols(); j++)
                r[i*rows() + j] = matrix(i,j);
    }
    */
    SparseMatrix<T> cwiseAbs()    {
        EigenSparseMatrix<T> r = matrix.cwiseAbs();
        return r;
    }
    SparseMatrix<T> cwiseAbs2()    {
        EigenSparseMatrix<T> r = matrix.cwiseAbs2();
        return r;
    }
    SparseMatrix<T> cwiseProduct(const SparseMatrix<T>& q)    {
        EigenSparseMatrix<T> r = matrix.cwiseProduct(q.matrix); 
        return r;
    }
    SparseMatrix<T> cwiseQuotient(const SparseMatrix<T>& q)    {
        EigenSparseMatrix<T> r = matrix.cwiseQuotient(q.matrix); 
        return r;
    }
    SparseMatrix<T> cwiseInverse()    {
        EigenSparseMatrix<T> r = matrix.cwiseInverse();
        return r;
    }
    SparseMatrix<T> cwiseSqrt(SparseMatrix<T>& out)    {
        EigenSparseMatrix<T> r = matrix.cwiseSqrt();
        return r;
    }
    SparseMatrix<T> cwiseMax(SparseMatrix<T>& q)    {
        EigenSparseMatrix<T> r = matrix.cwiseMin(q.matrix);
        return r;        
    }
    SparseMatrix<T> cwiseMin(SparseMatrix<T>& q)    {
        EigenSparseMatrix<T> r = matrix.cwiseMin(q.matrix);
        return r;
    }

    /*
    SparseMatrix<T> cwiseEqual(SparseMatrix<T>& q)
    {
        auto r = matrix.cwiseEqual(q.matrix);
        SparseMatrix<T> x;
        x.matrix = r;
        return x;
    }
    SparseMatrix<T> cwiseNotEqual(SparseMatrix<T>& q)
    {
        auto r = matrix.cwiseNotEqual(q.matrix);
        SparseMatrix<T> x;
        x.matrix = r;
        return x;
    }
    */

    bool operator == (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix == m.matrix);
        return x;
    }
    bool operator != (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix != m.matrix);
        return x;
    }
    bool operator >= (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix >= m.matrix);
        return x;
    }
    bool operator > (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix > m.matrix);
        return x;
    }        
    /* eigen is kind of a huge mess inside
    bool operator <= (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix.array() <= m.matrix.array());
        return x;        
    }    
    bool operator < (const SparseMatrix<T> &m) {
        SparseMatrix<T> r;
        auto x = (matrix.array() < m.matrix.array());
        return x;
    }
    */
    

    SparseMatrix<T>& operator=(const SparseMatrix<T> & m)    {        
        matrix = m.matrix.eval();        
        /*
        matrix.resize(m.rows(),m.cols());
        for(size_t i = 0; i < m.rows(); i++)
            for(size_t j = 0; j < m.cols(); j++)
                matrix(i,j) = m.matrix(i,j);
        */
        return *this;
    }
    SparseMatrix<T>& operator=(const EigenSparseMatrix<T> & m)    {        
        matrix = m;
        return *this;
    }
    SparseMatrix<T>& operator=(const Scalar<T> & m)    {        
        fill(m.val);
        return *this;
    }

    SparseMatrix<T> operator * (const SparseMatrix<T> & m)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix * m.matrix;
        return r;
    } 
    SparseMatrix<T> operator + (const SparseMatrix<T> & m)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix + m.matrix;
        return r;
    }
    SparseMatrix<T> operator - (const SparseMatrix<T> & m)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix - m.matrix;
        return r;
    }

    SparseMatrix<T> operator * (const Scalar<T> & m)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix * m.val;
        return r;
    }
    SparseMatrix<T> operator + (const Scalar<T> & m)    {
        SparseMatrix<T> r(*this);
        SparseMatrix<T> x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix + x.matrix;
        return r;
    }
    SparseMatrix<T> operator - (const Scalar<T> & m)    {
        SparseMatrix<T> r(*this);
        SparseMatrix<T> x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix - x.matrix;
        return r;
    }
    SparseMatrix<T> operator / (const Scalar<T> & m)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix / m.val;
        return r;
    }
    SparseMatrix<T> operator - ()     {
        SparseMatrix<T> r(*this);
        r.matrix = -matrix; 
        return r;
    }
    SparseRowVector<T> operator * (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix * v.vector;
        return r;
    }
    SparseRowVector<T> operator + (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix + v.vector;
        return r;
    }
    SparseRowVector<T> operator - (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix - v.vector;
        return r;
    }
    SparseColVector<T> operator * (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix * v.vector;
        return r;
    }
    SparseColVector<T> operator + (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix + v.vector;
        return r;
    }
    SparseColVector<T> operator - (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix - v.vector;
        return r;
    }
    SparseMatrix<T> operator * (const T f)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix * f;
        return r;
    }    
    SparseMatrix<T> operator / (const T f)    {
        SparseMatrix<T> r(*this);
        r.matrix = r.matrix / f;
        return r;
    }
        
   void print()     {
        std::cout << matrix << std::endl;
    }
    
    SparseMatrix<T> t()     {
        return transpose();
    }
    

// slice 

    SparseMatrix<T> slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
        if(last_r = -1) last_r = matrix.rows()-1;
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(matrix.seq(first_r,last_r),matrix.seq(first_c,last_c));
        SparseMatrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    SparseMatrix<T> sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(matrix.seqN(first_r,first_rn),matrix.seq(first_c,last_c));
        SparseMatrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    SparseMatrix<T> sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
        auto r = matrix(matrix.seq(first_r,last_r),matrix.seqN(first_c,first_cn));
        SparseMatrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    SparseMatrix<T> slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
        auto r = matrix(matrix.seqN(first_r,first_rn),matrix.seqN(first_c,first_cn));
        SparseMatrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    void rowwise_add(RowVector<T> & r) {
        matrix.rowwise() += r.vector;
    }
    void rowwise_sub(RowVector<T> & r) {
        matrix.rowwise() -= r.vector;
    }
    void rowwise_mul(RowVector<T> & r) {
        matrix = (matrix.array().rowwise() * r.vector.array()).matrix();
    }
    void rowwise_div(RowVector<T> & r) {
        matrix = (matrix.array().rowwise() / r.vector.array()).matrix();
    }

    void colwise_add(ColVector<T> & r) {
        matrix.colwise() += r.vector;
    }
    void colwise_sub(ColVector<T> & r) {
        matrix.colwise() -= r.vector;
    }
    void colwise_mul(ColVector<T> & r) {
        matrix = (matrix.array().colwise() * r.vector.array()).matrix();
    }
    void colwise_div(ColVector<T> & r) {
        matrix = (matrix.array().colwise() / r.vector.array()).matrix();
    }


};


/*
template<typename T> SparseMatrix<T> abs(SparseMatrix<T> & matrix) { SparseMatrix<T> r; r = matrix.cwiseAbs(); return r;}
template<typename T> SparseMatrix<T> inverse(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cwiseInverse(); return r; }
template<typename T> SparseMatrix<T> exp(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.matrix.array().exp().matrix(); return r;  }
template<typename T> SparseMatrix<T> log(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.log(); return r;  }
template<typename T> SparseMatrix<T> log1p(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.log1p(); return r; }
template<typename T> SparseMatrix<T> log10(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.log10(); return r; }
template<typename T> SparseMatrix<T> pow(SparseMatrix<T> & matrix,const T& b) { SparseMatrix<T> r = matrix.pow(matrix.matrix,b); return r;}
template<typename T> SparseMatrix<T> sqrt(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cwiseSqrt(); return r;}
template<typename T> SparseMatrix<T> rsqrt(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.rsqrt(); return r; }
template<typename T> SparseMatrix<T> square(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.square(); return r;}
template<typename T> SparseMatrix<T> cube(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cube(); return r; }
template<typename T> SparseMatrix<T> abs2(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cwiseAbs2(); return r; }
template<typename T> SparseMatrix<T> sin(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.sin(); return r;}
template<typename T> SparseMatrix<T> cos(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cos(); return r; }
template<typename T> SparseMatrix<T> tan(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.tan(); return r;}
template<typename T> SparseMatrix<T> asin(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.asin(); return r; }
template<typename T> SparseMatrix<T> acos(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.acos(); return r; }
template<typename T> SparseMatrix<T> atan(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.atan(); return r; }
template<typename T> SparseMatrix<T> sinh(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.sinh(); return r; }
template<typename T> SparseMatrix<T> cosh(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.cosh(); return r; }
template<typename T> SparseMatrix<T> tanh(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.tanh(); return r; }
template<typename T> SparseMatrix<T> ceil(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.ceil(); return r; }
template<typename T> SparseMatrix<T> floor(SparseMatrix<T> & matrix) { SparseMatrix<T> r= matrix.floor(); return r; }
template<typename T> SparseMatrix<T> round(SparseMatrix<T> & matrix) { SparseMatrix<T> r = matrix.round(); return r; }
*/

};