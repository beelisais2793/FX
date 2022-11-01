% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

%----- filtering -----%
Filter=[    0  2099 2100 3000 3001 fs/2;   % Hz
         -200  -200    0    0 -200 -200 ]; % db
ysmag = ysmag+interp1(Filter(1,:),Filter(2,:),ysloc/Ns*fs);