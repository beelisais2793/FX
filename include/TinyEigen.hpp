#pragma once

template<typename T>
using EigenMatrix  = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;    
template<typename T>
using EigenArray   = Eigen::Array<T,1,Eigen::Dynamic,Eigen::RowMajor>;
template<typename T>
using EigenArray2D = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;
template<typename T>
using EigenVector  = Eigen::Matrix<T,1,Eigen::Dynamic,Eigen::RowMajor>;
template<typename T>
using EigenColMatrix  = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>;        
template<typename T>
using EigenColVector  = Eigen::Matrix<T,Eigen::Dynamic,1,Eigen::ColMajor>;

   template<typename T>
    struct Vector 
    {    
        EigenVector<T> vector;
        
        Vector() {}
        Vector(size_t i) { vector = EigenVector<T>(i); }
        Vector(size_t i, const T v) { vector = EigenVector<T>(i); vector.fill(v); }
        Vector(const Vector<T> & v) { vector = v.vector;  }
        Vector(const EigenVector<T> & v) { vector = v; }
        Vector(const std::vector<T> & v) { 
            vector.resize(v.size());
            memcpy(vector.data(),v.data(),v.size()*sizeof(T));
        }
        T get(int i) { return vector(i); }
        void  set(int i, T v) { vector(i) = v; }
        
        void random(int i) { vector = EigenVector<T>::Random(i); }
        void random() { vector.setRandom(vector.size()); }
        void zero() { vector.setZero(vector.size()); }    
        void ones() { vector.setOnes(vector.size()); }

        size_t  innerStride() { return vector.innerStride(); }
        T*  data() { return vector.data(); }
        void    resize(size_t size) { vector.resize(size); }
        void    resizeLike(const Vector<T> & v) { vector.resizeLike(v.vector); }
        void    conservativeResize(size_t size) { vector.conservativeResize(size); }

        T coeff(size_t i) { return vector.coeff(i); }
        T& coeffRef(size_t i) { return vector.coeffRef(i); }

        void print()     {        std::cout << vector << std::endl;    }
        size_t cols() { return vector.cols(); }
        T dot(const Vector<T> & b) { return vector.dot(b.vector);  }        
        size_t size() const { return vector.size(); }
        void fill(T v) { vector.fill(v); }
        T norm() { return vector.norm(); }
        T squaredNorm() { return vector.squaredNorm(); }

        Vector<T>& eval() { vector.eval(); return *this;  }

        void normalize() {
            vector.normalize();
        }

        Vector<T> normalized() { Vector<T> r; r.vector = vector.normalized(); return r; }            
        Vector<T>& head(Vector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
        Vector<T>& tail(Vector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
        Vector<T>& segment(Vector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
        Vector<T>& LinSpaced(T num, T a, T b)
        {        
            vector = EigenVector<T>::LinSpaced(num,a,b);
            return *this;
        }

        T operator()(size_t i) { return vector(i); }
        T operator[](size_t i) { return vector(i); }

        /*
        T inner_product(const Vector<T> & v)
        { return vector.adjoint() * v.vector; }
        */

        Vector<T> operator + (const Vector<T>& b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector + b.vector;
            return r;
        }
        Vector<T> operator - (const Vector<T>& b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector - b.vector;
            return r;
        }    
        
        T operator * (const Vector<T>& b) 
        { 
            return vector.dot(b.vector);        
        }

        
        Vector<T> operator * (const T b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector * b;
            return r;
        }

        Vector<T> operator / (const T b)  
        { 
            Vector<T> r(*this);
            r.vector = r.vector / b;
            return r;
        }

        
        void aplus(const Vector<T>& b) { vector += b.vector; }
        void aminus(const Vector<T>& b) { vector -= b.vector; }
        void adot(const Vector<T>& b) { vector.dot(b.vector); }
        
        void atimes(const T b) { vector = vector * b; }
        void adiv(const T b) { vector = vector / b; }


        Vector<T> t() { return Vector<T>(vector.transpose().eval());}    
        Vector<T> abs() {  return Vector<T>(vector.cwiseAbs());  }
        Vector<T> inverse() { return Vector<T>(vector.cwiseInverse()); }    
        Vector<T> sqrt() { return Vector<T>(vector.cwiseSqrt()); }
        Vector<T> abs2() { return Vector<T>(vector.cwiseAbs2()); }
        Vector<T> operator * (Matrix<T> & m);

        Vector<T> operator = (const Vector<T> & v) { vector = v.vector; return *this; }
        Vector<T> operator = (const EigenVector<T> & v) { vector = v; return *this; }

        // add to each col
        Matrix<T> operator + (Matrix<T> & m);
        // subtract from each col 
        Matrix<T> operator - (Matrix<T> & m);
        // divide from each col
        Matrix<T> operator / (Matrix<T> & m);

        Matrix<T> matrix();    
    };

    
    template<typename T> Vector<T>  abs(Vector<T> & vector ) { Vector<T>  r; r = vector.abs(); return r;}
    template<typename T> Vector<T>  inverse(Vector<T> & vector ) { Vector<T>  r; r = vector.inverse(); return r; }
    template<typename T> Vector<T>  exp(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().exp(); return r;  }
    template<typename T> Vector<T>  log(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log(); return r;  }
    template<typename T> Vector<T>  log1p(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log1p(); return r; }
    template<typename T> Vector<T>  log10(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log10(); return r; }
    template<typename T> Vector<T>  pow(Vector<T> & vector ,const T b) { Vector<T>  r; r = vector.vector.array().pow(b); return r;}
    template<typename T> Vector<T>  sqrt(Vector<T> & vector ) { Vector<T>  r; r = vector.sqrt(); return r;}
    template<typename T> Vector<T>  rsqrt(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().rsqrt(); return r; }
    template<typename T> Vector<T>  square(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().square(); return r;}
    template<typename T> Vector<T>  cube(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cube(); return r; }
    template<typename T> Vector<T>  abs2(Vector<T> & vector ) { Vector<T>  r; r = vector.abs2(); return r; }
    template<typename T> Vector<T>  sin(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().sin(); return r;}
    template<typename T> Vector<T>  cos(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cos(); return r; }
    template<typename T> Vector<T>  tan(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().tan(); return r;}
    template<typename T> Vector<T>  asin(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().asin(); return r; }
    template<typename T> Vector<T>  acos(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().acos(); return r; }
    template<typename T> Vector<T>  atan(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().atan(); return r; }
    template<typename T> Vector<T>  sinh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().sinh(); return r; }
    template<typename T> Vector<T>  cosh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cosh(); return r; }
    template<typename T> Vector<T>  tanh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().tanh(); return r; }
    template<typename T> Vector<T>  ceil(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().ceil(); return r; }
    template<typename T> Vector<T>  floor(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().floor(); return r; }
    template<typename T> Vector<T>  round(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().round(); return r; }

    
    template<typename T>
    class MatrixView 
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
        void  __setitem(size_t i, const T val) { 
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
        Matrix(int x, int y, const T val) { matrix = EigenMatrix<T>(x,y); matrix.fill(val);  }
        Matrix(const Vector<T> & v) { matrix = v.vector; }        
        Matrix(const Matrix<T>& m) { matrix = m.matrix;  }    
        Matrix(const EigenMatrix<T>& m) { matrix = m; }    
        Matrix(const EigenVector<T> & v) { matrix = v; }    
        Matrix(const EigenArray2D<T> & a) { matrix = a.matrix(); }
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
        
        Vector<T> operator[](int index) { 
            if(index < 0) index += rows();
            EigenMatrix<T> r = matrix.row(index);
            return Vector<T>(r);        
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
                
        Vector<T> row_vector(size_t row) { Vector<T> r; r.vector = matrix.row(row); return r; }    
        Vector<T> row(size_t row) { 
            Vector<T> r;        
            r.vector = matrix.row(row); 
            return r; 
        }        
        void set_row(size_t row, std::vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v[i];
        }
        void set_row_vector(size_t row, Vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v(i);
        }
        void set_row_matrix(size_t row, Matrix<T> & v, size_t src_row)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v.matrix(src_row,i);
        }

                
        void set_col(size_t col, std::vector<T> & v)    {
            assert(rows() == v.size());        
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v[i];
        }
        void set_col_vector(size_t col, Vector<T> & v)    {
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
            r.matrix = r.matrix.rowwise() + Vector<T>(v.matrix).vector;
            return r;
        }
        
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
        

        Matrix<T>& operator=(const Matrix<T> & m)    {        
            matrix = m.matrix.eval();                    
            return *this;
        }
        Matrix<T>& operator=(const EigenMatrix<T> & m)    {        
            matrix = m;
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

        
        Matrix<T> operator - ()     {
            Matrix<T> r(*this);
            r.matrix = -matrix; 
            return r;
        }
        Vector<T> operator * (const Vector<T> & v)    {
            Vector<T> r;
            r.vector = matrix * v.vector;
            return r;
        }
        Vector<T> operator + (const Vector<T> & v)    {
            Vector<T> r;
            r.vector = matrix + v.vector;
            return r;
        }
        Vector<T> operator - (const Vector<T> & v)    {
            Vector<T> r;
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

        void rowwise_add(Vector<T> & r) {
            matrix.rowwise() += r.vector;
        }
        void rowwise_sub(Vector<T> & r) {
            matrix.rowwise() -= r.vector;
        }
        void rowwise_mul(Vector<T> & r) {
            matrix = (matrix.array().rowwise() * r.vector.array()).matrix();
        }
        void rowwise_div(Vector<T> & r) {
            matrix = (matrix.array().rowwise() / r.vector.array()).matrix();
        }

        EigenArray2D<T> array() const { return matrix.array(); }
        
    };



    template<typename T> Matrix<T> abs(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().abs().matrix(); return r;}
    template<typename T> Matrix<T> inverse(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().inverse().matrix(); return r; }
    template<typename T> Matrix<T> exp(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().exp().matrix(); return r;  }
    template<typename T> Matrix<T> log(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log().matrix(); return r;  }
    template<typename T> Matrix<T> log1p(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log1p().matrix(); return r; }
    template<typename T> Matrix<T> log10(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log10().matrix(); return r; }
    template<typename T> Matrix<T> pow(const Matrix<T> & matrix,const T b) { Matrix<T> r; r.matrix = matrix.array().pow(b).matrix(); return r;}
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
    Vector<T> Vector<T>::operator * (Matrix<T> &b) 
    { 
        Vector<T> r;        
        r.vector =  b.matrix * vector;
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator + (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) + vector(j);
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator - (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) - vector(j);
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator / (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) / vector(j);
        return r;
    }

    
    template<typename T> Matrix<T> Vector<T>::matrix() { return Matrix<T>(vector); }

    template<typename T>
    struct ColVector 
    {    
        EigenColVector<T> vector;
        
        ColVector() {}
        ColVector(size_t i) { vector = EigenColVector<T>(i); }
        ColVector(size_t i, const T v) { vector = EigenColVector<T>(i); vector.fill(v); }
        ColVector(const ColVector<T> & v) { vector = v.vector;  }
        ColVector(const EigenColVector<T> & v) { vector = v; }
        ColVector(const std::vector<T> & v) { 
            vector.resize(v.size());
            memcpy(vector.data(),v.data(),v.size()*sizeof(T));
        }
        T get(int i) { return vector(i); }
        void  set(int i, T v) { vector(i) = v; }
        
        void random(int i) { vector = EigenColVector<T>::Random(i); }
        void random() { vector.setRandom(vector.size()); }
        void zero() { vector.setZero(vector.size()); }    
        void ones() { vector.setOnes(vector.size()); }

        size_t  innerStride() { return vector.innerStride(); }
        T*  data() { return vector.data(); }
        void    resize(size_t size) { vector.resize(size); }
        void    resizeLike(const ColVector<T> & v) { vector.resizeLike(v.vector); }
        void    conservativeResize(size_t size) { vector.conservativeResize(size); }

        T coeff(size_t i) { return vector.coeff(i); }
        T& coeffRef(size_t i) { return vector.coeffRef(i); }

        void print()     {        std::cout << vector << std::endl;    }
        size_t cols() { return vector.cols(); }
        T dot(const ColVector<T> & b) { return vector.dot(b.vector);  }        
        size_t size() const { return vector.size(); }
        void fill(T v) { vector.fill(v); }
        T norm() { return vector.norm(); }
        T squaredNorm() { return vector.squaredNorm(); }

        ColVector<T>& eval() { vector.eval(); return *this;  }

        void normalize() {
            vector.normalize();
        }

        ColVector<T> normalized() { ColVector<T> r; r.vector = vector.normalized(); return r; }            
        ColVector<T>& head(ColVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
        ColVector<T>& tail(ColVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
        ColVector<T>& segment(ColVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
        ColVector<T>& LinSpaced(T num, T a, T b)
        {        
            vector = EigenColVector<T>::LinSpaced(num,a,b);
            return *this;
        }

        T operator()(size_t i) { return vector(i); }
        T operator[](size_t i) { return vector(i); }

        /*
        T inner_product(const ColVector<T> & v)
        { return vector.adjoint() * v.vector; }
        */

        ColVector<T> operator + (const ColVector<T>& b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector + b.vector;
            return r;
        }
        ColVector<T> operator - (const ColVector<T>& b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector - b.vector;
            return r;
        }    
        
        T operator * (const ColVector<T>& b) 
        { 
            return vector.dot(b.vector);        
        }

        
        ColVector<T> operator * (const T b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector * b;
            return r;
        }

        ColVector<T> operator / (const T b)  
        { 
            ColVector<T> r(*this);
            r.vector = r.vector / b;
            return r;
        }

        
        void aplus(const ColVector<T>& b) { vector += b.vector; }
        void aminus(const ColVector<T>& b) { vector -= b.vector; }
        void adot(const ColVector<T>& b) { vector.dot(b.vector); }
        
        void atimes(const T b) { vector = vector * b; }
        void adiv(const T b) { vector = vector / b; }


        ColVector<T> t() { return ColVector<T>(vector.transpose().eval());}    
        ColVector<T> abs() {  return ColVector<T>(vector.cwiseAbs());  }
        ColVector<T> inverse() { return ColVector<T>(vector.cwiseInverse()); }    
        ColVector<T> sqrt() { return ColVector<T>(vector.cwiseSqrt()); }
        ColVector<T> abs2() { return ColVector<T>(vector.cwiseAbs2()); }
        ColVector<T> operator * (ColMatrix<T> & m);

        ColVector<T> operator = (const ColVector<T> & v) { vector = v.vector; return *this; }
        ColVector<T> operator = (const EigenColVector<T> & v) { vector = v; return *this; }

        // add to each col
        ColMatrix<T> operator + (ColMatrix<T> & m);
        // subtract from each col 
        ColMatrix<T> operator - (ColMatrix<T> & m);
        // divide from each col
        ColMatrix<T> operator / (ColMatrix<T> & m);

        ColMatrix<T> matrix();    
    };

    
    template<typename T> ColVector<T>  abs(ColVector<T> & vector ) { ColVector<T>  r; r = vector.abs(); return r;}
    template<typename T> ColVector<T>  inverse(ColVector<T> & vector ) { ColVector<T>  r; r = vector.inverse(); return r; }
    template<typename T> ColVector<T>  exp(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().exp(); return r;  }
    template<typename T> ColVector<T>  log(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log(); return r;  }
    template<typename T> ColVector<T>  log1p(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log1p(); return r; }
    template<typename T> ColVector<T>  log10(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log10(); return r; }
    template<typename T> ColVector<T>  pow(ColVector<T> & vector ,const T b) { ColVector<T>  r; r = vector.vector.array().pow(b); return r;}
    template<typename T> ColVector<T>  sqrt(ColVector<T> & vector ) { ColVector<T>  r; r = vector.sqrt(); return r;}
    template<typename T> ColVector<T>  rsqrt(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().rsqrt(); return r; }
    template<typename T> ColVector<T>  square(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().square(); return r;}
    template<typename T> ColVector<T>  cube(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cube(); return r; }
    template<typename T> ColVector<T>  abs2(ColVector<T> & vector ) { ColVector<T>  r; r = vector.abs2(); return r; }
    template<typename T> ColVector<T>  sin(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().sin(); return r;}
    template<typename T> ColVector<T>  cos(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cos(); return r; }
    template<typename T> ColVector<T>  tan(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().tan(); return r;}
    template<typename T> ColVector<T>  asin(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().asin(); return r; }
    template<typename T> ColVector<T>  acos(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().acos(); return r; }
    template<typename T> ColVector<T>  atan(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().atan(); return r; }
    template<typename T> ColVector<T>  sinh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().sinh(); return r; }
    template<typename T> ColVector<T>  cosh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cosh(); return r; }
    template<typename T> ColVector<T>  tanh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().tanh(); return r; }
    template<typename T> ColVector<T>  ceil(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().ceil(); return r; }
    template<typename T> ColVector<T>  floor(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().floor(); return r; }
    template<typename T> ColVector<T>  round(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().round(); return r; }

    
    template<typename T>
    class ColMatrixView 
    {
    private:
        EigenColMatrix<T> *matrix;
        size_t row;
        size_t col;
        bool bUseRows;

    public:

        ColMatrixView(EigenColMatrix<T> *m, size_t r)
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
        void  __setitem(size_t i, const T val) { 
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
        EigenColMatrix<T> matrix;

        ColMatrix() {}
        ColMatrix(int x, int y) { matrix = EigenColMatrix<T>(x,y); }
        ColMatrix(int x, int y, const T val) { matrix = EigenColMatrix<T>(x,y); matrix.fill(val);  }
        ColMatrix(const ColVector<T> & v) { matrix = v.vector; }        
        ColMatrix(const ColMatrix<T>& m) { matrix = m.matrix;  }    
        ColMatrix(const EigenColMatrix<T>& m) { matrix = m; }    
        ColMatrix(const EigenColVector<T> & v) { matrix = v; }    
        ColMatrix(const EigenArray2D<T> & a) { matrix = a.matrix(); }
        ColMatrix(const std::vector<T> & data, size_t r, size_t c) {       
            matrix.resize(r,c);
            memcpy(matrix.data(),data.data(),data.size()*sizeof(T));        
        }

        T& operator()(size_t i, size_t j) { return matrix(i,j); }
        
        void random(int x, int y) { matrix =  EigenColMatrix<T>::Random(x,y); }
        void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
        void identity(int x, int y) { matrix = EigenColMatrix<T>::Identity(x,y); }
        void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
        void zero(int x, int y) { matrix = EigenColMatrix<T>::Zero(x,y); }
        void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
        void ones(int x, int y) { matrix = EigenColMatrix<T>::Ones(x,y); }
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
        
        ColVector<T> operator[](int index) { 
            if(index < 0) index += rows();
            EigenColMatrix<T> r = matrix.row(index);
            return ColVector<T>(r);        
        }
        void normalize()
        {
            matrix.normalize();
        }
        ColMatrix<T> normalized() { 
            ColMatrix<T> r; 
            r.matrix = matrix.normalized(); 
            return r; 
        }    

        void fill(T v) { matrix.fill(v); }
        ColMatrix<T> eval() { return ColMatrix<T>(matrix.eval()); }
        bool hasNaN() { return matrix.hasNaN(); }
        size_t innerSize() { return matrix.innerSize(); }
        size_t outerSize() { return matrix.outerSize(); }    
        bool isMuchSmallerThan(const ColMatrix<T>& n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
        bool isOnes() { return matrix.isOnes(); }
        bool isZero() { return matrix.isZero(); }
                
        ColVector<T> col_vector(size_t row) { ColVector<T> r; r.vector = matrix.col(row); return r; }    
        ColVector<T> col(size_t row) { 
            ColVector<T> r;        
            r.vector = matrix.col(row); 
            return r; 
        }        
        void set_row(size_t row, std::vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v[i];
        }
        void set_row_vector(size_t row, ColVector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v(i);
        }
        void set_row_matrix(size_t row, ColMatrix<T> & v, size_t src_row)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v.matrix(src_row,i);
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
        void set_col_matrix(size_t col, ColMatrix<T> & v, size_t row)    {
            assert(rows() == v.cols());
            assert(cols() == v.rows());
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v.matrix(row,i);

        }

        
        ColMatrix<T> leftCols(ColMatrix<T>& r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
        ColMatrix<T> middleCols(ColMatrix<T>& r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
        ColMatrix<T> rightCols(ColMatrix<T>& r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
        ColMatrix<T> topRows(ColMatrix<T>& r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
        ColMatrix<T> middleRows(ColMatrix<T>& r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
        ColMatrix<T> bottomRows(ColMatrix<T>& r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
        ColMatrix<T> topLeftCorner(ColMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
        ColMatrix<T> topRightCorner(ColMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

        
        ColMatrix<T> adjoint()  { ColMatrix<T> m; m.matrix = matrix.adjoint().eval(); return m; }
        ColMatrix<T> transpose() { ColMatrix<T> m; m.matrix = matrix.transpose().eval(); return m; }
        ColMatrix<T> diagonal() { ColMatrix<T> m; m.matrix = matrix.diagonal().eval(); return m; }        
        ColMatrix<T> reverse() { ColMatrix<T> m; m.matrix = matrix.reverse().eval(); return m; }    
        ColMatrix<T> replicate(size_t i, size_t j) { ColMatrix<T> m; m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        void adjointInPlace() {  matrix.adjointInPlace(); }    
        void transposeInPlace() { matrix.transposeInPlace(); }
        void reverseInPlace() { matrix.reverseInPlace(); }

        
        ColMatrix<T> adjointInto(ColMatrix<T> & m) { m.matrix = matrix.adjoint().eval(); return m; }    
        ColMatrix<T> transposeInto(ColMatrix<T> & m) { m.matrix = matrix.transpose().eval();return m; }    
        ColMatrix<T> diagonalInto(ColMatrix<T> & m) { m.matrix = matrix.diagonal().eval();return m; }
        ColMatrix<T> reverseInto(ColMatrix<T> & m) { m.matrix = matrix.reverse().eval();return m; }
        ColMatrix<T> replicateInto(ColMatrix<T> & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        /*
        ColMatrix<T> outer_product(ColMatrix<T> & m)
        {   return ColMatrix<T>(matrix * m.t().matrix); }
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
        ColMatrix<T> addToEachRow(ColMatrix<T> & v)    {
            ColMatrix<T> r(*this);        
            r.matrix = r.matrix.rowwise() + ColVector<T>(v.matrix).vector;
            return r;
        }
        
        ColMatrix<T> cwiseAbs()    {
            EigenColMatrix<T> r = matrix.cwiseAbs();
            return r;
        }
        ColMatrix<T> cwiseAbs2()    {
            EigenColMatrix<T> r = matrix.cwiseAbs2();
            return r;
        }
        ColMatrix<T> cwiseProduct(const ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseProduct(q.matrix); 
            return r;
        }
        ColMatrix<T> cwiseQuotient(const ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseQuotient(q.matrix); 
            return r;
        }
        ColMatrix<T> cwiseInverse()    {
            EigenColMatrix<T> r = matrix.cwiseInverse();
            return r;
        }
        ColMatrix<T> cwiseSqrt(ColMatrix<T>& out)    {
            EigenColMatrix<T> r = matrix.cwiseSqrt();
            return r;
        }
        ColMatrix<T> cwiseMax(ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;        
        }
        ColMatrix<T> cwiseMin(ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;
        }

        
        bool operator == (const ColMatrix<T> &m) {
            ColMatrix<T> r;
            auto x = (matrix == m.matrix);
            return x;
        }
        bool operator != (const ColMatrix<T> &m) {
            ColMatrix<T> r;
            auto x = (matrix != m.matrix);
            return x;
        }
        

        ColMatrix<T>& operator=(const ColMatrix<T> & m)    {        
            matrix = m.matrix.eval();                    
            return *this;
        }
        ColMatrix<T>& operator=(const EigenColMatrix<T> & m)    {        
            matrix = m;
            return *this;
        }
        ColMatrix<T> operator * (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix * m.matrix;
            return r;
        } 
        ColMatrix<T> operator + (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix + m.matrix;
            return r;
        }
        ColMatrix<T> operator - (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix - m.matrix;
            return r;
        }

        
        ColMatrix<T> operator - ()     {
            ColMatrix<T> r(*this);
            r.matrix = -matrix; 
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
        
        ColMatrix<T> operator * (const T f)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix * f;
            return r;
        }    
        ColMatrix<T> operator / (const T f)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix / f;
            return r;
        }
            
        void print()     {
            std::cout << matrix << std::endl;
        }
        
        ColMatrix<T> t()     {
            return transpose();
        }
        

    // slice 

        ColMatrix<T> slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
            if(last_r = -1) last_r = matrix.rows()-1;
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seq(first_c,last_c));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seq(first_c,last_c));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seqN(first_c,first_cn));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seqN(first_c,first_cn));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        void rowwise_add(ColVector<T> & r) {
            matrix.rowwise() += r.vector;
        }
        void rowwise_sub(ColVector<T> & r) {
            matrix.rowwise() -= r.vector;
        }
        void rowwise_mul(ColVector<T> & r) {
            matrix = (matrix.array().rowwise() * r.vector.array()).matrix();
        }
        void rowwise_div(ColVector<T> & r) {
            matrix = (matrix.array().rowwise() / r.vector.array()).matrix();
        }

        EigenArray2D<T> array() const { return matrix.array(); }
        
    };



    template<typename T> ColMatrix<T> abs(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().abs().matrix(); return r;}
    template<typename T> ColMatrix<T> inverse(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().inverse().matrix(); return r; }
    template<typename T> ColMatrix<T> exp(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().exp().matrix(); return r;  }
    template<typename T> ColMatrix<T> log(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log().matrix(); return r;  }
    template<typename T> ColMatrix<T> log1p(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log1p().matrix(); return r; }
    template<typename T> ColMatrix<T> log10(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log10().matrix(); return r; }
    template<typename T> ColMatrix<T> pow(const ColMatrix<T> & matrix,const T b) { ColMatrix<T> r; r.matrix = matrix.array().pow(b).matrix(); return r;}
    template<typename T> ColMatrix<T> sqrt(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sqrt().matrix(); return r;}
    template<typename T> ColMatrix<T> rsqrt(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().rsqrt().matrix(); return r; }
    template<typename T> ColMatrix<T> square(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().square().matrix(); return r;}
    template<typename T> ColMatrix<T> cube(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cube().matrix(); return r; }
    template<typename T> ColMatrix<T> abs2(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().abs2().matrix(); return r; }
    template<typename T> ColMatrix<T> sin(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sin().matrix(); return r;}
    template<typename T> ColMatrix<T> cos(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cos().matrix(); return r; }
    template<typename T> ColMatrix<T> tan(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().tan().matrix(); return r;}
    template<typename T> ColMatrix<T> asin(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().asin().matrix(); return r; }
    template<typename T> ColMatrix<T> acos(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().acos().matrix(); return r; }
    template<typename T> ColMatrix<T> atan(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().atan().matrix(); return r; }
    template<typename T> ColMatrix<T> sinh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sinh().matrix(); return r; }
    template<typename T> ColMatrix<T> cosh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cosh().matrix(); return r; }
    template<typename T> ColMatrix<T> tanh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().tanh().matrix(); return r; }
    template<typename T> ColMatrix<T> ceil(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().ceil().matrix(); return r; }
    template<typename T> ColMatrix<T> floor(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().floor().matrix(); return r; }
    template<typename T> ColMatrix<T> round(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().round().matrix(); return r; }
    template<typename T> ColMatrix<T> asinh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().asinh().matrix(); return r; }
    template<typename T> ColMatrix<T> acosh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().acosh().matrix(); return r; }
    template<typename T> ColMatrix<T> atanh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().atanh().matrix(); return r; }
    template<typename T> ColMatrix<T> rint(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().rint().matrix(); return r; }

    template<typename T> 
    ColVector<T> ColVector<T>::operator * (ColMatrix<T> &b) 
    { 
        ColVector<T> r;        
        r.vector =  b.matrix * vector;
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator + (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) + vector(j);
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator - (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) - vector(j);
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator / (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) / vector(j);
        return r;
    }
    
    template<typename T> ColMatrix<T> ColVector<T>::matrix() { return ColMatrix<T>(vector); }
