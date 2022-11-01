% function cep = discrete_cepstrum_basic(F, A, order)
% [DAFXbook, 2nd ed., chapter 8]
% ==== This function computes the discrete spectrum regardless of 
%      matrix conditionning and singularity.
% Inputs:
%   - A [1*L | float]        harmonic partial amplitudes
%   - F [1*L | float]        harmonic partial frequencies
%   - order [1*1 | int]      number of cepstral coefficients
% Outputs:
%   - cep [1xorder | float]  cepstral coefficients
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

function cep = discrete_cepstrum_basic(F, A, order)

%---- initialize matrices and vectors
L = length(A);
M = zeros(L,order+1);
M = zeros(L,L);
R = zeros(order+1,1);
W = zeros(L,L);

for i=1:L
  M(i,1) = 0.5;
  for k=2:order+1
    M(i,k) = cos(2*pi*(k-1)*F(i));
  end
  W(i,i) = 1; % weights = 1 by default
end
M = 2.*M;

%---- compute the solution, regardless of matric conditioning
Mt = transpose(M);
MtWMR = Mt * W * M;
cep = inv(MtWMR) * Mt * W * log(A);