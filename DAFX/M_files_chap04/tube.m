function y=tube(x, gain, Q, dist, rh, rl, mix)
% function y=tube(x, gain, Q, dist, rh, rl, mix)
% Author: Bendiksen, Dutilleux, Zölzer

% y=tube(x, gain, Q, dist, rh, rl, mix)
% "Tube distortion" simulation, asymmetrical function
% x    - input
% gain - the amount of distortion, >0->
% Q    - work point. Controls the linearity of the transfer
%	     function for low input levels, more negative=more linear
% dist - controls the distortion's character, a higher number gives 
%	     a harder distortion, >0
% rh   - abs(rh)<1, but close to 1. Placement of poles in the HP 
%	     filter which removes the DC component
% rl   - 0<rl<1. The pole placement in the LP filter used to 
%	     simulate capacitances in a tube amplifier
% mix  - mix of original and distorted sound, 1=only distorted
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

q=x*gain/max(abs(x));			%Normalization
if Q==0
  z=q./(1-exp(-dist*q));
  for i=1:length(q)			%Test because of the
    if q(i)==Q				%transfer function's
      z(i)=1/dist;			%0/0 value in Q
    end;
  end;
else
  z=(q-Q)./(1-exp(-dist*(q-Q)))+Q/(1-exp(dist*Q));
  for i=1:length(q)				%Test because of the
    if q(i)==Q					%transfer function's
      z(i)=1/dist+Q/(1-exp(dist*Q));	%0/0 value in Q
    end;
  end;
end;
y=mix*z*max(abs(x))/max(abs(z))+(1-mix)*x;
y=y*max(abs(x))/max(abs(y));			
y=filter([1 -2 1],[1 -2*rh rh^2],y);	%HP filter
y=filter([1-rl],[1 -rl],y);			    %LP filter