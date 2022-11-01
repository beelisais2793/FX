function y = limiter(x, lt)
% function y = limiter(x, lt)
% Author: M. Holters
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

at = 0.3;
rt = 0.01;
delay = 5;

xpeak = 0;
g = 1;
buffer = zeros(1,delay);

for n = 1:length(x)
  a = abs(x(n));
  if a > xpeak
    coeff = at;
  else
    coeff = rt;
  end;
  xpeak = (1-coeff) * xpeak + coeff * a;
  f = min(1, lt/xpeak);
  if f < g
    coeff = at;
  else
    coeff = rt;
  end;
  g = (1-coeff) * g + coeff * f;
  y(n) = g * buffer(end);
  buffer = [x(n) buffer(1:end-1)];
end;