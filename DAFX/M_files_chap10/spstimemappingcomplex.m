%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

tm = 0.01:0.01:.93;
timemapping = [  0 tm+0.05*sin(8.6*pi*tm) 1 ;  % input time --> keep end value
                 0 tm                     1 ]; % output time
timemapping = timemapping*length(x)/fs;     