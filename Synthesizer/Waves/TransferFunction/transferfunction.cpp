#include "transferfunction.h"

TransferFunction::TransferFunction(double a, double b, double c, double d, double x0, bool doPrint):
    initialValue(x0)
{
    A = matrix<double>(1, 1);
    A(0, 0) = a;

    B = matrix<double>(1, 1);
    B(0, 0) = b;

    C = matrix<double>(1, 1);
    C(0, 0) = c;

    D = matrix<double>(1, 1);
    D(0, 0) = d;

    init(doPrint);
}


TransferFunction::TransferFunction(matrix<double> a, matrix<double> b, matrix<double> c, matrix<double> d, double x0, bool doPrint)
    :A(a), B(b), C(c), D(d), initialValue(x0)
{
    init(doPrint);
}

double TransferFunction::computeNextOutput(double nextInput)
{
    // Рассчеты
    xout = prod(Ad, xout) + Bd0 * prevInput + Bd1 * nextInput;
    double y = (prod(C, xout) + D * nextInput)(0, 0);
    prevInput = nextInput;
    return y;
}

void TransferFunction::init(bool doPrint)
{
    calc_matrix();

    if (doPrint)
    {
        print("A", A);
        print("B", B);
        print("C", C);
        print("D", D);
    }
}

void TransferFunction::calc_matrix()
{
    prevInput = initialValue;

    int n_states = A.size1();
    int n_inputs = B.size2();
    int n_outputs = C.size2();
    int n_steps = 1;

    xout.resize(n_states, n_steps);
    for (size_t i = 0; i < xout.size1(); i++)
        for (size_t j = 0; j < xout.size2(); j++)
            xout(i, j) = 0;

    double dt = 1; // ???

    M.resize(B.size1()+n_inputs*2, A.size2()+B.size2()+n_inputs);
    // 1,1 -> 3,3
    for (size_t i = 0; i < A.size1(); i++)
        for (size_t j = 0; j < A.size2(); j++)
            M(i, j) = A(i, j) * dt; 

    // 1,4 -> 3,4
    for (size_t i = 0; i < B.size1(); i++)
        M(i, A.size1()) = B(i, 0);

    // 1,5 -> 3,5 
    for (size_t i = 0; i < n_states; i++)
        M(i, n_states + n_inputs) = 0;

    // 3,1 -> 3,4
    for (size_t i = 0; i < n_states + n_inputs; i++)
        M(n_states, i) = 0;

    // 4,5
    for (size_t i = n_states + n_inputs; i < n_states + 2*n_inputs; i++)
        M(n_states, i) = 1;

    for (size_t i = 0; i < n_states + 2 * n_inputs; i++)
        M(n_states + 1, i) = 0;


    expM = expm(M, 1.0, 6);
    
    Ad.resize(n_states, n_states);
    for (size_t i = 0; i < Ad.size1(); i++)
        for (size_t j = 0; j < Ad.size2(); j++)
            Ad(i, j) = expM(i, j);
    
    Bd1.resize(n_states, 1);
    for (size_t i = 0; i < Bd1.size1(); i++)
        Bd1(i, 0) = expM(i, n_states + n_inputs);

    Bd0.resize(n_states, 1);
    for (size_t i = 0; i < Bd0.size1(); i++)
        Bd0(i, 0) = expM(i, n_states);
    Bd0 -= Bd1;

}

void TransferFunction::print(QString name, matrix<double>& M)
{
    int width = 10;
    int size1 = M.size1();
    int size2 = M.size2();
    qDebug() << name << size1 << "x" << size2;
    
    if (name == "Simulation")
    {
        std::cout << "_____________________________________" << "\n"
                  << "|" << std::setw(width + 1) << "input:  "
                  << "|" << std::setw(width + 1) << "Output: "
                  << "|" << std::setw(width + 1) << "Time:   "
                  << "|" << std::endl;
    }
    

    for (auto j = 0; j < size2*(width+2); j++) { std::cout << "_"; }
    std::cout << "_\n";
    for (auto i = 0; i < size1; i++)
    {
        for (auto j = 0; j < size2; j++)
            std::cout << std::setprecision(5) << "|" << std::setw(width) << M(i, j) << " ";
        std::cout << "|\n";
        for (auto j = 0; j < size2; j++) { std::cout << "|___________"; }
        std::cout << "|\n";
    }
    std::cout << "\n";
}

void TransferFunction::print(QString name, vector<double>& V)
{
    matrix<double> M(V.size(), 1);

    column(M, 0) = V;
    print(name, M);
}

bool TransferFunction::invertMatrix(const matrix<double>& input, matrix<double>& inverse)
{
    typedef permutation_matrix<std::size_t> pmatrix;
    // create a working copy of the input
    matrix<double> A(input);
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());
    // perform LU-factorization
    int res = lu_factorize(A, pm);
    if (res != 0) return false;
    // create identity matrix of "inverse"
    inverse.assign(identity_matrix<double>(A.size1()));
    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);
    return true;
}

matrix<double> TransferFunction::expm(const matrix<double> H, double t, const int p)
{
    typedef typename matrix<double>::value_type value_type;
    typedef typename matrix<double>::size_type size_type;
    typedef typename type_traits<value_type>::real_type real_value_type;
    assert(H.size1() == H.size2());
    assert(p >= 1);
    const size_type n = H.size1();
    const identity_matrix<value_type> I(n);
    matrix<value_type> U(n, n), H2(n, n), P(n, n), Q(n, n);
    real_value_type norm = 0.0;
    // Calcuate Pade coefficients
    vector<real_value_type> c(p + 1);
    c(0) = 1;
    for (size_type i = 0; i < p; ++i)
        c(i + 1) = c(i) * ((p - i) / ((i + 1.0) * (2.0 * p - i)));
    // Calcuate the infinty norm of H, which is defined as the largest row sum of a matrix
    for (size_type i = 0; i < n; ++i)
    {
        real_value_type temp = 0.0;
        for (size_type j = 0; j < n; j++)
            temp += std::abs(H(i, j));
        norm = t * std::max<real_value_type>(norm, temp);
    }
    // If norm = 0, and all H elements are not NaN or infinity but zero,
    // then U should be identity.
    if (norm == 0.0)
    {
        bool all_H_are_zero = true;
        for (size_type i = 0; i < n; i++)
            for (size_type j = 0; j < n; j++)
                if (H(i, j) != value_type(0.0))
                    all_H_are_zero = false;
        if (all_H_are_zero == true) return I;
        // Some error happens, H has elements which are NaN or infinity.
        std::cerr << "Null input error in the template expm_pad.\n";
        std::cout << "Null INPUT : " << "\n";
        exit(0);
    }
    // Scaling, seek s such that || H*2^(-s) || < 1/2, and set scale = 2^(-s) 
    int s = 0; real_value_type scale = 1.0;
    if (norm > 0.5)
    {
        s = std::max(0, static_cast<int>((log(norm) / log(2.0) + 2.0)));
        scale /= real_value_type(std::pow(2.0, s));
        U.assign((scale * t) * H); // Here U is used as temp value due to that H is const
    }
    else
        U.assign(H);

    // Horner evaluation of the irreducible fraction, see the following ref above.
    // Initialise P (numerator) and Q (denominator)
    H2.assign(prod(U, U));
    Q.assign(c(p) * I);
    P.assign(c(p - 1) * I);
    size_type odd = 1;
    for (size_type k = p - 1; k > 0; --k) {
        (odd == 1) ?
            (Q = (prod(Q, H2) + c(k - 1) * I)) :
            (P = (prod(P, H2) + c(k - 1) * I));
        odd = 1 - odd;
    }
    (odd == 1) ? (Q = prod(Q, U)) : (P = prod(P, U));
    Q -= P;
    // In origine expokit package, they use lapack ZGESV to obtain inverse matrix,
    // and in that ZGESV routine, it uses LU decomposition for obtaing inverse matrix.
    // Since in ublas, there is no matrix inversion template, I simply use the build-in
    // LU decompostion package in ublas, and back substitute by myself.

    // Implement Matrix Inversion
    permutation_matrix<size_type> pm(n);
    int res = lu_factorize(Q, pm);
    if (res != 0) {
        std::cerr << "Matrix inversion error in the template expm_pad.\n";
        exit(0);
    }
    // H2 is not needed anymore, so it is temporary used as identity matrix for substituting.
    H2.assign(I);
    lu_substitute(Q, pm, H2);
    (odd == 1) ?
        (U.assign(-(I + real_value_type(2.0) * prod(H2, P)))) :
        (U.assign(I + real_value_type(2.0) * prod(H2, P)));
    // Squaring
    for (size_type i = 0; i < s; ++i)
        U = (prod(U, U));
    return U;
}
       
matrix<double> TransferFunction::simulate(const vector<double> &U, double T)
{
    const int steps = U.size();

    matrix<double> output(steps, 3);
    vector<double> time_vector(steps);
    double dt = T / steps;

    time_vector(0) = 0;
    for (size_t i = 1; i < steps; i++)
        time_vector(i) = time_vector(i - 1) + dt;


    for (size_t i = 0; i < steps; i++)
    {
        output(i, 0) = U(i);
        output(i, 1) = computeNextOutput(U(i));
        output(i, 2) = time_vector(i);
    }

    print("Simulation", output);
    
    return output;
}
