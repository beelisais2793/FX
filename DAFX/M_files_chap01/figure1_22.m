%Author: U. Zölzer
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[signal,FS,NBITS]=wavread('ton2');
subplot(211);plot(signal);
title('Signal x(n)');
ylabel('x(n) \rightarrow');
xlabel('n \rightarrow');
axis([0 length(signal) -1 1]);
subplot(212);
waterfspec(signal,256,256,512,FS,20,-100);