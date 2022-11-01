function y = apbandpass (x, Wc, Wb)
% y = apbandpass (x, Wc, Wb)
% Author: M. Holters
% Applies a bandpass filter to the input signal x.
% Wc is the normalized center frequency 0<Wc<1, i.e. 2*fc/fS.
% Wb is the normalized bandwidth 0<Wb<1, i.e. 2*fb/fS.
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

c = (tan(pi*Wb/2)-1) / (tan(pi*Wb/2)+1);
d = -cos(pi*Wc);
xh = [0, 0];
for n=1:length(x)
  xh_new = x(n) - d*(1-c)*xh(1) + c*xh(2);
  ap_y = -c * xh_new + d*(1-c)*xh(1) + xh(2);
  xh = [xh_new, xh(1)];
  y(n) = 0.5 * (x(n) - ap_y);  % change to plus for bandreject
end;