% function cep = discrete_cepstrum_reg(F, A, order, lambda)
% [DAFXbook, 2nd ed., chapter 8]
% ==== This function computes the discrete spectrum using a 
%      regularization function
% Inputs:
%   - A [1*L | float]       harmonic partial amplitudes
%   - F [1*L | float]       harmonic partial frequencies
%   - order [1*1 | int]     number of cepstral coefficients
%   - lambda [1*2 | float]  weighting of the perturbation, in [0,1[
% Output:
%   - cep [1*order | float] cepstral coefficients
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

function cep = discrete_cepstrum_reg(F, A, order, lambda)

%---- reject incorrect lambda values
if lambda>=1 | lambda <0 
  disp('Error: lambda must be in [0,1[')
  cep = [];
  return;
end

%---- initialize matrices and vectors
L = length(A);
M = zeros(L,order+1);
R = zeros(order+1,1);
for i=1:L
  M(i,1) = 1;
  for k=2:order+1
    M(i,k) = 2 * cos(2*pi*(k-1)*F(i));
  end
end

%---- initialize the R vector values
coef = 8*(pi^2);
for k=1:order+1
  R(k,1) = coef * (k-1)^2;
end

%---- compute the solution
Mt = transpose(M);
MtMR = Mt*M + (lambda/(1.-lambda))*diag(R);
cep = inv(MtMR) * Mt*log(A);
