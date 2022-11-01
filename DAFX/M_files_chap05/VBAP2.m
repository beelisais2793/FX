function [gains] = VBAP2(pan_dir)
% function [gains] = VBAP2(pan_dir)
% Author: V. Pulkki
% Computes 2D VBAP gains for horizontal loudspeaker setup.  
% Loudspeaker directions in clockwise or counterclockwise order.
% Change these numbers to match with your system.
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

ls_dirs=[30 -30 -90 -150 150 90];
ls_num=length(ls_dirs);
ls_dirs=[ls_dirs ls_dirs(1)]/180*pi;
% Panning direction in cartesian coordinates.
panvec=[cos(pan_dir/180*pi) sin(pan_dir/180*pi)];
for i=1:ls_num
    % Compute inverse of loudspeaker base matrix.
    lsmat=inv([[cos(ls_dirs(i)) sin(ls_dirs(i))];...
        [cos(ls_dirs(i+1)) sin(ls_dirs(i+1))]]);
    % Compute unnormalized gains
    tempg=panvec*lsmat;
    % If gains nonnegative, normalize the gains and stop
    if min(tempg) > -0.001
        g=zeros(1,ls_num);
        g([i mod(i,ls_num)+1])=tempg;
        gains=g/sqrt(sum(g.^2));
        return
    end
end