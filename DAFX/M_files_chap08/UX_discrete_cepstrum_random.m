% function cep = discrete_cepstrum_random(F, A, order, lambda, Nrand, dev)
% [DAFXbook, 2nd ed., chapter 8]
% ==== This function computes the discrete spectrum using multiples of each
%      peak to which a small random amount is added, for better smoothing.
% Inputs:
%   - A [1*L | float]        harmonic partial amplitudes
%   - F [1*L | float]        harmonic partial frequencies
%   - order [1*1 | int]      number of cepstral coefficients
%   - lambda [1v2 | float]   weighting of the perturbation, in [0,1[
%   - Nrand [1*1 | int]      nb of random points generated per (Ai,Fi) pair
%   - dev  1*2 | float]     deviation of random points, with Gaussian
% Outputs:
%   - cep [1*order |â float] cepstral coefficients
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

function cep = discrete_cepstrum_random(F, A, order, lambda, Nrand, dev)

if lambda>=1 | lambda <0 % reject incorrect lambda values
  disp('Error: lambda must be in [0,1[')
  cep = [];
  return;
end

%---- generate random points ----
L = length(A);
new_A = zeros(L*Nrand,1);
new_F = zeros(L*Nrand,1);
for k=1:L
  sigA = dev * A(k);
  sigF = dev * F(k);
  for l=1:L
    new_A((l-1)*Nrand+1) = A(l);
    new_F((l-1)*Nrand+1) = F(l);
    for n=1:Nrand
      new_A((l-1)*Nrand+n+1) = random('norm', A(l), sigA);
      new_F((l-1)*Nrand+n+1) = random('norm', F(l), sigF);
    end
  end
end

%---- initialize matrices and vectors
L = length(new_A);
M = zeros(L,order+1);
R = zeros(order+1,1);
for i=1:L
  M(i,1) = 1;
  for k=2:order+1
    M(i,k) = 2 * cos(2*pi*(k-1)*new_F(i));
  end
end
%---- initialize the R vector values
coef = 8*(pi^2);
for k=1:order+1,
  R(k,1) = coef * (k-1)^2;
end
%---- compute the solution
Mt = transpose(M);
MtMR = Mt*M + (lambda/(1.-lambda))*diag(R);
cep = inv(MtMR) * Mt*log(new_A);