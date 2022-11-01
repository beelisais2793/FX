function y=lagt(x,b,M)
% Author: G. Evangelista
% computes M terms of the Laguerre transform y of the input x
% with Laguerre parameter b
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

N=length(x);
x=x(N:-1:1);     % time reverse input
% filter by normalizing filter lambda_0
yy=filter(sqrt(1-b^2),[1,b],x);
y(1)=yy(N);      % retain the last sample only
for k=2:M
% filter the previous output by allpass
 yy=filter([b,1],[1,b],yy);
 y(k)=yy(N);     % retain the last sample only
end