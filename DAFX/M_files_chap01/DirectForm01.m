% Author: U. Zölzer
% Impulse response of 2nd order IIR filter
% Sample-by-sample algorithm
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear

% Coefficients for a high-pass
a=[1, -1.28, 0.47];
b=[0.69, -1.38, 0.69];

% Initialization of state variables
xh1=0;xh2=0;
yh1=0;yh2=0;

% Input signal: unit impulse
N=20;  % length of input signal
x(N)=0;x(1)=1;

% Sample-by-sample algorithm
for n=1:N
y(n)=b(1)*x(n) + b(2)*xh1 + b(3)*xh2 - a(2)*yh1 - a(3)*yh2;
xh2=xh1;xh1=x(n);
yh2=yh1;yh1=y(n);
end;

% Plot results
subplot(2,1,1)
stem(0:1:length(x)-1,x,'.');axis([-0.6 length(x)-1 -1.2 1.2]);
xlabel('n \rightarrow');ylabel('x(n) \rightarrow');
subplot(2,1,2)
stem(0:1:length(x)-1,y,'.');axis([-0.6 length(x)-1 -1.2 1.2]);
xlabel('n \rightarrow');ylabel('y(n) \rightarrow');