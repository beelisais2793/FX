%POLYROOTS Find polynomial roots using Jenkins-Traub method.
%   POLYROOTS(C) computes the roots of the polynomial whose coefficients
%   are the elements of the vector C in order of decreasing powers. If C
%   has N+1 components, the polynomial is C(1)*X^N + ... + C(N)*X + C(N+1).
%   Leading zeros in C are discarded.
%
%   Class support for input c: 
%      float: double
%
%   See also ROOTS.
%
%   References:
%       - Jenkins, M. A. and Traub, J. F. (1972), Algorithm 419: Zeros of a
%         Complex Polynomial, Comm. ACM, 15, 97–99. 
%       - Jenkins, M. A. (1975), Algorithm 493: Zeros of a Real Polynomial, 
%         ACM TOMS, 1, 178–189.
