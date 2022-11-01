#pragma once 
namespace SimpleEigen {
// all the cool kids call it the hadamard product.
template<typename T>
Matrix<T> hadamard(Matrix<T> & a, Matrix<T> &b)
{
    return Matrix<T>(a.cwiseProduct(b.matrix));
}

/// Neural Network functions

template<typename T>
void sigmoid(Matrix<T> & m)
{       
    
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> t = m.matrix.array();
    t = -t;
    m.matrix = (1 / (1 + t.exp())).matrix();
}

template<typename T>
void sigmoidd(Matrix<T> & m)
{ 
    
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> t = m.matrix.array();
    m.matrix = (t * ( 1 - t )).matrix();
}

template<typename T>
void tanH(Matrix<T> & m)
{   
    
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> t = m.matrix.array();  
    m.matrix = (t.tanh()).matrix();
}

template<typename T>
void tanHd(Matrix<T> & m)
{   
    
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> t = m.matrix.array();
    m.matrix = (1 - (t*t)).matrix();
}

template<typename T>
void relu(Matrix<T> & m)
{                  
    m.matrix = m.matrix.cwiseMax(0).eval();    
}

template<typename T>
void relud(Matrix<T> & m)
{          
    for(size_t i = 0; i < m.rows(); i++)
        for(size_t j = 0; j < m.cols(); j++)
        {
            T x = m.matrix(i,j);
            if(x > FLT_MIN) m.matrix(i,j) = 1;
            else m.matrix(i,j) = 0;
        }
}    

template<typename T>
void softmax(Matrix<T> & m)
{        
    T summed = m.matrix.sum();
    Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic> t = m.matrix.array();        
    m.matrix = (t.exp() / summed).matrix();
}


// Identity or Linear
template<typename T>
void noalias(Matrix<T> & m, Matrix<T> & rhs)  
{
    m.matrix.noalias() = rhs.matrix;
}

template<typename T>
void mish_activate(const Matrix<T> & Z, Matrix<T> & A)
{
    // h(x) = tanh(softplus(x)) = (1 + exp(x))^2 - 1
    //                            ------------------
    //                            (1 + exp(x))^2 + 1
    // Let s = exp(-abs(x)), t = 1 + s
    // If x >= 0, then h(x) = (t^2 - s^2) / (t^2 + s^2)
    // If x <= 0, then h(x) = (t^2 - 1) / (t^2 + 1)
    Matrix<T> S;
    S.matrix = ((-Z.matrix.array().abs()).exp().matrix());
    A.matrix.array() = (S.matrix.array() + (T)1).square();
    S.matrix.noalias() = (Z.matrix.array() >= (T)0).select(S.matrix.cwiseAbs2(),(T)1);
    A.matrix.array() = (A.matrix.array() - S.matrix.array()) /
                            (A.matrix.array() + S.matrix.array());
    A.matrix.array() *= Z.matrix.array();                                
}

template<typename T>
void mish_apply_jacobian(const Matrix<T>& Z, const Matrix<T>& A,
                                        const Matrix<T>& F, Matrix<T>& G)
    {
        // Let h(x) = tanh(softplus(x))
        // Mish'(x) = h(x) + x * h'(x)
        // h'(x) = tanh'(softplus(x)) * softplus'(x)
        //       = [1 - h(x)^2] * exp(x) / (1 + exp(x))
        //       = [1 - h(x)^2] / (1 + exp(-x))
        // Mish'(x) = h(x) + [x - Mish(x) * h(x)] / (1 + exp(-x))
        // A = Mish(Z) = Z .* h(Z) => h(Z) = A ./ Z, h(0) = 0.6
        G.matrix.noalias() = (Z.matrix.array() == (T)0).select((T)0.6, A.matrix.cwiseQuotient(Z.matrix));
        G.matrix.array() += (Z.matrix.array() - A.matrix.array() * G.matrix.array()) / ((T)1 + (-Z.matrix).array().exp());
        G.matrix.array() *= F.matrix.array();
    }
}