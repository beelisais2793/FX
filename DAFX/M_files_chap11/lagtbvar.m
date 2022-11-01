function y=lagtbvar(x,b,M)
% Author: G. Evangelista
% computes coefficients y of biorthogonal Laguerre expansion of x
% using variable parameters b(k) where b is a length M array
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

N=length(x);
yy=x(N:-1:1);                 % time reverse input
y=zeros(1,M);
yy=filter(1,[1, b(1)],yy);    % filter by psi_0(z)
y(1)=yy(N);                   % retain the last sample only
% filter by H_1(z)(unscaled, b to -b)
yy=filter([0,1],[1, b(2)],yy);
y(2)=yy(N)*(1-b(1)*b(2));     % retain the last sample only and scale
for k=3:M
 % filter by H_(k-1)(z)(unscaled, b to -b)
 yy=filter([b(k-2),1],[1, b(k)],yy);
 y(k)=yy(N)*(1-b(k-1)*b(k));  % retain the last sample only and scale
end