#pragma once 

namespace SimpleEigen {
template<typename T>
using EigenMatrix = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;
template<typename T>
using Eigen2DArray = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;


// a hack to index from lua
template<typename T>
class MatrixView : public Base
{
private:
    EigenMatrix<T> *matrix;
    size_t row;
    size_t col;
    bool bUseRows;
public:

    MatrixView(EigenMatrix<T> *m, size_t r)
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
struct Matrix
{
    EigenMatrix<T> matrix;

    Matrix() {}
    Matrix(int x, int y) { matrix = EigenMatrix<T>(x,y); }
    Matrix(int x, int y, const T& val) { matrix = EigenMatrix<T>(x,y); matrix.fill(val);  }
    Matrix(const RowVector<T> & v) { matrix = v.vector; }
    Matrix(const ColVector<T> & v) { matrix = v.vector; }
    Matrix(const Matrix<T>& m) { matrix = m.matrix;  }    
    Matrix(const EigenMatrix<T>& m) { matrix = m; }    
    Matrix(const Eigen::Matrix<T,1,Eigen::Dynamic> & v) { matrix = v; }    
    Matrix(const Eigen::Matrix<T,Eigen::Dynamic,1> & c) { matrix = c; }    
    Matrix(const Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> & a) { matrix = a.matrix(); }
    Matrix(const std::vector<T> & data, size_t r, size_t c) {       
        matrix.resize(r,c);
        memcpy(matrix.data(),data.data(),data.size()*sizeof(T));        
    }

    T& operator()(size_t i, size_t j) { return matrix(i,j); }
    
    void random(int x, int y) { matrix =  EigenMatrix<T>::Random(x,y); }
    void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
    void identity(int x, int y) { matrix = EigenMatrix<T>::Identity(x,y); }
    void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
    void zero(int x, int y) { matrix = EigenMatrix<T>::Zero(x,y); }
    void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
    void ones(int x, int y) { matrix = EigenMatrix<T>::Ones(x,y); }
    void ones() { matrix.setOnes(matrix.rows(),matrix.cols()); }

    T get(int i, int j) { return matrix(i,j); }
    void set(int i, int j, T v) { matrix(i,j) = v; }
    //void swap(const Matrix<T>& m) { matrix.swap(m.matrix); }
    T norm() { return matrix.norm(); }
    //T lpNorm1() { return matrix.lpNorm<1>(); }
    //T lpNorm() { return matrix.lpNorm<Eigen::Infinity>(); }
    T squaredNorm() { return matrix.squaredNorm(); }

    bool all() { return matrix.all(); }
    bool allFinite() { return matrix.allFinite(); }
    bool any() { return matrix.any(); }
    bool count() { return matrix.count(); }

    size_t rows() const { return matrix.rows(); }
    size_t cols() const { return matrix.cols(); }
    void resize(int x, int y) { matrix.resize(x,y); }

    
    //Matrix<T> reshaped(size_t i, size_t j) { return Matrix<T>(matrix.reshaped(i,j)); }

    T* data() { return matrix.data(); }    
    //Matrix<T> asDiagonal() { return Matrix<T>(matrix.asDiagonal()); }

    
    RowVector<T> operator[](int index) { 
        if(index < 0) index += rows();
        EigenMatrix<T> r = matrix.row(index);
        return RowVector<T>(r);        
    }
    MatrixView<T> __getitem(int r) { 
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
    Matrix<T> normalized() { 
        Matrix<T> r; 
        r.matrix = matrix.normalized(); 
        return r; 
    }    

    void fill(T v) { matrix.fill(v); }
    Matrix<T> eval() { return Matrix<T>(matrix.eval()); }
    bool hasNaN() { return matrix.hasNaN(); }
    size_t innerSize() { return matrix.innerSize(); }
    size_t outerSize() { return matrix.outerSize(); }    
    bool isMuchSmallerThan(const Matrix<T>& n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
    bool isOnes() { return matrix.isOnes(); }
    bool isZero() { return matrix.isZero(); }
    //Matrix<T> lazyAssign(const Matrix<T>& other) { return Matrix<T>(matrix.lazyAssign(other.matrix)); }
    //size_t nonZeros() { return matrix.nonZeros(); }
    
    
    RowVector<T> row_vector(size_t row) { RowVector<T> r; r.vector = matrix.row(row); return r; }    
    RowVector<T> row(size_t row) { 
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
    void set_row_matrix(size_t row, Matrix<T> & v, size_t src_row)    {        
        for(size_t i = 0; i < cols(); i++)
            matrix(row,i) = v.matrix(src_row,i);
    }

    
    ColVector<T> col_vector(size_t col) { ColVector<T> r; r.vector = matrix.col(col); return r; }
    ColVector<T> col(size_t col)     { 
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
    void set_col_matrix(size_t col, Matrix<T> & v, size_t row)    {
        assert(rows() == v.cols());
        assert(cols() == v.rows());
        for(size_t i = 0; i < rows(); i++)
            matrix(i,col) = v.matrix(row,i);

    }

    
    Matrix<T> leftCols(Matrix<T>& r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
    Matrix<T> middleCols(Matrix<T>& r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
    Matrix<T> rightCols(Matrix<T>& r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
    Matrix<T> topRows(Matrix<T>& r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
    Matrix<T> middleRows(Matrix<T>& r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
    Matrix<T> bottomRows(Matrix<T>& r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
    Matrix<T> topLeftCorner(Matrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
    Matrix<T> topRightCorner(Matrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

    
    Matrix<T> adjoint()  { Matrix<T> m; m.matrix = matrix.adjoint().eval(); return m; }
    Matrix<T> transpose() { Matrix<T> m; m.matrix = matrix.transpose().eval(); return m; }
    Matrix<T> diagonal() { Matrix<T> m; m.matrix = matrix.diagonal().eval(); return m; }        
    Matrix<T> reverse() { Matrix<T> m; m.matrix = matrix.reverse().eval(); return m; }    
    Matrix<T> replicate(size_t i, size_t j) { Matrix<T> m; m.matrix = matrix.replicate(i,j).eval(); return m; }
    
    void adjointInPlace() {  matrix.adjointInPlace(); }    
    void transposeInPlace() { matrix.transposeInPlace(); }
    void reverseInPlace() { matrix.reverseInPlace(); }

    
    Matrix<T> adjointInto(Matrix<T> & m) { m.matrix = matrix.adjoint().eval(); return m; }    
    Matrix<T> transposeInto(Matrix<T> & m) { m.matrix = matrix.transpose().eval();return m; }    
    Matrix<T> diagonalInto(Matrix<T> & m) { m.matrix = matrix.diagonal().eval();return m; }
    Matrix<T> reverseInto(Matrix<T> & m) { m.matrix = matrix.reverse().eval();return m; }
    Matrix<T> replicateInto(Matrix<T> & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
    
    /*
    Matrix<T> outer_product(Matrix<T> & m)
    {   return Matrix<T>(matrix * m.t().matrix); }
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
    Matrix<T> addToEachRow(Matrix<T> & v)    {
        Matrix<T> r(*this);        
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
    Matrix<T> cwiseAbs()    {
        EigenMatrix<T> r = matrix.cwiseAbs();
        return r;
    }
    Matrix<T> cwiseAbs2()    {
        EigenMatrix<T> r = matrix.cwiseAbs2();
        return r;
    }
    Matrix<T> cwiseProduct(const Matrix<T>& q)    {
        EigenMatrix<T> r = matrix.cwiseProduct(q.matrix); 
        return r;
    }
    Matrix<T> cwiseQuotient(const Matrix<T>& q)    {
        EigenMatrix<T> r = matrix.cwiseQuotient(q.matrix); 
        return r;
    }
    Matrix<T> cwiseInverse()    {
        EigenMatrix<T> r = matrix.cwiseInverse();
        return r;
    }
    Matrix<T> cwiseSqrt(Matrix<T>& out)    {
        EigenMatrix<T> r = matrix.cwiseSqrt();
        return r;
    }
    Matrix<T> cwiseMax(Matrix<T>& q)    {
        EigenMatrix<T> r = matrix.cwiseMin(q.matrix);
        return r;        
    }
    Matrix<T> cwiseMin(Matrix<T>& q)    {
        EigenMatrix<T> r = matrix.cwiseMin(q.matrix);
        return r;
    }

    /*
    Matrix<T> cwiseEqual(Matrix<T>& q)
    {
        auto r = matrix.cwiseEqual(q.matrix);
        Matrix<T> x;
        x.matrix = r;
        return x;
    }
    Matrix<T> cwiseNotEqual(Matrix<T>& q)
    {
        auto r = matrix.cwiseNotEqual(q.matrix);
        Matrix<T> x;
        x.matrix = r;
        return x;
    }
    */

    bool operator == (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix == m.matrix);
        return x;
    }
    bool operator != (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix != m.matrix);
        return x;
    }
    bool operator >= (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix >= m.matrix);
        return x;
    }
    bool operator > (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix > m.matrix);
        return x;
    }        
    /* eigen is kind of a huge mess inside
    bool operator <= (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix.array() <= m.matrix.array());
        return x;        
    }    
    bool operator < (const Matrix<T> &m) {
        Matrix<T> r;
        auto x = (matrix.array() < m.matrix.array());
        return x;
    }
    */
    

    Matrix<T>& operator=(const Matrix<T> & m)    {        
        matrix = m.matrix.eval();        
        /*
        matrix.resize(m.rows(),m.cols());
        for(size_t i = 0; i < m.rows(); i++)
            for(size_t j = 0; j < m.cols(); j++)
                matrix(i,j) = m.matrix(i,j);
        */
        return *this;
    }
    Matrix<T>& operator=(const EigenMatrix<T> & m)    {        
        matrix = m;
        return *this;
    }
    Matrix<T>& operator=(const Scalar<T> & m)    {        
        fill(m.val);
        return *this;
    }

    Matrix<T> operator * (const Matrix<T> & m)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix * m.matrix;
        return r;
    } 
    Matrix<T> operator + (const Matrix<T> & m)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix + m.matrix;
        return r;
    }
    Matrix<T> operator - (const Matrix<T> & m)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix - m.matrix;
        return r;
    }

    Matrix<T> operator * (const Scalar<T> & m)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix * m.val;
        return r;
    }
    Matrix<T> operator + (const Scalar<T> & m)    {
        Matrix<T> r(*this);
        Matrix<T> x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix + x.matrix;
        return r;
    }
    Matrix<T> operator - (const Scalar<T> & m)    {
        Matrix<T> r(*this);
        Matrix<T> x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix - x.matrix;
        return r;
    }
    Matrix<T> operator / (const Scalar<T> & m)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix / m.val;
        return r;
    }
    Matrix<T> operator - ()     {
        Matrix<T> r(*this);
        r.matrix = -matrix; 
        return r;
    }
    RowVector<T> operator * (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix * v.vector;
        return r;
    }
    RowVector<T> operator + (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix + v.vector;
        return r;
    }
    RowVector<T> operator - (const RowVector<T> & v)    {
        RowVector<T> r;
        r.vector = matrix - v.vector;
        return r;
    }
    ColVector<T> operator * (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix * v.vector;
        return r;
    }
    ColVector<T> operator + (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix + v.vector;
        return r;
    }
    ColVector<T> operator - (const ColVector<T> & v)    {
        ColVector<T> r;
        r.vector = matrix - v.vector;
        return r;
    }
    Matrix<T> operator * (const T f)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix * f;
        return r;
    }    
    Matrix<T> operator / (const T f)    {
        Matrix<T> r(*this);
        r.matrix = r.matrix / f;
        return r;
    }
        
   void print()     {
        std::cout << matrix << std::endl;
    }
    
    Matrix<T> t()     {
        return transpose();
    }
    

// slice 

    Matrix<T> slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
        if(last_r = -1) last_r = matrix.rows()-1;
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seq(first_c,last_c));
        Matrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    Matrix<T> sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seq(first_c,last_c));
        Matrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    Matrix<T> sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
        auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seqN(first_c,first_cn));
        Matrix<T> ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    Matrix<T> slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
        auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seqN(first_c,first_cn));
        Matrix<T> ret(r.rows(),r.cols());
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

    Eigen2DArray<T> array() const { return matrix.array(); }
    
};



template<typename T> Matrix<T> abs(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().abs().matrix(); return r;}
template<typename T> Matrix<T> inverse(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().inverse().matrix(); return r; }
template<typename T> Matrix<T> exp(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().exp().matrix(); return r;  }
template<typename T> Matrix<T> log(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log().matrix(); return r;  }
template<typename T> Matrix<T> log1p(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log1p().matrix(); return r; }
template<typename T> Matrix<T> log10(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log10().matrix(); return r; }
template<typename T> Matrix<T> pow(const Matrix<T> & matrix,const T& b) { Matrix<T> r; r.matrix = matrix.array().pow(b).matrix(); return r;}
template<typename T> Matrix<T> sqrt(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sqrt().matrix(); return r;}
template<typename T> Matrix<T> rsqrt(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().rsqrt().matrix(); return r; }
template<typename T> Matrix<T> square(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().square().matrix(); return r;}
template<typename T> Matrix<T> cube(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cube().matrix(); return r; }
template<typename T> Matrix<T> abs2(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().abs2().matrix(); return r; }
template<typename T> Matrix<T> sin(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sin().matrix(); return r;}
template<typename T> Matrix<T> cos(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cos().matrix(); return r; }
template<typename T> Matrix<T> tan(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().tan().matrix(); return r;}
template<typename T> Matrix<T> asin(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().asin().matrix(); return r; }
template<typename T> Matrix<T> acos(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().acos().matrix(); return r; }
template<typename T> Matrix<T> atan(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().atan().matrix(); return r; }
template<typename T> Matrix<T> sinh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sinh().matrix(); return r; }
template<typename T> Matrix<T> cosh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cosh().matrix(); return r; }
template<typename T> Matrix<T> tanh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().tanh().matrix(); return r; }
template<typename T> Matrix<T> ceil(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().ceil().matrix(); return r; }
template<typename T> Matrix<T> floor(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().floor().matrix(); return r; }
template<typename T> Matrix<T> round(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().round().matrix(); return r; }


template<typename T> Matrix<T> asinh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().asinh().matrix(); return r; }
template<typename T> Matrix<T> acosh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().acosh().matrix(); return r; }
template<typename T> Matrix<T> atanh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().atanh().matrix(); return r; }
template<typename T> Matrix<T> rint(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().rint().matrix(); return r; }


template<typename T> 
void random(Matrix<T> & matrix, int x, int y) { matrix.random(x,y); }
template<typename T> 
void random(Matrix<T> & matrix) { matrix.random(); }
template<typename T> 
void identity(Matrix<T> & matrix,int x, int y) { matrix.identity(x,y); }
template<typename T> 
void identity(Matrix<T> & matrix) { matrix.identity(); }
template<typename T> 
void zero(Matrix<T> & matrix,int x, int y) { matrix.zero(x,y); }
template<typename T> 
void zero(Matrix<T> & matrix) { matrix.zero(); }
template<typename T> 
void ones(Matrix<T> & matrix,int x, int y) { matrix.ones(x,y); }
template<typename T> 
void ones(Matrix<T> & matrix) { matrix.ones(); }

template<typename T> 
T get(Matrix<T> & matrix,int i, int j) { return matrix.get(i,j); }

template<typename T> 
void set(Matrix<T> & matrix,int i, int j, T v) { matrix.set(i,j,v); }

template<typename T> 
T norm(Matrix<T> & matrix) { return matrix.norm(); }

template<typename T> 
T squaredNorm(Matrix<T> & matrix) { return matrix.squaredNorm(); }

template<typename T> 
bool all(Matrix<T> & matrix) { return matrix.all(); }

template<typename T> 
bool allFinite(Matrix<T> & matrix) { return matrix.allFinite(); }

template<typename T> 
bool any(Matrix<T> & matrix) { return matrix.any(); }

template<typename T> 
bool count(Matrix<T> & matrix) { return matrix.count(); }

template<typename T> 
size_t rows(Matrix<T> & matrix) { return matrix.rows(); }

template<typename T> 
size_t cols(Matrix<T> & matrix) { return matrix.cols(); }

template<typename T> 
void resize(Matrix<T> & matrix,int x, int y) { matrix.resize(x,y); }

template<typename T> 
void normalize(Matrix<T> & matrix) { matrix.normalize(); }

template<typename T> 
Matrix<T>  normalized(Matrix<T> & matrix) { return matrix.normalized(); }    


template<typename T> 
void fill(Matrix<T> & matrix,T v) { matrix.fill(v); }

template<typename T> 
Matrix<T>  eval(Matrix<T> & matrix) { return Matrix<T> (matrix.eval()); }

template<typename T> 
bool hasNaN(Matrix<T> & matrix) { return matrix.hasNaN(); }

template<typename T> 
size_t innerSize(Matrix<T> & matrix) { return matrix.innerSize(); }

template<typename T> 
size_t outerSize(Matrix<T> & matrix) { return matrix.outerSize(); }    

template<typename T> 
bool isMuchSmallerThan(Matrix<T> & matrix,const Matrix<T> & n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }

template<typename T> 
bool isOnes(Matrix<T> & matrix) { return matrix.isOnes(); }

template<typename T> 
bool isZero(Matrix<T> & matrix) { return matrix.isZero(); }

template<typename T> 
Matrix<T>  adjoint(Matrix<T> & matrix)  { return matrix.adjoint(); }

template<typename T> 
Matrix<T>  transpose(Matrix<T> & matrix) { return matrix.tranpose(); }

template<typename T> 
Matrix<T>  diagonal(Matrix<T> & matrix) { return matrix.diagonal(); }        

template<typename T> 
Matrix<T>  reverse(Matrix<T> & matrix) { return matrix.revese(); }    

template<typename T> 
Matrix<T>  replicate(Matrix<T> & matrix, size_t i, size_t j) { return matrix.replicate(i,j); }
    

template<typename T> 
T sum(Matrix<T> & matrix)    {        
    return matrix.sum();        
}

template<typename T> 
T prod(Matrix<T> & matrix)    {        
    return matrix.prod();        
}

template<typename T> 
T mean(Matrix<T> & matrix)    {        
    return matrix.mean();        
}

template<typename T> 
T minCoeff(Matrix<T> & matrix)    {        
    return matrix.minCoeff();        
}

template<typename T> 
T maxCoeff(Matrix<T> & matrix)    {        
    return matrix.maxCoeff();        
}    

template<typename T> 
T trace(Matrix<T> & matrix)    {        
    return matrix.trace();        
}

template<typename T> 
Matrix<T>  addToEachRow(Matrix<T>  & m, Matrix<T>  & v)    {
    Matrix<T>  r(m);        
    r.matrix = r.matrix.rowwise() + RowVector<T>(v.matrix).vector;
    return r;
}

template<typename T> 
Matrix<T>  cwiseAbs(Matrix<T>  & matrix)    {
    EigenMatrix<T>  r = matrix.cwiseAbs();
    return r;
}

template<typename T> 
Matrix<T>  cwiseAbs2(Matrix<T>  & matrix)    {
    EigenMatrix<T>  r = matrix.cwiseAbs2();
    return r;
}

template<typename T> 
Matrix<T>  cwiseProduct(Matrix<T>  & matrix,const Matrix<T> & q)    {
    EigenMatrix<T>  r = matrix.cwiseProduct(q.matrix); 
    return r;
}

template<typename T> 
Matrix<T>  cwiseQuotient(Matrix<T>  & matrix, const Matrix<T> & q)    {
    EigenMatrix<T>  r = matrix.cwiseQuotient(q.matrix); 
    return r;
}

template<typename T> 
Matrix<T>  cwiseInverse(Matrix<T>  & matrix)    {
    EigenMatrix<T>  r = matrix.cwiseInverse();
    return r;
}

template<typename T> 
Matrix<T>  cwiseSqrt(Matrix<T>  & matrix)    {
    EigenMatrix<T>  r = matrix.cwiseSqrt();
    return r;
}

template<typename T> 
Matrix<T>  cwiseMax(Matrix<T>  & matrix, Matrix<T> & q)    {
    EigenMatrix<T>  r = matrix.cwiseMin(q.matrix);
    return r;        
}

template<typename T> 
Matrix<T>  cwiseMin(Matrix<T>  & matrix, Matrix<T> & q)    {
    EigenMatrix<T>  r = matrix.cwiseMin(q.matrix);
    return r;
}


template<typename T> 
Matrix<T>  slice(Matrix<T>  & matrix,int first_r,int first_c, int last_r=-1, int last_c=-1)    {
    return matrix.slice(first_r,first_c,last_r,last_c);
}

template<typename T> 
Matrix<T>  sliceN1(Matrix<T>  & matrix,int first_r,int first_rn, int first_c, int last_c=-1)    {        
    return matrix.sliceN1(first_r,first_rn,first_c,last_c);
}

template<typename T> 
Matrix<T>  sliceN2(Matrix<T>  & matrix,int first_r,int first_c, int first_cn, int last_r=-1)    {                
    return matrix.sliceN2(first_r, first_c, first_cn, last_r);
}

template<typename T> 
Matrix<T>  slicen(Matrix<T>  & matrix,int first_r,int first_rn, int first_c, int first_cn)    {        
    return matrix.slicen(first_r,first_rn,first_c,first_cn);
}

template<typename T> 
Eigen2DArray <T> array(Matrix<T>  & matrix) { return matrix.array(); }
}