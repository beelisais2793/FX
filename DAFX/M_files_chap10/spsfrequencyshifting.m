% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

%-----frequency shift-----%
fshift = 100;
ysloc = (ysloc>0).*(ysloc + fshift/fs*Ns); % frequency shift in Hz