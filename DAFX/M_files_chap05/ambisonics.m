% ambisonics.m
% Author: V. Pulkki
% Second-order harmonics to compute gains for loudspeakers
% to position virtual source to a loudspeaker setup
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

theta=30;% Direction of virtual source
loudsp_dir=[30 -30 -90 -150 150 90]/180*pi; % loudspeaker setup 
ls_num=length(loudsp_dir);
harmC=[1 2/3 1/6]; % Coeffs for harmonics "smooth solution", [MON00]
theta=theta/180*pi;
for i=1:ls_num
    g(i)= (harmC(1) + 2*cos(theta-loudsp_dir(i))*harmC(2) +...
        2*cos(2*(theta-loudsp_dir(i)))*harmC(3));
end
% use gains in g for amplitude panning 