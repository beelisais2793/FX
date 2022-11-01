% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

yinws = round(fs*0.015);           % using approx. a 15 ms window for yin
yinws = yinws+mod(yinws,2);        % make it even
yb = pin-yinws/2;
ye = pin+yinws/2+yinws;
if (yb<1 || ye>length(x))          % out of boundaries
   f0 = 0; 
else
   f0 = f0detectionyin(x(yb:ye),fs,yinws,minf0,maxf0); 
end