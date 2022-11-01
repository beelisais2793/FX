%module vector
%{
#include <Eigen/Eigen>
%}

%include "eigen-vector.i"

%template(VectorXf) Eigen::Matrix<float,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(RowVectorXf) Eigen::Matrix<float,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(ColVectorXf) Eigen::Matrix<float,Eigen::Dynamic,1,Eigen::ColMajor>;

%template(VectorXf) Eigen::Matrix<double,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(RowVectorXf) Eigen::Matrix<double,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(ColVectorXf) Eigen::Matrix<double,Eigen::Dynamic,1,Eigen::ColMajor>;

%template(VectorXi) Eigen::Matrix<int,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(RowVectorXi) Eigen::Matrix<int,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(ColVectorXi) Eigen::Matrix<int,Eigen::Dynamic,1,Eigen::ColMajor>;

%template(VectorXl) Eigen::Matrix<long,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(RowVectorXl) Eigen::Matrix<long,1,Eigen::Dynamic,Eigen::RowMajor>;
%template(ColVectorXl) Eigen::Matrix<long,Eigen::Dynamic,1,Eigen::ColMajor>;

%template(absf) abs<float>;
%template(inversef) inverse<float>;
%template(logf) log<float>;
%template(log1p) log1p<float>;
%template(log10) log10<float>;
%template(powf) pow<float>;
%template(sqrtf) sqrt<float>;
%template(rsqrtf) rsqrt<float>;
%template(squaref) square<float>;
%template(cubef) cube<float>;
%template(abs2f) abs2<float>;
%template(sinf) sin<float>;
%template(cosf) cos<float>;
%template(tanf) tan<float>;
%template(asinf) asin<float>;
%template(acosf) acos<float>;
%template(atanf) atan<float>;
%template(sinhf) sinh<float>;
%template(coshf) cosh<float>;
%template(tanhf) tanh<float>;
%template(asinhf) asinh<float>;
%template(acoshf) acosh<float>;
%template(atanhf) atanh<float>;
%template(ceilf) ceil<float>;
%template(floorf) floor<float>;
%template(roundf) round<float>;
%template(rintf) rint<float>;
%template(randomf) random<float>;
%template(zerof) zero<float>;
%template(onesf) ones<float>;
%template(innerStridef) innerStride<float>;
%template(resizef) resize<float>;
%template(resizeLikef) resizeLike<float>;
%template(conservativeResizef) conservativeResize<float>;
%template(coefff) coeff<float>;
%template(coeffReff) coefReff<float>;
%template(printlnf) print<float>;
%template(colsf) cols<float>;
%template(dotf) dot<float>;
%template(sizef) size<float>;
%template(fillf) fill<float>;
%template(normf) norm<float>;
%template(squaredNormf) squaredNorm<float>;
%template(evalf) eval<float>;
%template(normalizef) normalize<float>;
%template(normalizedf) normalized<float>;
%template(cwiseAbsf) cwiseAbs<float>;
%template(cwiseInversef) cwiseInverse<float>;
%template(cwiseSqrtf) cwiseSqrt<float>;
%template(cwiseAbs2f) cwiseAbs2<float>;
