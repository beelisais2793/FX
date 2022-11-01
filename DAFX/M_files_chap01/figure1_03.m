% Author: U. Zölzer
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,FS,NBITS]=wavread('ton2.wav');

figure(1)
subplot(3,1,1);
plot(0:7999,x(1:8000));ylabel('x(n)');
subplot(3,1,2);
plot(0:999,x(1:1000));ylabel('x(n)');
subplot(3,1,3);
stem(0:99,x(1:100),'.');ylabel('x(n)');
xlabel('n \rightarrow');