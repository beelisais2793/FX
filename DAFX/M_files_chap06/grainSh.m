function y = grainSh(x,init,L)
% Authors: G. De Poli
% extract a short grain
% x    input signal
% init first sample
% L    grain length (in samples)
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

y=x(init:init+L-1).*hanning(L)';