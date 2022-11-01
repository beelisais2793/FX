% Author: V. Pulkki, T. Lokki
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

% Create an impulse
x = zeros(1,2500); x(1) = 1;

% Delay line and read position
A = zeros(1,100);
Adelay=40;

% Output vector
ir = zeros(1,2500);

% Feedback gain
g=0.7;

% Comb-allpass filtering
for n = 1:length(ir)
    tmp = A(Adelay) + x(n)*(-g);
    A = [(tmp*g + x(n))' A(1:length(A)-1)];
    ir(n) = tmp;
end

% Plot the filtering result
plot(ir)