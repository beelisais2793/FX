% function [a,g] = calc_lpc(x,p) [DAFXbook, 2nd ed., chapter 8]
% ===== This function computes LPC coeffs via autocorrelation method
%       Similar to MATLAB function "lpc" 
% !!! IMPORTANT: function "lpc" does not work correctly with MATLAB 6!
% Inputs:
%   x: input signal
%   p: prediction order
% Outputs:
%   a: LPC coefficients
%   g: gain factor
% (c) 2002 Florian Keiler
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

function [a,g] = calc_lpc(x,p)

R = xcorr(x,p);       % autocorrelation sequence R(k) with k=-p,..,p
R(1:p) = [];          % delete entries for k=-p,..,-1
if norm(R)~=0
   a = levinson(R,p);    % Levinson-Durbin recursion
%   a = [1, -a_1, -a_2,..., -a_p]
else
    a = [1, zeros(1,p)];
end    
R = R(:)'; a = a(:)';   % row vectors
g = sqrt(sum(a.*R));  % gain factor
