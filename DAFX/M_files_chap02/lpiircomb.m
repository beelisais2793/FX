% Authors: P.Dutilleux, U. Zölzer
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

x=zeros(100,1);x(1)=1; % unit impulse signal of length 100
g=0.5;
b_0=0.5;
b_1=0.5;
a_1=0.7;
xhold=0;yhold=0;
Delayline=zeros(10,1); % memory allocation for length 10
for n=1:length(x);
    yh(n)=b_0*Delayline(10)+b_1*xhold-a_1*yhold; 
    % 1st-order difference equation
    yhold=yh(n);
    xhhold=Delayline(10);
    y(n)=x(n)+g*yh(n);
    Delayline=[y(n);Delayline(1:10-1)];
end;