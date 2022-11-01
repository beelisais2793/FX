function y = genbh92lobe(x)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Calculate transform of the Blackman-Harris 92dB window
% x: bin positions to compute (real values)
% y: transform values
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

N = 512;
f = x*pi*2/N;                                          % frequency sampling
df = 2*pi/N;
y = zeros(size(x));                                    % initialize window
consts = [.35875, .48829, .14128, .01168];             % window constants
for m=0:3
    y = y + consts(m+1)/2*(D(f-df*m,N)+D(f+df*m,N));   % sum Dirichlet kernels
end
y = y/N/consts(1);                                     % normalize
end

function y = D(x,N)
% Calculate rectangular window transform (Dirichlet kernel)
y = sin(N*x/2)./sin(x/2);
y(find(y~=y))=N;                                       % avoid NaN if x==0
end