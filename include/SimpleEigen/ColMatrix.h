#pragma once 

namespace SimpleEigen {
template<typename T>
using EigenColMatrix  = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>;
template<typename T>
using EigenCol2DArray  = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>;


// a hack to index from lua
template<typename T>
class ColMatrixView  : public Base
{
private:
    EigenColMatrix<T>  *matrix;
    size_t row;
    size_t col;
    bool bUseRows;
public:

    ColMatrixView (EigenColMatrix<T>  *m, size_t r)
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
struct ColMatrix
{
    EigenColMatrix<T>  matrix;

    ColMatrix() {}
    ColMatrix(int x, int y) { matrix = EigenColMatrix<T> (x,y); }
    ColMatrix(int x, int y, const T& val) { matrix = EigenColMatrix<T> (x,y); matrix.fill(val);  }
    ColMatrix(const RowVector<T> & v) { matrix = v.vector; }
    ColMatrix(const ColVector<T> & v) { matrix = v.vector; }
    ColMatrix(const ColMatrix<T> & m) { matrix = m.matrix;  }    
    ColMatrix(const EigenColMatrix<T> & m) { matrix = m; }    
    ColMatrix(const Eigen::Matrix<T,1,Eigen::Dynamic> & v) { matrix = v; }    
    ColMatrix(const Eigen::Matrix<T,Eigen::Dynamic,1> & c) { matrix = c; }    
    ColMatrix(const Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> & a) { matrix = a.matrix(); }
    ColMatrix(const std::vector<T> & data, size_t r, size_t c) {       
        matrix.resize(r,c);
        memcpy(matrix.data(),data.data(),data.size()*sizeof(T));        
    }

    T& operator()(size_t i, size_t j) { return matrix(i,j); }
    
    void random(int x, int y) { matrix =  EigenColMatrix<T> ::Random(x,y); }
    void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
    void identity(int x, int y) { matrix = EigenColMatrix<T> ::Identity(x,y); }
    void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
    void zero(int x, int y) { matrix = EigenColMatrix<T> ::Zero(x,y); }
    void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
    void ones(int x, int y) { matrix = EigenColMatrix<T> ::Ones(x,y); }
    void ones() { matrix.setOnes(matrix.rows(),matrix.cols()); }

    T get(int i, int j) { return matrix(i,j); }
    void set(int i, int j, T v) { matrix(i,j) = v; }    
    T norm() { return matrix.norm(); }
    T squaredNorm() { return matrix.squaredNorm(); }

    bool all() { return matrix.all(); }
    bool allFinite() { return matrix.allFinite(); }
    bool any() { return matrix.any(); }
    bool count() { return matrix.count(); }

    size_t rows() const { return matrix.rows(); }
    size_t cols() const { return matrix.cols(); }
    void resize(int x, int y) { matrix.resize(x,y); }

    T* data() { return matrix.data(); }    
    
    RowVector<T> operator[](int index) { 
        if(index < 0) index += rows();
        EigenColMatrix<T>  r = matrix.row(index);
        return RowVector<T>(r);        
    }
    ColMatrixView <T> __getitem(int r) { 
        if(r < 0) r += rows();
        return ColMatrixView <T>(&matrix, r);
     }
    void __setitem(int r, const RowVector<T> & v) { 
        if(r < 0) r += rows();
        matrix.row(r) = v.vector; 
    }
  
    void normalize()
    {
       matrix.normalize();
    }
    ColMatrix<T>  normalized() { 
        ColMatrix<T>  r; 
        r.matrix = matrix.normalized(); 
        return r; 
    }    

    void fill(T v) { matrix.fill(v); }
    ColMatrix<T>  eval() { return ColMatrix<T> (matrix.eval()); }
    bool hasNaN() { return matrix.hasNaN(); }
    size_t innerSize() { return matrix.innerSize(); }
    size_t outerSize() { return matrix.outerSize(); }    
    bool isMuchSmallerThan(const ColMatrix<T> & n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
    bool isOnes() { return matrix.isOnes(); }
    bool isZero() { return matrix.isZero(); }  
    
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
    void set_row_matrix(size_t row, ColMatrix<T>  & v, size_t src_row)    {        
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
    void set_col_matrix(size_t col, ColMatrix<T>  & v, size_t row)    {
        assert(rows() == v.cols());
        assert(cols() == v.rows());
        for(size_t i = 0; i < rows(); i++)
            matrix(i,col) = v.matrix(row,i);

    }

    
    ColMatrix<T>  leftCols(ColMatrix<T> & r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
    ColMatrix<T>  middleCols(ColMatrix<T> & r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
    ColMatrix<T>  rightCols(ColMatrix<T> & r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
    ColMatrix<T>  topRows(ColMatrix<T> & r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
    ColMatrix<T>  middleRows(ColMatrix<T> & r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
    ColMatrix<T>  bottomRows(ColMatrix<T> & r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
    ColMatrix<T>  topLeftCorner(ColMatrix<T> & r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
    ColMatrix<T>  topRightCorner(ColMatrix<T> & r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

    
    ColMatrix<T>  adjoint()  { ColMatrix<T>  m; m.matrix = matrix.adjoint().eval(); return m; }
    ColMatrix<T>  transpose() { ColMatrix<T>  m; m.matrix = matrix.transpose().eval(); return m; }
    ColMatrix<T>  diagonal() { ColMatrix<T>  m; m.matrix = matrix.diagonal().eval(); return m; }        
    ColMatrix<T>  reverse() { ColMatrix<T>  m; m.matrix = matrix.reverse().eval(); return m; }    
    ColMatrix<T>  replicate(size_t i, size_t j) { ColMatrix<T>  m; m.matrix = matrix.replicate(i,j).eval(); return m; }
    
    void adjointInPlace() {  matrix.adjointInPlace(); }    
    void transposeInPlace() { matrix.transposeInPlace(); }
    void reverseInPlace() { matrix.reverseInPlace(); }

    
    ColMatrix<T>  adjointInto(ColMatrix<T>  & m) { m.matrix = matrix.adjoint().eval(); return m; }    
    ColMatrix<T>  transposeInto(ColMatrix<T>  & m) { m.matrix = matrix.transpose().eval();return m; }    
    ColMatrix<T>  diagonalInto(ColMatrix<T>  & m) { m.matrix = matrix.diagonal().eval();return m; }
    ColMatrix<T>  reverseInto(ColMatrix<T>  & m) { m.matrix = matrix.reverse().eval();return m; }
    ColMatrix<T>  replicateInto(ColMatrix<T>  & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
    
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
    
    ColMatrix<T>  addToEachRow(ColMatrix<T>  & v)    {
        ColMatrix<T>  r(*this);        
        r.matrix = r.matrix.rowwise() + RowVector<T>(v.matrix).vector;
        return r;
    }
    ColMatrix<T>  cwiseAbs()    {
        EigenColMatrix<T>  r = matrix.cwiseAbs();
        return r;
    }
    ColMatrix<T>  cwiseAbs2()    {
        EigenColMatrix<T>  r = matrix.cwiseAbs2();
        return r;
    }
    ColMatrix<T>  cwiseProduct(const ColMatrix<T> & q)    {
        EigenColMatrix<T>  r = matrix.cwiseProduct(q.matrix); 
        return r;
    }
    ColMatrix<T>  cwiseQuotient(const ColMatrix<T> & q)    {
        EigenColMatrix<T>  r = matrix.cwiseQuotient(q.matrix); 
        return r;
    }
    ColMatrix<T>  cwiseInverse()    {
        EigenColMatrix<T>  r = matrix.cwiseInverse();
        return r;
    }
    ColMatrix<T>  cwiseSqrt()    {
        EigenColMatrix<T>  r = matrix.cwiseSqrt();
        return r;
    }
    ColMatrix<T>  cwiseMax(ColMatrix<T> & q)    {
        EigenColMatrix<T>  r = matrix.cwiseMin(q.matrix);
        return r;        
    }
    ColMatrix<T>  cwiseMin(ColMatrix<T> & q)    {
        EigenColMatrix<T>  r = matrix.cwiseMin(q.matrix);
        return r;
    }



    bool operator == (const ColMatrix<T>  &m) {
        ColMatrix<T>  r;
        auto x = (matrix == m.matrix);
        return x;
    }
    bool operator != (const ColMatrix<T>  &m) {
        ColMatrix<T>  r;
        auto x = (matrix != m.matrix);
        return x;
    }
    bool operator >= (const ColMatrix<T>  &m) {
        ColMatrix<T>  r;
        auto x = (matrix >= m.matrix);
        return x;
    }
    bool operator > (const ColMatrix<T>  &m) {
        ColMatrix<T>  r;
        auto x = (matrix > m.matrix);
        return x;
    }        
    

    ColMatrix<T> & operator=(const ColMatrix<T>  & m)    {        
        matrix = m.matrix.eval();        
        return *this;
    }
    ColMatrix<T> & operator=(const EigenColMatrix<T>  & m)    {        
        matrix = m;
        return *this;
    }
    ColMatrix<T> & operator=(const Scalar<T> & m)    {        
        fill(m.val);
        return *this;
    }

    ColMatrix<T>  operator * (const ColMatrix<T>  & m)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix * m.matrix;
        return r;
    } 
    ColMatrix<T>  operator + (const ColMatrix<T>  & m)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix + m.matrix;
        return r;
    }
    ColMatrix<T>  operator - (const ColMatrix<T>  & m)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix - m.matrix;
        return r;
    }

    ColMatrix<T>  operator * (const Scalar<T> & m)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix * m.val;
        return r;
    }
    ColMatrix<T>  operator + (const Scalar<T> & m)    {
        ColMatrix<T>  r(*this);
        ColMatrix<T>  x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix + x.matrix;
        return r;
    }
    ColMatrix<T>  operator - (const Scalar<T> & m)    {
        ColMatrix<T>  r(*this);
        ColMatrix<T>  x(r.rows(),r.cols());
        x.fill(m.val);
        r.matrix = r.matrix - x.matrix;
        return r;
    }
    ColMatrix<T>  operator / (const Scalar<T> & m)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix / m.val;
        return r;
    }
    ColMatrix<T>  operator - ()     {
        ColMatrix<T>  r(*this);
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
    ColMatrix<T>  operator * (const T f)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix * f;
        return r;
    }    
    ColMatrix<T>  operator / (const T f)    {
        ColMatrix<T>  r(*this);
        r.matrix = r.matrix / f;
        return r;
    }
        
   void print()     {
        std::cout << matrix << std::endl;
    }
    
    ColMatrix<T>  t()     {
        return transpose();
    }
    

// slice 

    ColMatrix<T>  slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
        if(last_r = -1) last_r = matrix.rows()-1;
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seq(first_c,last_c));
        ColMatrix<T>  ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    ColMatrix<T>  sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
        if(last_c = -1) last_c = matrix.cols()-1;
        auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seq(first_c,last_c));
        ColMatrix<T>  ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    ColMatrix<T>  sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
        auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seqN(first_c,first_cn));
        ColMatrix<T>  ret(r.rows(),r.cols());
        for(size_t i = 0; i < r.rows(); i++)
            for(size_t j = 0; j < r.cols(); j++)
                ret.matrix(i,j) = r(i,j);
        return ret;
    }

    ColMatrix<T>  slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
        auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seqN(first_c,first_cn));
        ColMatrix<T>  ret(r.rows(),r.cols());
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

    EigenCol2DArray <T> array() const { return matrix.array(); }
    
};



template<typename T> ColMatrix<T>  abs(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().abs().matrix(); return r;}
template<typename T> ColMatrix<T>  inverse(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().inverse().matrix(); return r; }
template<typename T> ColMatrix<T>  exp(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().exp().matrix(); return r;  }
template<typename T> ColMatrix<T>  log(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().log().matrix(); return r;  }
template<typename T> ColMatrix<T>  log1p(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().log1p().matrix(); return r; }
template<typename T> ColMatrix<T>  log10(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().log10().matrix(); return r; }
template<typename T> ColMatrix<T>  pow(const ColMatrix<T>  & matrix,const T& b) { ColMatrix<T>  r; r.matrix = matrix.array().pow(b).matrix(); return r;}
template<typename T> ColMatrix<T>  sqrt(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().sqrt().matrix(); return r;}
template<typename T> ColMatrix<T>  rsqrt(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().rsqrt().matrix(); return r; }
template<typename T> ColMatrix<T>  square(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().square().matrix(); return r;}
template<typename T> ColMatrix<T>  cube(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().cube().matrix(); return r; }
template<typename T> ColMatrix<T>  abs2(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().abs2().matrix(); return r; }
template<typename T> ColMatrix<T>  sin(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().sin().matrix(); return r;}
template<typename T> ColMatrix<T>  cos(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().cos().matrix(); return r; }
template<typename T> ColMatrix<T>  tan(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().tan().matrix(); return r;}
template<typename T> ColMatrix<T>  asin(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().asin().matrix(); return r; }
template<typename T> ColMatrix<T>  acos(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().acos().matrix(); return r; }
template<typename T> ColMatrix<T>  atan(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().atan().matrix(); return r; }
template<typename T> ColMatrix<T>  sinh(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().sinh().matrix(); return r; }
template<typename T> ColMatrix<T>  cosh(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().cosh().matrix(); return r; }
template<typename T> ColMatrix<T>  tanh(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().tanh().matrix(); return r; }
template<typename T> ColMatrix<T>  ceil(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().ceil().matrix(); return r; }
template<typename T> ColMatrix<T>  floor(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().floor().matrix(); return r; }
template<typename T> ColMatrix<T>  round(const ColMatrix<T>  & matrix) { ColMatrix<T>  r; r.matrix = matrix.array().round().matrix(); return r; }


template<typename T> 
ColMatrix<T>  asinh(const ColMatrix<T>  & matrix) {         
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> array = matrix.array();
    for(size_t j = 0; j < matrix.cols(); j++)
        array(j) = std::asinh(array(j));
    ColMatrix<T>  r(array);    
    return r;
}
template<typename T> 
ColMatrix<T>  acosh(const ColMatrix<T>  & matrix) { 
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> array = matrix.array();
    for(size_t j = 0; j < matrix.cols(); j++)
        array(j) = std::acosh(array(j));
    ColMatrix<T>  r(array);
    return r;
}
template<typename T> 
ColMatrix<T>  atanh(const ColMatrix<T>  & matrix) {             
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> array = matrix.array();
    for(size_t j = 0; j < matrix.cols(); j++)
        array(j) = std::atanh(array(j));
    ColMatrix<T>  r(array);
    return r;
}
template<typename T> 
ColMatrix<T>  rint(const ColMatrix<T>  & matrix)     { 
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> array = matrix.array();
    for(size_t j = 0; j < matrix.cols(); j++)
        array(j) = std::rint(array(j));
    ColMatrix<T>  r(array);
    return r;
}

template<typename T> 
void random(ColMatrix<T> & matrix, int x, int y) { matrix.random(x,y); }
template<typename T> 
void random(ColMatrix<T> & matrix) { matrix.random(); }
template<typename T> 
void identity(ColMatrix<T> & matrix,int x, int y) { matrix.identity(x,y); }
template<typename T> 
void identity(ColMatrix<T> & matrix) { matrix.identity(); }
template<typename T> 
void zero(ColMatrix<T> & matrix,int x, int y) { matrix.zero(x,y); }
template<typename T> 
void zero(ColMatrix<T> & matrix) { matrix.zero(); }
template<typename T> 
void ones(ColMatrix<T> & matrix,int x, int y) { matrix.ones(x,y); }
template<typename T> 
void ones(ColMatrix<T> & matrix) { matrix.ones(); }

template<typename T> 
T get(ColMatrix<T> & matrix,int i, int j) { return matrix.get(i,j); }

template<typename T> 
void set(ColMatrix<T> & matrix,int i, int j, T v) { matrix.set(i,j,v); }

template<typename T> 
T norm(ColMatrix<T> & matrix) { return matrix.norm(); }

template<typename T> 
T squaredNorm(ColMatrix<T> & matrix) { return matrix.squaredNorm(); }

template<typename T> 
bool all(ColMatrix<T> & matrix) { return matrix.all(); }

template<typename T> 
bool allFinite(ColMatrix<T> & matrix) { return matrix.allFinite(); }

template<typename T> 
bool any(ColMatrix<T> & matrix) { return matrix.any(); }

template<typename T> 
bool count(ColMatrix<T> & matrix) { return matrix.count(); }

template<typename T> 
size_t rows(ColMatrix<T> & matrix) { return matrix.rows(); }

template<typename T> 
size_t cols(ColMatrix<T> & matrix) { return matrix.cols(); }

template<typename T> 
void resize(ColMatrix<T> & matrix,int x, int y) { matrix.resize(x,y); }

template<typename T> 
void normalize(ColMatrix<T> & matrix) { matrix.normalize(); }

template<typename T> 
ColMatrix<T>  normalized(ColMatrix<T> & matrix) { return matrix.normalized(); }    


template<typename T> 
void fill(ColMatrix<T> & matrix,T v) { matrix.fill(v); }

template<typename T> 
ColMatrix<T>  eval(ColMatrix<T> & matrix) { return ColMatrix<T> (matrix.eval()); }

template<typename T> 
bool hasNaN(ColMatrix<T> & matrix) { return matrix.hasNaN(); }

template<typename T> 
size_t innerSize(ColMatrix<T> & matrix) { return matrix.innerSize(); }

template<typename T> 
size_t outerSize(ColMatrix<T> & matrix) { return matrix.outerSize(); }    

template<typename T> 
bool isMuchSmallerThan(ColMatrix<T> & matrix,const ColMatrix<T> & n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }

template<typename T> 
bool isOnes(ColMatrix<T> & matrix) { return matrix.isOnes(); }

template<typename T> 
bool isZero(ColMatrix<T> & matrix) { return matrix.isZero(); }

template<typename T> 
ColMatrix<T>  adjoint(ColMatrix<T> & matrix)  { return matrix.adjoint(); }

template<typename T> 
ColMatrix<T>  transpose(ColMatrix<T> & matrix) { return matrix.tranpose(); }

template<typename T> 
ColMatrix<T>  diagonal(ColMatrix<T> & matrix) { return matrix.diagonal(); }        

template<typename T> 
ColMatrix<T>  reverse(ColMatrix<T> & matrix) { return matrix.revese(); }    

template<typename T> 
ColMatrix<T>  replicate(ColMatrix<T> & matrix, size_t i, size_t j) { return matrix.replicate(i,j); }
    

template<typename T> 
T sum(ColMatrix<T> & matrix)    {        
    return matrix.sum();        
}

template<typename T> 
T prod(ColMatrix<T> & matrix)    {        
    return matrix.prod();        
}

template<typename T> 
T mean(ColMatrix<T> & matrix)    {        
    return matrix.mean();        
}

template<typename T> 
T minCoeff(ColMatrix<T> & matrix)    {        
    return matrix.minCoeff();        
}

template<typename T> 
T maxCoeff(ColMatrix<T> & matrix)    {        
    return matrix.maxCoeff();        
}    

template<typename T> 
T trace(ColMatrix<T> & matrix)    {        
    return matrix.trace();        
}

template<typename T> 
ColMatrix<T>  addToEachRow(ColMatrix<T>  & m, ColMatrix<T>  & v)    {
    ColMatrix<T>  r(m);        
    r.matrix = r.matrix.rowwise() + RowVector<T>(v.matrix).vector;
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseAbs(ColMatrix<T>  & matrix)    {
    EigenColMatrix<T>  r = matrix.cwiseAbs();
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseAbs2(ColMatrix<T>  & matrix)    {
    EigenColMatrix<T>  r = matrix.cwiseAbs2();
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseProduct(ColMatrix<T>  & matrix,const ColMatrix<T> & q)    {
    EigenColMatrix<T>  r = matrix.cwiseProduct(q.matrix); 
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseQuotient(ColMatrix<T>  & matrix, const ColMatrix<T> & q)    {
    EigenColMatrix<T>  r = matrix.cwiseQuotient(q.matrix); 
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseInverse(ColMatrix<T>  & matrix)    {
    EigenColMatrix<T>  r = matrix.cwiseInverse();
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseSqrt(ColMatrix<T>  & matrix)    {
    EigenColMatrix<T>  r = matrix.cwiseSqrt();
    return r;
}

template<typename T> 
ColMatrix<T>  cwiseMax(ColMatrix<T>  & matrix, ColMatrix<T> & q)    {
    EigenColMatrix<T>  r = matrix.cwiseMin(q.matrix);
    return r;        
}

template<typename T> 
ColMatrix<T>  cwiseMin(ColMatrix<T>  & matrix, ColMatrix<T> & q)    {
    EigenColMatrix<T>  r = matrix.cwiseMin(q.matrix);
    return r;
}

template<typename T> 
ColMatrix<T>  slice(ColMatrix<T>  & matrix,int first_r,int first_c, int last_r=-1, int last_c=-1)    {
    return matrix.slice(first_r,first_c,last_r,last_c);
}

template<typename T> 
ColMatrix<T>  sliceN1(ColMatrix<T>  & matrix,int first_r,int first_rn, int first_c, int last_c=-1)    {        
    return matrix.sliceN1(first_r,first_rn,first_c,last_c);
}

template<typename T> 
ColMatrix<T>  sliceN2(ColMatrix<T>  & matrix,int first_r,int first_c, int first_cn, int last_r=-1)    {                
    return matrix.sliceN2(first_r, first_c, first_cn, last_r);
}

template<typename T> 
ColMatrix<T>  slicen(ColMatrix<T>  & matrix,int first_r,int first_rn, int first_c, int first_cn)    {        
    return matrix.slicen(first_r,first_rn,first_c,first_cn);
}

template<typename T> 
EigenCol2DArray <T> array(ColMatrix<T>  & matrix) { return matrix.array(); }
}
