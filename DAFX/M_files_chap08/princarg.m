function phase = princarg(phase_in)
% This function puts an arbitrary phase value into ]-pi,pi] [rad]
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

phase = mod(phase_in+pi,-2*pi) + pi;
