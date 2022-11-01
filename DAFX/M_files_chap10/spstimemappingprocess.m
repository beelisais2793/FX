%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

poutend = outsoundlength-max(hM,H);             % last sample to start a frame
pout = pin;
minpin = max(H+1,1+hM);
maxpin = min(length(x)-hM-1);
fridx = 0;
while pout<poutend
  pin = round( interp1(TM(2,:),TM(1,:),pout/fs,'linear','extrap') * fs );
  pin = max(minpin,pin);
  pin = min(maxpin,pin);
  %-----analysis-----%
  (...)
  ro = pout-hNs;                            % output sound pointer for overlap
  yh(ro:ro+Ns-1) = yh(ro:ro+Ns-1)+yhw(1:Ns).*sw;  % overlap-add for sines
  ys(ro:ro+Ns-1) = ys(ro:ro+Ns-1)+ysw(1:Ns).*sws; % overlap-add for stochastic
  pout = pout+H;                                  % advance the sound pointer	
  fridx = fridx+1;
end
y= yh+ys;                                         % sum sines and stochastic