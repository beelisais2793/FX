function y = lowshelving (x, Wc, G)
% y = lowshelving (x, Wc, G)
% Author: M. Holters
% Applies a low-frequency shelving filter to the input signal x.
% Wc is the normalized cut-off frequency 0<Wc<1, i.e. 2*fc/fS
% G is the gain in dB
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

V0 = 10^(G/20); H0 = V0 - 1;
if G >= 0
  c = (tan(pi*Wc/2)-1) / (tan(pi*Wc/2)+1);     % boost
else
  c = (tan(pi*Wc/2)-V0) / (tan(pi*Wc/2)+V0);   % cut
end;
xh = 0;
for n=1:length(x)
  xh_new = x(n) - c*xh;
  ap_y = c * xh_new + xh;
  xh = xh_new;
  y(n) = 0.5 * H0 * (x(n) + ap_y) + x(n);  % change to minus for HS
end;