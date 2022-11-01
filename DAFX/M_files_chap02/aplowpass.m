function y = aplowpass (x, Wc)
% y = aplowpass (x, Wc)
% Author: M. Holters
% Applies a lowpass filter to the input signal x.
% Wc is the normalized cut-off frequency 0<Wc<1, i.e. 2*fc/fS.
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

c = (tan(pi*Wc/2)-1) / (tan(pi*Wc/2)+1);
xh = 0;
for n=1:length(x)
  xh_new = x(n) - c*xh;
  ap_y = c * xh_new + xh;
  xh = xh_new;
  y(n) = 0.5 * (x(n) + ap_y);  % change to minus for highpass
end;