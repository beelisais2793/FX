% shape invariance: 
% assume pulse onsets to match zero phase of the fundamental
% and ideal harmonic distribution
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

pos = mod(hphase(1),2*pi)/2/pi;         % input normalized period position
ypos = mod(yhphase(1),2*pi)/2/pi;       % output normalized period position
yhphase = hphase + (ypos-pos)*2*pi*[1:length(yhloc)]';