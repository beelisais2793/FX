%-----frequency stretch-----%
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

fstretch = 1.1;
ysloc = ysloc .* (fstretch.^[0:length(ysloc)-1]');